#include <iostream>
#include "network/MySocket.hpp"
#include "crypto/DiffieHellman.hpp"

#define PORT 2222

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    socklen_t addrlen = sizeof(address);
    char buffer[1024] = {0};

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        std::cerr << "[Server] Failed to create socket\n";
        return 1;
    }

    // Allow immediate port reuse
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        std::cerr << "[Server] Bind failed\n";
        return 1;
    }

    // Start listening
    if (listen(server_fd, 1) < 0) {
        std::cerr << "[Server] Listen failed\n";
        return 1;
    }

    std::cout << "[Server] Waiting for connection on port " << PORT << "...\n";

    // Accept incoming connection
    new_socket = accept(server_fd, (struct sockaddr*)&address, &addrlen);
    if (new_socket < 0) {
        std::cerr << "[Server] Accept failed\n";
        return 1;
    }

    std::cout << "[Server] Client connected.\n";

    // Diffie-Hellman setup
    DiffieHellman dh;
    uint64_t serverPubKey = dh.getPublicKey();

    uint64_t clientPubKey;
    ssize_t bytesRead = read(new_socket, &clientPubKey, sizeof(clientPubKey));
    if (bytesRead <= 0) {
        std::cerr << "[Server] Failed to receive client public key.\n";
        return 1;
    }

    std::cout << "[Server] Received client public key: " << clientPubKey << "\n";

    send(new_socket, &serverPubKey, sizeof(serverPubKey), 0);
    std::cout << "[Server] Sent server public key: " << serverPubKey << "\n";

    uint64_t sharedSecret = dh.computeSharedSecret(clientPubKey);
    std::cout << "[Server] Shared secret: " << sharedSecret << "\n";

    bytesRead = read(new_socket, buffer, sizeof(buffer));
    if (bytesRead <= 0) {
        std::cerr << "[Server] Failed to receive message from client.\n";
        return 1;
    }

    std::cout << "[Server] Encrypted message received.\n";

    std::string decrypted;
    for (int i = 0; buffer[i] != '\0'; ++i) {
        decrypted += buffer[i] ^ (sharedSecret & 0xFF);
    }

    std::cout << "[Server] Decrypted message: " << decrypted << "\n";

    close(new_socket);
    close(server_fd);
    std::cout << "[Server] Connection closed.\n";

    return 0;
}
