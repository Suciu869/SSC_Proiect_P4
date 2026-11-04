#pragma once

#include <cstddef>
#include <cstdint>

const std::uint32_t DEFAULT_SEED = 1;

// xorshift32: aceleasi date la fiecare rulare si pe orice sistem
class Xorshift32 {
public:
    // Seed-ul 0 este interzis: starea ar ramane 0 la nesfarsit
    explicit Xorshift32(std::uint32_t seed = DEFAULT_SEED)
        : state_(seed != 0 ? seed : DEFAULT_SEED) {}

    std::uint32_t next() {
        std::uint32_t x = state_;
        x ^= x << 13;
        x ^= x >> 17;
        x ^= x << 5;
        state_ = x;
        return x;
    }

    // Valori in [0, 1). Fara numere negative, suma de la produsul scalar nu se
    // apropie de 0, unde toleranta relativa nu ar mai avea sens.
    float next_float() {
        return static_cast<float>(next() >> 8) / 16777216.0f;
    }

    std::uint8_t next_u8() {
        return static_cast<std::uint8_t>(next() >> 24);
    }

    void fill(float* dst, std::size_t n) {
        for (std::size_t i = 0; i < n; ++i) {
            dst[i] = next_float();
        }
    }

    void fill(std::uint8_t* dst, std::size_t n) {
        for (std::size_t i = 0; i < n; ++i) {
            dst[i] = next_u8();
        }
    }

private:
    std::uint32_t state_;
};
