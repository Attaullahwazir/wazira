// Shared TypeScript interfaces for search queries/results

export interface SearchQuery {
  query: string;
  answer?: boolean;
}

export interface SearchResult {
  results: string[];
  answer?: string;
}

export interface EncryptedQuery {
  ciphertext: string; // Base64-encoded encrypted query
  query?: string;
  answer?: boolean;
}

export interface EncryptedResult {
  ciphertext: string; // Base64-encoded encrypted result
  results: string[];
  answer?: string;
}

export interface Settings {
  adBlock: boolean;
  antiFingerprint: boolean;
  federatedLearning: boolean;
} 