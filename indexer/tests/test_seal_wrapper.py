import seal_wrapper

def test_seal_wrapper():
    seal_wrapper.seal_init()
    ct = seal_wrapper.encrypt_vector([1.0, 2.0, 3.0])
    print('Encrypted:', ct)
    pt = seal_wrapper.decrypt_vector(ct)
    print('Decrypted:', pt)
    dot = seal_wrapper.encrypted_dot(ct, ct)
    print('Encrypted dot product:', dot)

if __name__ == '__main__':
    test_seal_wrapper() 