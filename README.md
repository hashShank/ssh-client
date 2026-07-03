# Secure SSH Client-Server Implementation

This project is a simplified SSH-like client and server built in C++. It demonstrates fundamental networking and enterprise-grade cryptography concepts, including TCP sockets, RSA authentication, Diffie-Hellman key exchange, AES-128 encryption, and HMAC message integrity.

## Features

* Simplified SSH-style communication over TCP
* RSA Signatures for server authentication (preventing MITM attacks)
* Diffie-Hellman (DH) key exchange for establishing an ephemeral shared secret
* AES-128-CBC block cipher encryption for secure data transmission
* HMAC-SHA256 for guaranteed message integrity and tamper detection
* Both encrypted and unencrypted client/server modes

## Components

The project is structured around the following modules:

1. Unencrypted Client/Server – Basic POSIX TCP client/server setup
2. Encrypted Client/Server – Uses OpenSSL for RSA/AES/HMAC and custom DH math
3. DiffieHellman – Custom mathematical implementation of Diffie-Hellman key exchange
4. Network/Socket Headers – POSIX C socket definitions (MySocket.hpp)
5. CMakeLists.txt – Modular build configuration linked with OpenSSL

## Dependencies

* C++17-compatible compiler
* cmake (VERSION >= 3.10)
* OpenSSL (Required for RSA, AES, and HMAC implementations)

macOS Users: Install OpenSSL via Homebrew before building:
    brew install openssl

## Build Instructions

    mkdir build
    cd build
    cmake -DOPENSSL_ROOT_DIR=$(brew --prefix openssl) ..
    make

## Usage

### Run Unencrypted Version
Open a terminal and run:
    ./ssh_unencrypted

This demonstrates basic client-server socket communication without the cryptography layer.

### Run Encrypted Version
Use two separate terminal windows:

Terminal 1 (Server):
    ./ssh_server_encrypted

Terminal 2 (Client):
    ./ssh_client_encrypted

This executes the full secure pipeline: RSA server verification, DH key exchange, and sends an AES-encrypted, HMAC-verified message from the client to the server.

## Implementation Details & Protocol Flow

1. Identity (RSA): The server signs its Diffie-Hellman public key with its RSA Private Key. The client verifies this signature using the Server's RSA Public Key to ensure authenticity.
2. Key Exchange (DH): Both parties compute a Shared Secret using Diffie-Hellman. This secret is ephemeral and used only for the current session.
3. Encryption (AES): The Shared Secret is used to derive a 128-bit key for AES-CBC. All subsequent messages are encrypted using this block cipher.
4. Integrity (HMAC): An HMAC-SHA256 hash is generated using the Shared Secret and attached to the ciphertext. The receiver computes their own HMAC to verify the packet was not altered in transit.

## License
This project is for educational purposes to demonstrate secure network architecture.