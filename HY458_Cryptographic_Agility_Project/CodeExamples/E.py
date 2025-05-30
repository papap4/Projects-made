#!/usr/bin/env python3
"""
test_python.py

A test file for various cryptographic primitives in Python.
After running your replacement script, the insecure primitives should be updated.
"""

import hashlib
import base64
import hmac

########################################
# PBKDF1 Implementation
########################################

def pbkdf1(password: bytes, salt: bytes, iterations: int, dklen: int) -> bytes:
    """
    Derive a key from a password using PBKDF1 as defined in PKCS #5 v1.5.
    
    Note: PBKDF1 is deprecated. Use PBKDF2 (or better) for production systems.
    
    Parameters:
      password   - The password as bytes.
      salt       - A salt as bytes (typically 8 bytes long).
      iterations - The iteration count (e.g., 1000).
      dklen      - Desired length of the derived key in bytes.
    
    Returns:
      The derived key as bytes.
    
    Raises:
      ValueError if dklen is larger than the hash output size.
    """
    # For this example, we use MD5 as the underlying hash.
    hash_func = hashlib.md5
    hash_size = hash_func().digest_size  # MD5 digest size is 16 bytes

    if dklen > hash_size:
        raise ValueError("Derived key too long for PBKDF1 with the chosen hash function.")

    # Initial digest: password concatenated with salt.
    key = password + salt

    # Perform iterative hashing.
    for _ in range(iterations):
        key = hash_func(key).digest()

    # Truncate the key to the desired length.
    return key[:dklen]

########################################
# Test Functions
########################################

def test_md5():
    # Insecure MD5 usage
    data = "SensitiveData".encode()
    md5_hash = hashlib.md5(data).hexdigest()
    print("MD5:", md5_hash)

def test_sha1():
    # Insecure SHA-1 usage
    data = "SensitiveData".encode()
    sha1_hash = hashlib.sha1(data).hexdigest()
    print("SHA1:", sha1_hash)

def test_sha224():
    # Insecure SHA-224 usage
    data = "SensitiveData".encode()
    sha224_hash = hashlib.sha224(data).hexdigest()
    print("SHA224:", sha224_hash)

def test_base64():
    # Base64 encoding/decoding (should be flagged as insecure)
    data = "SensitiveData".encode()
    encoded = base64.b64encode(data)
    decoded = base64.b64decode(encoded)
    print("Base64 Encoded:", encoded)
    print("Base64 Decoded:", decoded.decode())

def test_saltless_hashing():
    # Duplicate usage of MD5/SHA1 without a salt
    data = "SensitiveData".encode()
    hash1 = hashlib.md5(data).hexdigest()
    hash2 = hashlib.sha1(data).hexdigest()
    print("Saltless MD5:", hash1)
    print("Saltless SHA1:", hash2)

def test_pbkdf1():
    # Test the PBKDF1 implementation
    password = b"password"
    salt = b"12345678"  # 8-byte salt
    iterations = 1000
    dklen = 16  # Maximum for MD5 (16 bytes)
    derived_key = pbkdf1(password, salt, iterations, dklen)
    print("PBKDF1 Derived Key (hex):", derived_key.hex())

def test_hmac():
    # HMAC with insecure hash functions (MD5 and SHA-1)
    key = b"secret"
    msg = b"message"
    hmac_md5 = hmac.new(key, msg, hashlib.md5).hexdigest()
    hmac_sha1 = hmac.new(key, msg, hashlib.sha1).hexdigest()
    print("HMAC MD5:", hmac_md5)
    print("HMAC SHA1:", hmac_sha1)

def test_xor():
    # Simple XOR encryption (insecure)
    def xorEncrypt(data, key):
        return bytes(b ^ key for b in data)
    data = b"SensitiveData"
    encrypted = xorEncrypt(data, 0x5A)
    print("XOR Encrypted:", encrypted)

########################################
# Main Entry Point
########################################

if __name__ == "__main__":
    test_md5()
    test_sha1()
    test_sha224()
    test_base64()
    test_saltless_hashing()
    test_pbkdf1()
    test_hmac()
    test_xor()
