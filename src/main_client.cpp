#include <iostream>
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

    DiffieHellman dh;
    uint64_t clientPubKey = dh.getPublicKey();

    // Send client public key
    send(sock, &clientPubKey, sizeof(clientPubKey), 0);
    std::cout << "[Client] Sent public key: " << clientPubKey << "\n";

    // Receive server's public key
    uint64_t serverPubKey;
    read(sock, &serverPubKey, sizeof(serverPubKey));
    std::cout << "[Client] Received server public key: " << serverPubKey << "\n";

    // Compute shared secret
    uint64_t sharedSecret = dh.computeSharedSecret(serverPubKey);
    std::cout << "[Client] Shared secret: " << sharedSecret << "\n";

    // Encrypt and send message
    std::string message = "hello from client!";
    std::string encrypted;
    for (char c : message)
        encrypted += c ^ (sharedSecret & 0xFF);

    send(sock, encrypted.c_str(), encrypted.size() + 1, 0); // include null terminator
    std::cout << "[Client] Encrypted message sent.\n";

    close(sock);
    std::cout << "[Client] Connection closed.\n";
    return 0;
}
