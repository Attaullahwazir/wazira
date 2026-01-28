# Wazira: Privacy-First AI-Powered Search Engine

## Overview
Wazira is a next-generation, privacy-first, AI-powered search engine designed to surpass Google, DuckDuckGo, and Brave. It features modular architecture, privacy and security by design, and advanced AI/ML for search and personalization.

## Architecture
```
[Client (React+TS Extension)] <-> [Service (FastAPI, AI, Privacy)] <-> [Indexer (C++)] <-> [Crawler (C++)]
        |                        |                        |
        |                        |                        |
        +------[Sandbox (VM, JS Instrumentation)]---------+
```
- **Client:** Browser extension (React+TS) with anti-fingerprinting, ad-blocking, and advanced UX
- **Service:** FastAPI backend for secure query processing, BERT reranking, encrypted search, federated learning
- **Indexer:** C++ microservice for tokenization, stemming, inverted index, Merkle diffing
- **Crawler:** C++ distributed crawler with P2P DHT, content-addressed storage, robots.txt, Merkle tree
- **Sandbox:** Firecracker VM manager, JS instrumentation (Frida), behavioral analysis
- **Common:** Shared schemas/types for backend/frontend
- **CI/CD:** Automated lint, test, build for all modules

## Quickstart

### 1. Clone and Install
```
git clone <repo-url>
cd wazira
```

### 2. Build & Run Modules
- **Crawler/Indexer (C++):**
  ```
  cd crawler && cmake . && make
  cd ../indexer && cmake . && make
  ```
- **Service (Python):**
  ```
  cd service && pip install -r requirements.txt
  uvicorn app:app --reload
  ```
- **Sandbox (Python):**
  ```
  cd sandbox && pip install -r requirements.txt
  python firecracker_manager.py --help
  ```
- **Client Extension (React+TS):**
  ```
  cd client/extension
  npm install
  npm run build
  # Load as unpacked extension in Chrome
  ```

### 3. Run CI Locally
- Python: `flake8 service/ sandbox/ && pytest service/ sandbox/`
- C++: `cd crawler && make && cd ../indexer && make`
- Node: `cd client/extension && npm run build && npx eslint src`

## Common Interfaces
- Shared schemas: `common/schemas.py` (Python, Pydantic), `common/types.ts` (TypeScript)
- **Keep in sync:** Update both files when changing data models for search queries/results.

## Contributing
- Follow modular, test-driven, and privacy-first principles
- Update the `progress` file with each major change
- See `docs/` for detailed plans and design docs

## License
[MIT License]

## Deployment & E2E Testing

- Use `docker-compose.yml` to launch all services: crawler, indexer, service, sandbox, client, and NGINX reverse proxy.
- NGINX proxies:
  - `/api/` → FastAPI service (port 8000)
  - `/sandbox/` → Sandbox (port 5005)
  - `/` → Client extension (port 3000)
- To start:
  ```
  docker-compose up --build
  ```
- To run end-to-end tests:
  ```
  python ci/e2e_test.py
  ```
- See `ci/nginx.conf` for proxy config.
- See `ci/e2e_test.py` for E2E test logic. 