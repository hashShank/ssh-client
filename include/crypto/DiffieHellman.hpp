#ifndef DIFFIEHELLMAN_HPP
#define DIFFIEHELLMAN_HPP

#include <cstdint>

class DiffieHellman {
public:
    DiffieHellman();

    uint64_t getPublicKey() const;
    uint64_t computeSharedSecret(uint64_t otherPublicKey);

private:
    uint64_t privateKey;
    uint64_t publicKey;

    uint64_t modexp(uint64_t base, uint64_t exp, uint64_t mod);
};

// Default small values for demo
constexpr uint64_t PRIME = 23;
constexpr uint64_t GENERATOR = 5;

#endif
