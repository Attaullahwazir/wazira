#include "seal_wrapper.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

// Try to include SEAL headers
#if __has_include(<seal/seal.h>)
#include <seal/seal.h>
#define SEAL_AVAILABLE 1
#else
#define SEAL_AVAILABLE 0
#endif

#if SEAL_AVAILABLE
using namespace seal;
namespace {
    std::shared_ptr<SEALContext> context;
    std::unique_ptr<CKKSEncoder> encoder;
    std::unique_ptr<KeyGenerator> keygen;
    std::unique_ptr<Encryptor> encryptor;
    std::unique_ptr<Decryptor> decryptor;
    std::unique_ptr<Evaluator> evaluator;
    PublicKey public_key;
    SecretKey secret_key;
    double scale = pow(2.0, 40);
    size_t slot_count = 0;

    void save_keys() {
        std::ofstream pk("seal_public.key", std::ios::binary);
        public_key.save(pk);
        pk.close();
        std::ofstream sk("seal_secret.key", std::ios::binary);
        secret_key.save(sk);
        sk.close();
    }
    void load_keys() {
        std::ifstream pk("seal_public.key", std::ios::binary);
        std::ifstream sk("seal_secret.key", std::ios::binary);
        if (pk && sk) {
            public_key.load(*context, pk);
            secret_key.load(*context, sk);
        }
    }
}
#endif

void seal_init() {
#if SEAL_AVAILABLE
    EncryptionParameters parms(scheme_type::ckks);
    parms.set_poly_modulus_degree(8192);
    parms.set_coeff_modulus(CoeffModulus::Create(8192, {60, 40, 40, 60}));
    context = std::make_shared<SEALContext>(parms);
    encoder = std::make_unique<CKKSEncoder>(*context);
    keygen = std::make_unique<KeyGenerator>(*context);
    public_key = keygen->public_key();
    secret_key = keygen->secret_key();
    encryptor = std::make_unique<Encryptor>(*context, public_key);
    decryptor = std::make_unique<Decryptor>(*context, secret_key);
    evaluator = std::make_unique<Evaluator>(*context);
    slot_count = encoder->slot_count();
    save_keys();
    std::cout << "[seal] SEAL context initialized (real)" << std::endl;
#else
    std::cout << "[seal] SEAL context initialized (dummy)" << std::endl;
#endif
}

std::string encrypt_vector(const std::vector<double>& vec) {
#if SEAL_AVAILABLE
    Plaintext pt;
    encoder->encode(vec, scale, pt);
    Ciphertext ct;
    encryptor->encrypt(pt, ct);
    std::stringstream ss;
    ct.save(ss);
    return ss.str();
#else
    std::cout << "[seal] encrypt_vector called (dummy)" << std::endl;
    return "encrypted_dummy";
#endif
}

std::vector<double> decrypt_vector(const std::string& ciphertext_b64) {
#if SEAL_AVAILABLE
    Ciphertext ct;
    std::stringstream ss(ciphertext_b64);
    ct.load(*context, ss);
    Plaintext pt;
    decryptor->decrypt(ct, pt);
    std::vector<double> result;
    encoder->decode(pt, result);
    return result;
#else
    std::cout << "[seal] decrypt_vector called (dummy)" << std::endl;
    return {1.0, 2.0, 3.0};
#endif
}

std::string encrypted_dot(const std::string& ciphertext1_b64, const std::string& ciphertext2_b64) {
#if SEAL_AVAILABLE
    Ciphertext ct1, ct2;
    std::stringstream ss1(ciphertext1_b64), ss2(ciphertext2_b64);
    ct1.load(*context, ss1);
    ct2.load(*context, ss2);
    Ciphertext result;
    evaluator->multiply(ct1, ct2, result);
    evaluator->rescale_to_next_inplace(result);
    std::stringstream ss;
    result.save(ss);
    return ss.str();
#else
    std::cout << "[seal] encrypted_dot called (dummy)" << std::endl;
    return "encrypted_dot_dummy";
#endif
} 