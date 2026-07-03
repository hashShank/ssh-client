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
    int server_fd, new_socket;
    struct sockaddr_in address;
    socklen_t addrlen = sizeof(address);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    bind(server_fd, (struct sockaddr*)&address, sizeof(address));
    listen(server_fd, 1);
    std::cout << "[Server] Waiting for connection on port " << PORT << "...\n";

    new_socket = accept(server_fd, (struct sockaddr*)&address, &addrlen);
    std::cout << "[Server] Client connected.\n";

    // --- GENERATE RSA KEYS (Identity) ---
    EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, NULL);
    EVP_PKEY_keygen_init(ctx);
    EVP_PKEY_CTX_set_rsa_keygen_bits(ctx, 2048);
    EVP_PKEY *rsa_keypair = NULL;
    EVP_PKEY_keygen(ctx, &rsa_keypair);
    EVP_PKEY_CTX_free(ctx);

    BIO *bio = BIO_new(BIO_s_mem());
    PEM_write_bio_PUBKEY(bio, rsa_keypair);
    char* rsa_pub_key_str;
    long pub_len = BIO_get_mem_data(bio, &rsa_pub_key_str);
    
    send(new_socket, &pub_len, sizeof(pub_len), 0);
    send(new_socket, rsa_pub_key_str, pub_len, 0);
    BIO_free(bio);

    // --- DIFFIE-HELLMAN + RSA SIGNATURE ---
    DiffieHellman dh;
    uint64_t serverPubKey = dh.getPublicKey();

    EVP_MD_CTX *md_ctx = EVP_MD_CTX_new();
    EVP_DigestSignInit(md_ctx, NULL, EVP_sha256(), NULL, rsa_keypair);
    EVP_DigestSignUpdate(md_ctx, &serverPubKey, sizeof(serverPubKey));
    
    size_t sig_len;
    EVP_DigestSignFinal(md_ctx, NULL, &sig_len);
    unsigned char* signature = (unsigned char*)malloc(sig_len);
    EVP_DigestSignFinal(md_ctx, signature, &sig_len);
    EVP_MD_CTX_free(md_ctx);

    send(new_socket, &serverPubKey, sizeof(serverPubKey), 0);
    send(new_socket, &sig_len, sizeof(sig_len), 0);
    send(new_socket, signature, sig_len, 0);
    free(signature);

    uint64_t clientPubKey;
    read(new_socket, &clientPubKey, sizeof(clientPubKey));
    uint64_t sharedSecret = dh.computeSharedSecret(clientPubKey);
    std::cout << "[Server] Shared secret established.\n";

    // --- READ DATA FROM CLIENT ---
    int encrypted_len;
    read(new_socket, &encrypted_len, sizeof(encrypted_len)); // Read length first

    unsigned char encrypted[1024];
    read(new_socket, encrypted, encrypted_len); // Read exact ciphertext bytes

    unsigned char received_hmac[32];
    read(new_socket, received_hmac, 32); // SHA256 HMAC is always exactly 32 bytes

    // --- NEW: VERIFY HMAC ---
    unsigned char computed_hmac[32];
    unsigned int hmac_len = 0;
    HMAC(EVP_sha256(), &sharedSecret, sizeof(sharedSecret), encrypted, encrypted_len, computed_hmac, &hmac_len);

    if (memcmp(received_hmac, computed_hmac, 32) != 0) {
        std::cerr << "[Server] ERROR: HMAC Verification Failed! Packet tampered with.\n";
        close(new_socket);
        return 1;
    }
    std::cout << "[Server] HMAC Verified! Message integrity is intact.\n";

    // --- AES-128 DECRYPTION ---
    unsigned char aes_key[16] = {0}; 
    memcpy(aes_key, &sharedSecret, sizeof(sharedSecret));
    unsigned char iv[16] = {0}; 

    unsigned char decrypted[1024];
    int decrypted_len = 0, len = 0;

    EVP_CIPHER_CTX *aes_ctx = EVP_CIPHER_CTX_new();
    EVP_DecryptInit_ex(aes_ctx, EVP_aes_128_cbc(), NULL, aes_key, iv);
    EVP_DecryptUpdate(aes_ctx, decrypted, &len, (unsigned char*)encrypted, encrypted_len);
    decrypted_len = len;
    EVP_DecryptFinal_ex(aes_ctx, decrypted + len, &len);
    decrypted_len += len;
    EVP_CIPHER_CTX_free(aes_ctx);

    decrypted[decrypted_len] = '\0';
    std::cout << "[Server] Decrypted AES message: " << decrypted << "\n";

    close(new_socket);
    close(server_fd);
    EVP_PKEY_free(rsa_keypair);
    return 0;
}