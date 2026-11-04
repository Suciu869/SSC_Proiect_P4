// Detecteaza extensiile SIMD ale procesorului si verifica daca sistemul de
// operare salveaza registrele extinse. Fara acest suport, instructiunile AVX
// si AVX-512 ar genera exceptie, chiar daca procesorul le are.
//
// Se compileaza fara -mavx*, ca programul sa ruleze pe orice procesor x86-64.

#include <cpuid.h>
#include <cstdint>
#include <cstdio>

namespace {

const std::uint32_t CPUID1_ECX_OSXSAVE = 1u << 27;

// Bitii din registrul XCR0, activati de sistemul de operare
const std::uint64_t XCR0_SSE    = 1u << 1;
const std::uint64_t XCR0_YMM    = 1u << 2;
const std::uint64_t XCR0_OPMASK = 1u << 5;
const std::uint64_t XCR0_ZMM_HI = 1u << 6;
const std::uint64_t XCR0_ZMM_16 = 1u << 7;

const std::uint64_t XCR0_AVX    = XCR0_SSE | XCR0_YMM;
const std::uint64_t XCR0_AVX512 = XCR0_AVX | XCR0_OPMASK | XCR0_ZMM_HI | XCR0_ZMM_16;

bool has_osxsave() {
    unsigned eax, ebx, ecx, edx;
    if (!__get_cpuid(1, &eax, &ebx, &ecx, &edx)) {
        return false;
    }
    return (ecx & CPUID1_ECX_OSXSAVE) != 0;
}

// Asm direct, pentru ca _xgetbv() ar cere compilarea cu -mxsave
std::uint64_t read_xcr0() {
    std::uint32_t lo, hi;
    __asm__ volatile("xgetbv" : "=a"(lo), "=d"(hi) : "c"(0));
    return (static_cast<std::uint64_t>(hi) << 32) | lo;
}

void print_row(const char* name, bool ok) {
    std::printf("  %-12s %s\n", name, ok ? "DA" : "NU");
}

} // namespace

int main() {
    __builtin_cpu_init();

    const bool sse42    = __builtin_cpu_supports("sse4.2");
    const bool avx2     = __builtin_cpu_supports("avx2");
    const bool fma      = __builtin_cpu_supports("fma");
    const bool avx512f  = __builtin_cpu_supports("avx512f");
    const bool avx512vl = __builtin_cpu_supports("avx512vl");
    const bool avx512bw = __builtin_cpu_supports("avx512bw");
    const bool avx512dq = __builtin_cpu_supports("avx512dq");

    std::puts("Extensii SIMD:");
    print_row("SSE4.2", sse42);
    print_row("AVX2", avx2);
    print_row("FMA", fma);
    print_row("AVX-512 F", avx512f);
    print_row("AVX-512 VL", avx512vl);
    print_row("AVX-512 BW", avx512bw);
    print_row("AVX-512 DQ", avx512dq);

    // XGETBV exista doar daca OSXSAVE este activ
    const bool osxsave = has_osxsave();
    const std::uint64_t xcr0 = osxsave ? read_xcr0() : 0;
    const bool os_avx    = (xcr0 & XCR0_AVX) == XCR0_AVX;
    const bool os_avx512 = (xcr0 & XCR0_AVX512) == XCR0_AVX512;

    std::puts("\nSuport din sistemul de operare:");
    print_row("OSXSAVE", osxsave);
    print_row("YMM (AVX)", os_avx);
    print_row("ZMM (AVX512)", os_avx512);
    std::printf("  XCR0 = 0x%llx\n", static_cast<unsigned long long>(xcr0));

    std::puts("\nBackend-uri utilizabile:");
    print_row("scalar", true);
    print_row("sse", sse42);
    print_row("avx2", avx2 && fma && os_avx);
    print_row("avx512", avx512f && avx512vl && avx512bw && avx512dq && os_avx512);

    return 0;
}
