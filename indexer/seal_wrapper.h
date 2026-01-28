#pragma once
#include <vector>
#include <string>

// Forward declarations for SEAL types (actual includes in .cpp)
namespace seal {
    class SEALContext;
    class PublicKey;
    class SecretKey;
    class Ciphertext;
    class Plaintext;
    class Encryptor;
    class Decryptor;
    class Evaluator;
    class CKKSEncoder;
}

// Initialize SEAL context and keys (singleton for demo)
void seal_init();

// Encrypt a vector of doubles, return Base64 ciphertext
std::string encrypt_vector(const std::vector<double>& vec);

// Decrypt a Base64 ciphertext to a vector of doubles
std::vector<double> decrypt_vector(const std::string& ciphertext_b64);

// Compute encrypted dot product of two Base64 ciphertexts, return Base64 ciphertext
std::string encrypted_dot(const std::string& ciphertext1_b64, const std::string& ciphertext2_b64); 