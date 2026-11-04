
#include <immintrin.h>
#include <cstdio>

int main() {
    volatile float a[16];
    volatile float b[16];
    for (int i = 0; i < 16; ++i) {
        a[i] = static_cast<float>(i + 1);
        b[i] = static_cast<float>((i + 1) * 10);
    }

    float ia[16];
    float ib[16];
    for (int i = 0; i < 16; ++i) {
        ia[i] = a[i];
        ib[i] = b[i];
    }

    __m512 va = _mm512_loadu_ps(ia);
    __m512 vb = _mm512_loadu_ps(ib);

    float r[16];
    _mm512_storeu_ps(r, _mm512_add_ps(va, vb));

    for (int i = 0; i < 16; ++i) {
        if (r[i] != a[i] + b[i]) {
            std::puts("avx512: EROARE");
            return 1;
        }
    }

    std::puts("avx512: OK");
    return 0;
}
