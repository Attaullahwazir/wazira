from pydantic import BaseModel
from typing import List, Optional

class SearchQuery(BaseModel):
    """Plaintext search query."""
    query: str
    answer: Optional[bool] = False

class SearchResult(BaseModel):
    """Search result with optional answer."""
    results: List[str]
    answer: Optional[str] = None

class EncryptedQuery(BaseModel):
    """Encrypted search query (for privacy-preserving search)."""
    ciphertext: str  # Base64-encoded encrypted query
    query: Optional[str] = None
    answer: Optional[bool] = False

class EncryptedResult(BaseModel):
    """Encrypted search result (for privacy-preserving search)."""
    ciphertext: str  # Base64-encoded encrypted result
    results: List[str] = []
    answer: Optional[str] = None 