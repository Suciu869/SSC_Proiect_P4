#pragma once

#include <cstddef>

// 64 B = o linie de cache = un registru AVX-512
const std::size_t DEFAULT_ALIGNMENT = 64;

// Buffer aliniat la `alignment`, deplasat cu `offset` octeti. Un offset nenul
// produce intentionat date nealiniate, pentru testul load vs loadu. Pentru
// float, offset-ul trebuie sa fie multiplu de 4.
class AlignedBuffer {
public:
    explicit AlignedBuffer(std::size_t bytes,
                           std::size_t alignment = DEFAULT_ALIGNMENT,
                           std::size_t offset = 0);
    ~AlignedBuffer();

    AlignedBuffer(const AlignedBuffer&) = delete;
    AlignedBuffer& operator=(const AlignedBuffer&) = delete;

    template <typename T>
    T* as() {
        return reinterpret_cast<T*>(data_);
    }

private:
    unsigned char* raw_;
    unsigned char* data_;
    std::size_t alignment_;
};
