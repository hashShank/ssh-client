cmake_minimum_required(VERSION 3.10)
project(ssh-client)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find OpenSSL installed on the system
find_package(OpenSSL REQUIRED)

# Include directories
include_directories(include)
include_directories(${OPENSSL_INCLUDE_DIR})

# Source files
set(CRYPTO_SRC src/crypto/DiffieHellman.cpp)

# Unencrypted client/server
add_executable(ssh_unencrypted src/main.cpp)

# Encrypted client and server
add_executable(ssh_client_encrypted src/main_client.cpp ${CRYPTO_SRC})
add_executable(ssh_server_encrypted src/main_server.cpp ${CRYPTO_SRC})

# Link OpenSSL libraries to the encrypted executables
target_link_libraries(ssh_client_encrypted OpenSSL::SSL OpenSSL::Crypto)
target_link_libraries(ssh_server_encrypted OpenSSL::SSL OpenSSL::Crypto) 

#include directory for openssl "-DOPENSSL_ROOT_DIR=$(brew --prefix openssl)"