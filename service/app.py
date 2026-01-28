from fastapi import FastAPI, HTTPException, Body, Request, Header
from pydantic import BaseModel
from typing import List
import requests
import onnxruntime as ort
import numpy as np
import os
import urllib.request
from fastapi.responses import JSONResponse, Response
import time
import random
import math
from prometheus_client import Counter, generate_latest, CONTENT_TYPE_LATEST
# Placeholder: Microsoft SEAL integration (seal-python or pybind11)
# import seal  # Uncomment and configure when SEAL is available
# Import SEAL pybind11 binding
try:
    import seal_wrapper
    seal_wrapper.seal_init()
except ImportError:
    seal_wrapper = None
    print("[service] WARNING: seal_wrapper not available. Encrypted queries will not work.")

app = FastAPI(title="Secure Query Processing Service")

# Placeholder for C++ indexer integration (now via REST API)
SEARCH_SERVICE_URL = "http://localhost:8081/search"

# Model paths
BERT_MODEL_PATH = "bert_model.onnx"
T5_MODEL_PATH = "t5_model.onnx"

# Download models if missing (dummy URLs, replace with real ones)
def download_model(url, path):
    if not os.path.exists(path):
        print(f"[service] Downloading model: {path}")
        urllib.request.urlretrieve(url, path)

try:
    # Download BERT/DistilBERT model if missing
    download_model("https://huggingface.co/distilbert-base-uncased/resolve/main/model.onnx", BERT_MODEL_PATH)
    bert_session = ort.InferenceSession(BERT_MODEL_PATH)
    # Download T5 model if missing
    download_model("https://huggingface.co/t5-small/resolve/main/model.onnx", T5_MODEL_PATH)
    t5_session = ort.InferenceSession(T5_MODEL_PATH)
    # Placeholder: Load tokenizer (e.g., from transformers or custom)
    # from transformers import AutoTokenizer
    # tokenizer = AutoTokenizer.from_pretrained('distilbert-base-uncased')
except Exception as e:
    bert_session = None
    t5_session = None
    print(f"[service] WARNING: ONNX models not available: {e}")
    # tokenizer = None

def bert_rerank(query: str, docs: List[str], top_k: int = 10) -> List[dict]:
    if not bert_session:
        return [{"doc": doc, "badge": ""} for doc in docs[:top_k]]
    # TODO: Use real tokenizer and ONNX BERT for reranking
    # For now, use dummy logic
    scored = [(doc, len(doc)) for doc in docs]
    scored.sort(key=lambda x: x[1], reverse=True)
    # Add dummy fact-check badge
    return [{"doc": doc, "badge": ("✅ Fact-Checked" if "example" in doc else "")} for doc, _ in scored[:top_k]]

# Answer extraction using BERT QA (dummy for now)
def extract_answer(query: str, doc: str) -> str:
    if not bert_session:
        return "[Answer extraction not available]"
    # TODO: Use ONNX BERT QA model for answer extraction
    return "[Answer extraction not yet implemented]"

# Placeholder: Homomorphic encryption integration (Microsoft SEAL)
def decrypt_query(ciphertext: str) -> str:
    # TODO: Use SEAL (seal-python or pybind11) to decrypt the query
    return ciphertext  # For demo, treat as plaintext

def encrypt_result(result: List[str]) -> str:
    # TODO: Use SEAL (seal-python or pybind11) to encrypt the result
    return ",".join(result)  # For demo, join as plaintext

class EncryptedQuery(BaseModel):
    ciphertext: str  # Base64-encoded encrypted query vector
    query: str = None
    answer: bool = False  # If true, extract answer from top doc

class EncryptedResult(BaseModel):
    ciphertext: str  # Base64-encoded encrypted result
    results: List[str] = []
    answer: str = None

# In-memory store for federated updates (for demo)
federated_updates = []

API_KEY = os.environ.get('WAZIRA_API_KEY', 'changeme')
RATE_LIMIT = 100  # max requests per IP per hour
rate_limit_store = {}

def check_api_key(x_api_key: str = Header(None)):
    if x_api_key != API_KEY:
        return JSONResponse(status_code=401, content={"error": "Invalid API key"})

# Rate limiting dependency
def check_rate_limit(request: Request):
    ip = request.client.host
    now = int(time.time() // 3600)  # hour bucket
    key = f"{ip}:{now}"
    count = rate_limit_store.get(key, 0)
    if count >= RATE_LIMIT:
        return JSONResponse(status_code=429, content={"error": "Rate limit exceeded"})
    rate_limit_store[key] = count + 1

# Prometheus metrics
search_counter = Counter('search_queries_total', 'Total search queries')
federated_update_counter = Counter('federated_updates_total', 'Total federated learning updates')

@app.get("/metrics")
def metrics():
    return Response(generate_latest(), media_type=CONTENT_TYPE_LATEST)

# Add tracing hooks (OpenTelemetry, dummy for now)
def trace_event(event: str, **kwargs):
    # TODO: Integrate OpenTelemetry tracing
    print(f"[trace] {event}", kwargs)

@app.post("/query", response_model=EncryptedResult)
def submit_encrypted_query(query: EncryptedQuery, request: Request, x_api_key: str = Header(None)):
    resp = check_api_key(x_api_key)
    if resp: return resp
    resp = check_rate_limit(request)
    if resp: return resp
    search_counter.inc()
    trace_event('search_query', query=query.query)
    """
    Receive an encrypted query, process it securely, and return an encrypted result.
    If plaintext query is provided, forward to C++ search microservice, rerank with BERT, and optionally extract answer.
    If ciphertext is provided, decrypt, process, and encrypt the result.
    """
    if query.query:
        try:
            resp = requests.post(SEARCH_SERVICE_URL, json={"query": query.query})
            resp.raise_for_status()
            data = resp.json()
            bm25_results = data.get("results", [])
            reranked = bert_rerank(query.query, bm25_results, top_k=10)
            answer = None
            if query.answer and reranked:
                # For demo: extract answer from top doc (placeholder)
                answer = extract_answer(query.query, reranked[0]["doc"])
            return EncryptedResult(ciphertext=query.ciphertext, results=[doc["doc"] for doc in reranked], answer=answer)
        except Exception as ex:
            raise HTTPException(status_code=500, detail=f"Search service error: {ex}")
    elif query.ciphertext:
        plaintext_query = decrypt_query(query.ciphertext)
        try:
            resp = requests.post(SEARCH_SERVICE_URL, json={"query": plaintext_query})
            resp.raise_for_status()
            data = resp.json()
            bm25_results = data.get("results", [])
            reranked = bert_rerank(plaintext_query, bm25_results, top_k=10)
            encrypted = encrypt_result([doc["doc"] for doc in reranked])
            answer = None
            if query.answer and reranked:
                answer = extract_answer(plaintext_query, reranked[0]["doc"])
            return EncryptedResult(ciphertext=encrypted, results=[], answer=answer)
        except Exception as ex:
            raise HTTPException(status_code=500, detail=f"Encrypted search service error: {ex}")
    return EncryptedResult(ciphertext=query.ciphertext, results=[])

@app.get("/health")
def health_check():
    return {"status": "ok"}

@app.post("/test_encrypted_query")
def test_encrypted_query(vec1: list = Body(...), vec2: list = Body(...), request: Request, x_api_key: str = Header(None)):
    resp = check_api_key(x_api_key)
    if resp: return resp
    resp = check_rate_limit(request)
    if resp: return resp
    """
    Test endpoint: encrypt two vectors, compute encrypted dot product, decrypt result.
    """
    if not seal_wrapper:
        raise HTTPException(status_code=500, detail="SEAL wrapper not available")
    ct1 = seal_wrapper.encrypt_vector(vec1)
    ct2 = seal_wrapper.encrypt_vector(vec2)
    ct_dot = seal_wrapper.encrypted_dot(ct1, ct2)
    # For demo, decrypt_vector returns a vector; in real SEAL, would decrypt dot product result
    pt_dot = seal_wrapper.decrypt_vector(ct_dot)
    return {"dot_product": pt_dot}

# Differential privacy: Laplace mechanism
def add_laplace_noise(value, epsilon=1.0):
    scale = 1.0 / epsilon
    return value + random.gauss(0, scale)

@app.post("/federated_update")
def federated_update(update: dict, request: Request, x_api_key: str = Header(None)):
    resp = check_api_key(x_api_key)
    if resp: return resp
    resp = check_rate_limit(request)
    if resp: return resp
    # Input validation
    if not isinstance(update, dict) or 'noisyDwell' not in update or 'count' not in update:
        return JSONResponse(status_code=400, content={"error": "Invalid update format"})
    # Add Laplace noise for DP
    update['noisyDwell'] = add_laplace_noise(update['noisyDwell'])
    federated_updates.append(update)
    federated_update_counter.inc()
    trace_event('federated_update', update=update)
    return {"status": "received", "num_updates": len(federated_updates)}

# Zero-knowledge proof (ZKP) hook for ad-blocking/personalization
@app.post("/zkp_proof")
def zkp_proof(payload: dict, request: Request, x_api_key: str = Header(None)):
    resp = check_api_key(x_api_key)
    if resp: return resp
    resp = check_rate_limit(request)
    if resp: return resp
    # TODO: Integrate real ZKP logic (e.g., zk-SNARKs, Bulletproofs)
    return {"zkp": "dummy-proof", "status": "ZKP integration planned"}

@app.get("/aggregate_updates")
def aggregate_updates():
    """
    Aggregate all received updates (mean noisyDwell for demo).
    TODO: Use secure aggregation (PySyft or custom).
    """
    if not federated_updates:
        return {"mean_noisyDwell": 0, "count": 0}
    total = sum(u.get("noisyDwell", 0) for u in federated_updates)
    count = sum(u.get("count", 0) for u in federated_updates)
    mean = total / max(count, 1)
    return {"mean_noisyDwell": mean, "count": count, "num_updates": len(federated_updates)}

@app.post("/summarize")
def summarize(payload: dict, request: Request, x_api_key: str = Header(None)):
    resp = check_api_key(x_api_key)
    if resp: return resp
    resp = check_rate_limit(request)
    if resp: return resp
    """
    Summarize a URL or text. Uses ONNX T5 if available, else returns a dummy summary.
    """
    url = payload.get('url')
    text = payload.get('text')
    if t5_session and text:
        # TODO: Tokenize and run ONNX T5 for summarization
        return {"summary": f"[T5 summary] {text[:60]}..."}
    if url:
        summary = f"[Summary for {url}] This is a dummy summary."
    elif text:
        summary = f"[Summary] {text[:60]}..."
    else:
        summary = "No input provided."
    return {"summary": summary}

@app.post("/fact_check")
def fact_check(payload: dict, request: Request, x_api_key: str = Header(None)):
    resp = check_api_key(x_api_key)
    if resp: return resp
    resp = check_rate_limit(request)
    if resp: return resp
    claim = payload.get('claim') or payload.get('url')
    # TODO: Integrate with real fact-checking API or LLM
    if claim and 'example' in claim:
        return {"result": "Likely True", "badge": "✅ Fact-Checked"}
    elif claim:
        return {"result": "Unverified", "badge": "⚠️ Unverified"}
    else:
        return {"result": "No claim provided", "badge": "❓"}

# TODO: Add endpoints for multi-lingual support, summarization, federated learning, and secure aggregation 