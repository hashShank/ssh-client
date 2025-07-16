cmake_minimum_required(VERSION 3.10)
project(ssh-client)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories
include_directories(include)

# Source files
set(CRYPTO_SRC src/crypto/DiffieHellman.cpp)

# Unencrypted client/server
add_executable(ssh_unencrypted src/main.cpp)

# Encrypted client and server
add_executable(ssh_client_encrypted src/main_client.cpp ${CRYPTO_SRC})
add_executable(ssh_server_encrypted src/main_server.cpp ${CRYPTO_SRC})
