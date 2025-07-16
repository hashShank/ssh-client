# SSH Client-Server Implementation (C++)

This project is a simplified SSH-like client and server built from scratch in C++. It demonstrates fundamental networking and encryption concepts including TCP sockets, Diffie-Hellman key exchange, XOR encryption (for demonstration), and basic protocol structure — all using raw system calls and C++17.

## Features

- Simplified SSH-style communication over TCP  
- Diffie-Hellman key exchange for shared secret  
- XOR encryption of messages (for demo purposes)  
- Modular codebase using headers and CMake  
- Both encrypted and unencrypted client/server modes  
- No use of external crypto libraries  

## Components

The project is structured around the following modules:

1. **Unencrypted Client/Server** – Basic TCP client/server setup  
2. **Encrypted Client/Server** – Uses DH key exchange and XOR encryption  
3. **DiffieHellman** – Custom implementation of Diffie-Hellman  
4. **Message Struct** – Handles simple serialization/deserialization  
5. **CMakeLists.txt** – Modular build configuration

## Build Instructions

You'll need `cmake` and a C++17-compatible compiler.

```bash
mkdir build
cd build
cmake ..
make
```

## Usage

### Run Unencrypted Version

Open a terminal and run:

```bash
./ssh_unencrypted
```

This shows basic client-server communication without encryption.

### Run Encrypted Version

Use two terminals:

**Terminal 1 (Server):**

```bash
./ssh_server_encrypted
```

**Terminal 2 (Client):**

```bash
./ssh_client_encrypted
```

This performs a Diffie-Hellman key exchange and sends an XOR-encrypted message from client to server.

## Implementation Details

### Key Exchange

1. Client generates a DH public key and sends it to the server  
2. Server generates its DH public key and sends it back  
3. Both use the received key and their own private key to compute a shared secret  
4. That shared secret is used for XOR encryption

### Encryption

The message is XOR-ed with the shared secret (lower byte) before being sent over the socket.  
Decryption is done the same way on the receiver side.

## Educational Value

This project helps understand:

- TCP socket programming in C++  
- Manual key exchange using Diffie-Hellman  
- Basic encryption/decryption logic  
- Protocol design foundations  
- Modular project structure with CMake

## Disclaimer

This project is for educational purposes only.  

