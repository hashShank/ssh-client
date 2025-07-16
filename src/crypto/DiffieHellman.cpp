#include "crypto/DiffieHellman.hpp"
#include <random>   // for secure random generation

DiffieHellman::DiffieHellman() {
    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<uint64_t> dist(2, PRIME - 2);

    privateKey = dist(gen);
    publicKey = modexp(GENERATOR, privateKey, PRIME);
}

uint64_t DiffieHellman::getPublicKey() const {
    return publicKey;
}

uint64_t DiffieHellman::computeSharedSecret(uint64_t otherPublicKey) {
    return modexp(otherPublicKey, privateKey, PRIME);
}

// Helper function: Modular Exponentiation (base^exp % mod)
uint64_t DiffieHellman::modexp(uint64_t base, uint64_t exp, uint64_t mod) {
    uint64_t result = 1;
    base = base % mod;

    while (exp > 0) {
        if (exp % 2 == 1)
            result = (result * base) % mod;
        base = (base * base) % mod;
        exp = exp >> 1;
    }

    return result;
}
