/*
 * test_crypto.c
 *
 * A test file for various cryptographic primitives in C.
 * Insecure algorithms such as MD5, SHA-1, and DES are used here.
 * After processing with your replacement script, these should be fixed.
 *
 * Compile with:
 *     gcc test_crypto.c -lcrypto -o test_crypto
 */

#include <stdio.h>
#include <string.h>
#include <openssl/md5.h>
#include <openssl/sha.h>
#include <openssl/des.h>
#include <openssl/evp.h>

void test_md5() {
    unsigned char digest[MD5_DIGEST_LENGTH];
    const char *message = "SensitiveData";
    MD5((unsigned char*)message, strlen(message), digest);
    printf("MD5: ");
    for (int i = 0; i < MD5_DIGEST_LENGTH; i++)
        printf("%02x", digest[i]);
    printf("\n");
}

void test_sha1() {
    unsigned char digest[SHA_DIGEST_LENGTH];
    const char *message = "SensitiveData";
    SHA1((unsigned char*)message, strlen(message), digest);
    printf("SHA1: ");
    for (int i = 0; i < SHA_DIGEST_LENGTH; i++)
        printf("%02x", digest[i]);
    printf("\n");
}

void test_des() {
    // Insecure DES usage.
    // This code uses DES_cblock and related functions.
    DES_cblock key = {0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08};
    DES_key_schedule schedule;
    if (DES_set_key_checked(&key, &schedule) != 0) {
        fprintf(stderr, "DES key error\n");
        return;
    }
    DES_cblock input = {0};  // All-zero input
    DES_cblock output = {0};
    DES_ecb_encrypt(&input, &output, &schedule, DES_ENCRYPT);
    printf("DES encryption performed.\n");
}

void test_pbkdf1() {
    // Placeholder for PBKDF1 usage.
    printf("PBKDF1 test placeholder.\n");
}

void test_xor() {
    // Simple XOR encryption (insecure)
    char data[] = "SensitiveData";
    char key = 0x5A;
    size_t len = strlen(data);
    for (size_t i = 0; i < len; i++) {
        data[i] ^= key;
    }
    printf("XOR encrypted data: %s\n", data);
}

int main() {
    test_md5();
    test_sha1();
    test_des();
    test_pbkdf1();
    test_xor();
    return 0;
}
