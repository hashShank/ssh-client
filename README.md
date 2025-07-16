# SSH Client-Server Implementation (C++)

This project is a simplified implementation of an SSH-like client and server built entirely from scratch in C++. It demonstrates core concepts such as client-server communication, encryption using classic Diffie-Hellman, and basic custom networking without relying on system socket headers or external cryptographic libraries.


---

## Features

- Simplified SSH-style communication over TCP
- Optional Diffie-Hellman key exchange for encrypted messaging
- Clean modular code structure using CMake
- Completely self-contained — no external libraries
- Custom replacements for `sys/socket.h`, `netinet/in.h`, etc.

---

## Components

The project is structured around the following modules:

- `main.cpp` – Unencrypted demo client-server communication
- `main_client.cpp` – Encrypted client using DH
- `main_server.cpp` – Encrypted server using DH
- `include/crypto/DiffieHellman.hpp` – Basic DH key exchange
- `include/network/MySocket.hpp` – Custom networking structs/functions
- `src/crypto/DiffieHellman.cpp` – Implementation of DH
- `CMakeLists.txt` – Modular CMake build setup

---

## Build Instructions

You’ll need `cmake` and a C++17-compatible compiler.

```bash
git clone https://github.com/your-username/ssh-client.git
cd ssh-client
mkdir build
cd build
cmake ..
make

🚀 Usage
Run Unencrypted (for demo)

./ssh_unencrypted

This shows basic client-server communication without encryption.
Run Encrypted

Start in two terminals:

# Terminal 1
./ssh_server_encrypted

# Terminal 2
./ssh_client_encrypted

This will perform a Diffie-Hellman key exchange and send an XOR-encrypted message.
Implementation Highlights

    Classic Diffie-Hellman (small prime) key exchange

    XOR-based encryption (for demo only)

    No system-level socket includes — replaced with manual struct/function definitions

    Modular C++ architecture

    Custom modexp() used for safe exponentiation

Educational Value

This project demonstrates:

    TCP socket programming in C++

    Manual key exchange with Diffie-Hellman

    Simple encryption/decryption

    Protocol design basics

    CMake-based modular project structure

License

This project is for educational use only.
Do not use for real-world security applications.