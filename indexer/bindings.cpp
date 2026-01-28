#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "seal_wrapper.h"

namespace py = pybind11;

PYBIND11_MODULE(seal_wrapper, m) {
    m.doc() = "Microsoft SEAL wrapper for encrypted search (CKKS)";
    m.def("seal_init", &seal_init, "Initialize SEAL context and keys");
    m.def("encrypt_vector", &encrypt_vector, "Encrypt a vector of doubles, return Base64 ciphertext");
    m.def("decrypt_vector", &decrypt_vector, "Decrypt a Base64 ciphertext to a vector of doubles");
    m.def("encrypted_dot", &encrypted_dot, "Compute encrypted dot product of two Base64 ciphertexts");
} 