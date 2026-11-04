#include "common/backends.h"

namespace {

bool always() {
    return true;
}

bool has_sse() {
    return __builtin_cpu_supports("sse4.2");
}

bool has_avx2() {
    return __builtin_cpu_supports("avx2") && __builtin_cpu_supports("fma");
}

bool has_avx512() {
    return __builtin_cpu_supports("avx512f") && __builtin_cpu_supports("avx512vl")
        && __builtin_cpu_supports("avx512bw") && __builtin_cpu_supports("avx512dq");
}

} // namespace

// Un backend nou se adauga cu un director in src/ si o linie aici
const Backend BACKENDS[] = {
    // nume     suport      dot      matmul   conv2d
    {"scalar", always,     nullptr, nullptr, nullptr},
    {"sse",    has_sse,    nullptr, nullptr, nullptr},
    {"avx2",   has_avx2,   nullptr, nullptr, nullptr},
    {"avx512", has_avx512, nullptr, nullptr, nullptr},
};

const std::size_t BACKEND_COUNT = sizeof(BACKENDS) / sizeof(BACKENDS[0]);
