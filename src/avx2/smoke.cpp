
#include <immintrin.h>
#include <cstdio>

int main() {
    volatile float a[8] = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f};
    volatile float b[8] = {10.0f, 20.0f, 30.0f, 40.0f, 50.0f, 60.0f, 70.0f, 80.0f};

    __m256 va = _mm256_set_ps(a[7], a[6], a[5], a[4], a[3], a[2], a[1], a[0]);
    __m256 vb = _mm256_set_ps(b[7], b[6], b[5], b[4], b[3], b[2], b[1], b[0]);

    float r[8];
    _mm256_storeu_ps(r, _mm256_add_ps(va, vb));

    for (int i = 0; i < 8; ++i) {
        if (r[i] != a[i] + b[i]) {
            std::puts("avx2: EROARE");
            return 1;
        }
    }

    std::puts("avx2: OK");
    return 0;
}
