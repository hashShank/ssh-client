#ifndef MY_SOCKET_HPP
#define MY_SOCKET_HPP

#include <cstdint>

#define AF_INET 2
#define SOCK_STREAM 1
#define INADDR_ANY 0x00000000
#define SOL_SOCKET 1
#define SO_REUSEADDR 2
#define SO_REUSEPORT 15

typedef uint32_t in_addr_t;
typedef uint16_t sa_family_t;
typedef uint16_t in_port_t;
typedef unsigned int socklen_t;

struct in_addr {
    in_addr_t s_addr;
};

struct sockaddr_in {
    sa_family_t sin_family;
    in_port_t sin_port;
    struct in_addr sin_addr;
    unsigned char sin_zero[8];
};

struct sockaddr {
    sa_family_t sa_family;
    char sa_data[14];
};

// Function declarations (system calls, implemented in libc)
extern "C" {
    int socket(int domain, int type, int protocol);
    int bind(int sockfd, const struct sockaddr* addr, socklen_t addrlen);
    int listen(int sockfd, int backlog);
    int accept(int sockfd, struct sockaddr* addr, socklen_t* addrlen);
    int connect(int sockfd, const struct sockaddr* addr, socklen_t addrlen);
    int setsockopt(int sockfd, int level, int optname, const void* optval, socklen_t optlen);
    int close(int fd);
    ssize_t read(int fd, void* buf, size_t count);
    ssize_t write(int fd, const void* buf, size_t count);
    int send(int sockfd, const void* buf, size_t len, int flags);
    int inet_pton(int af, const char* src, void* dst);
}

#endif
