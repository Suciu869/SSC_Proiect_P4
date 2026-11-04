
#include <immintrin.h>
#include <cstdio>

int main() {
    volatile float a[4] = {1.0f, 2.0f, 3.0f, 4.0f};
    volatile float b[4] = {10.0f, 20.0f, 30.0f, 40.0f};

    __m128 va = _mm_set_ps(a[3], a[2], a[1], a[0]);
    __m128 vb = _mm_set_ps(b[3], b[2], b[1], b[0]);

    float r[4];
    _mm_storeu_ps(r, _mm_add_ps(va, vb));

    for (int i = 0; i < 4; ++i) {
        if (r[i] != a[i] + b[i]) {
            std::puts("sse: EROARE");
            return 1;
        }
    }

    std::puts("sse: OK");
    return 0;
}
