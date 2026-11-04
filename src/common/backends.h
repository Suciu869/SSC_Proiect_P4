#pragma once

#include <cstddef>
#include <cstdint>

using DotFn = float (*)(const float* a, const float* b, std::size_t n);
using MatmulFn = void (*)(const float* a, const float* b, float* c, std::size_t n);

// `kernel` are 9 coeficienti; suma ponderata se imparte la 2^shift
using Conv2dFn = void (*)(const std::uint8_t* src, std::uint8_t* dst,
                          std::size_t width, std::size_t height,
                          const std::int16_t* kernel, int shift);

// O linie din tabelul algoritm x backend. nullptr = varianta neimplementata.
struct Backend {
    const char* name;
    bool (*supported)();
    DotFn dot;
    MatmulFn matmul;
    Conv2dFn conv2d;
};

extern const Backend BACKENDS[];
extern const std::size_t BACKEND_COUNT;
