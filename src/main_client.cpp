#include <iostream>
#include <cstring>
#include <openssl/evp.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/hmac.h> // Added for HMAC
#include "network/MySocket.hpp"
#include "crypto/DiffieHellman.hpp"

#define PORT 2222

int main() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serv_addr{};
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "[Client] Connection Failed\n";
        return 1;
    }
    std::cout << "[Client] Connected to server.\n";

    // --- RECEIVE RSA PUBLIC KEY ---
    long pub_len;
    read(sock, &pub_len, sizeof(pub_len));
    char rsa_pub_key_str[2048] = {0};
    read(sock, rsa_pub_key_str, pub_len);

    BIO *bio = BIO_new_mem_buf(rsa_pub_key_str, -1);
    EVP_PKEY *server_rsa_pubkey = PEM_read_bio_PUBKEY(bio, NULL, NULL, NULL);
    BIO_free(bio);

    // --- RECEIVE DH KEY & VERIFY RSA SIGNATURE ---
    uint64_t serverPubKey;
    read(sock, &serverPubKey, sizeof(serverPubKey));
    
    size_t sig_len;
    read(sock, &sig_len, sizeof(sig_len));
    unsigned char signature[256];
    read(sock, signature, sig_len);

    EVP_MD_CTX *md_ctx = EVP_MD_CTX_new();
    EVP_DigestVerifyInit(md_ctx, NULL, EVP_sha256(), NULL, server_rsa_pubkey);
    EVP_DigestVerifyUpdate(md_ctx, &serverPubKey, sizeof(serverPubKey));
    
    if (EVP_DigestVerifyFinal(md_ctx, signature, sig_len) == 1) {
        std::cout << "[Client] RSA Signature Verified! Server is authentic.\n";
    } else {
        std::cerr << "[Client] WARNING: Signature Verification Failed! MITM Attack Detected.\n";
        close(sock);
        return 1;
    }
    EVP_MD_CTX_free(md_ctx);

    // --- DIFFIE HELLMAN EXCHANGE ---
    DiffieHellman dh;
    uint64_t clientPubKey = dh.getPublicKey();
    send(sock, &clientPubKey, sizeof(clientPubKey), 0);

    uint64_t sharedSecret = dh.computeSharedSecret(serverPubKey);
    std::cout << "[Client] Shared secret established.\n";

    // --- AES-128 ENCRYPTION ---
    std::string message = "hello from client, using AES and HMAC!";
    unsigned char encrypted[1024]; 
    int encrypted_len = 0, len = 0;

    unsigned char aes_key[16] = {0}; 
    memcpy(aes_key, &sharedSecret, sizeof(sharedSecret));
    unsigned char iv[16] = {0}; 

    EVP_CIPHER_CTX *aes_ctx = EVP_CIPHER_CTX_new();
    EVP_EncryptInit_ex(aes_ctx, EVP_aes_128_cbc(), NULL, aes_key, iv);
    EVP_EncryptUpdate(aes_ctx, encrypted, &len, (unsigned char*)message.c_str(), message.length());
    encrypted_len = len;
    EVP_EncryptFinal_ex(aes_ctx, encrypted + len, &len);
    encrypted_len += len;
    EVP_CIPHER_CTX_free(aes_ctx);

    // --- NEW: GENERATE HMAC-SHA256 ---
    unsigned char hmac_result[32];
    unsigned int hmac_len = 0;
    // We hash the ENCRYPTED message using the Shared Secret as the key
    HMAC(EVP_sha256(), &sharedSecret, sizeof(sharedSecret), encrypted, encrypted_len, hmac_result, &hmac_len);

    // --- SEND DATA (Length, Ciphertext, HMAC) ---
    send(sock, &encrypted_len, sizeof(encrypted_len), 0); // Tell server how long the message is
    send(sock, encrypted, encrypted_len, 0);              // Send encrypted message
    send(sock, hmac_result, hmac_len, 0);                 // Send HMAC signature
    
    std::cout << "[Client] AES Encrypted message + HMAC sent.\n";

    close(sock);
    EVP_PKEY_free(server_rsa_pubkey);
    return 0;
}