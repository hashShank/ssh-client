#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>

int main() {
    const char* hostname = "localhost";  // or IP of SSH server
    const int port = 22;

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Socket creation failed");
        return 1;
    }

    hostent* server = gethostbyname(hostname);
    if (!server) {
        std::cerr << "No such host: " << hostname << std::endl;
        return 1;
    }

    sockaddr_in serv_addr{}; // zero initializes the structure
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port); // host to network short, converts the port from machine's byte order to network byte order
    std::memcpy(&serv_addr.sin_addr.s_addr, server->h_addr, server->h_length);

    if (connect(sock, (sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection failed");
        return 1;
    }

    char buffer[256]{};
    ssize_t n = read(sock, buffer, 255);
    if (n < 0) {
        perror("Read failed");
        return 1;
    }
    std::cout << "Server: " << buffer;

    // Send SSH version string
    std::string clientVersion = "SSH-2.0-SimpleSSHClient\r\n";
    write(sock, clientVersion.c_str(), clientVersion.length());

    std::cout << "Sent client version.\n";

    close(sock);
    return 0;
}
