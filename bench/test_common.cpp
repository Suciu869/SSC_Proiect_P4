// Teste pentru src/common. Se ruleaza cu `make test`.

#include "common/affinity.h"
#include "common/aligned_buffer.h"
#include "common/backends.h"
#include "common/rng.h"
#include "common/timer.h"
#include "common/validate.h"

#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <limits>
#include <stdexcept>
#include <thread>

#ifdef _WIN32
#include <windows.h>
#else
#include <sched.h>
#endif

namespace {

int failures = 0;

void check(bool condition, const char* name) {
    std::printf("  [%s] %s\n", condition ? "OK" : "EROARE", name);
    if (!condition) {
        ++failures;
    }
}

std::uintptr_t address(void* p) {
    return reinterpret_cast<std::uintptr_t>(p);
}

void test_aligned_buffer() {
    std::puts("AlignedBuffer");

    AlignedBuffer implicit(1024);
    check(address(implicit.as<float>()) % 64 == 0, "aliniere implicita la 64 B");

    AlignedBuffer aligned32(1024, 32);
    check(address(aligned32.as<float>()) % 32 == 0, "aliniere la 32 B");

    AlignedBuffer shifted(1024, 64, 4);
    check(address(shifted.as<float>()) % 64 == 4, "buffer deplasat cu 4 B");

    std::memset(shifted.as<unsigned char>(), 0xAB, 1024);
    check(shifted.as<unsigned char>()[1023] == 0xAB, "buffer-ul deplasat are toti octetii ceruti");

    bool thrown = false;
    try {
        AlignedBuffer invalid(1024, 3);
    } catch (const std::invalid_argument&) {
        thrown = true;
    }
    check(thrown, "alinierea care nu e putere a lui 2 este respinsa");
}

void test_rng() {
    std::puts("Xorshift32");

    Xorshift32 known(1);
    check(known.next() == 270369u, "valoarea de referinta pentru seed 1");

    Xorshift32 first(42);
    Xorshift32 second(42);
    bool identical = true;
    for (int i = 0; i < 1000; ++i) {
        identical = identical && first.next() == second.next();
    }
    check(identical, "acelasi seed produce aceleasi date");

    Xorshift32 zero(0);
    check(zero.next() != 0, "seed-ul 0 nu blocheaza generatorul");

    Xorshift32 values(7);
    bool in_range = true;
    for (int i = 0; i < 100000; ++i) {
        const float value = values.next_float();
        in_range = in_range && value >= 0.0f && value < 1.0f;
    }
    check(in_range, "valorile float sunt in [0, 1)");
}

void test_timer() {
    std::puts("Timer");

    Timer timer;
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    const double first = timer.elapsed_ns();
    const double second = timer.elapsed_ns();
    check(first >= 5e6, "masoara o pauza de 10 ms");
    check(second >= first, "timpul nu scade");
}

void test_validate() {
    std::puts("Validare");

    const float nan = std::numeric_limits<float>::quiet_NaN();
    const float inf = std::numeric_limits<float>::infinity();

    check(nearly_equal(1000.0f, 1000.001f), "accepta o diferenta foarte mica");
    check(!nearly_equal(1.0f, 1.01f), "respinge o diferenta mare");
    check(nearly_equal(0.0f, 0.0f), "0 este egal cu 0");
    check(!nearly_equal(nan, nan), "NaN nu este acceptat");
    check(!nearly_equal(inf, 1.0f), "infinit nu este egal cu o valoare finita");

    const float a[3] = {1.0f, 2.0f, 3.0f};
    const float b[3] = {1.0f, 2.0f, 3.5f};
    check(nearly_equal(a, a, 3), "vectori float egali");
    check(!nearly_equal(a, b, 3), "vectori float diferiti");

    const std::uint8_t p[3] = {1, 2, 3};
    const std::uint8_t q[3] = {1, 2, 4};
    check(exactly_equal(p, p, 3), "pixeli egali");
    check(!exactly_equal(p, q, 3), "pixeli care difera cu 1");
}

int current_core() {
#ifdef _WIN32
    return static_cast<int>(GetCurrentProcessorNumber());
#else
    return sched_getcpu();
#endif
}

void test_affinity() {
    std::puts("Fixarea pe nucleu");

    check(pin_to_core(1), "fixarea pe nucleul 1 reuseste");
    check(current_core() == 1, "firul ruleaza chiar pe nucleul 1");
    check(!pin_to_core(-1), "nucleu negativ respins");
    check(!pin_to_core(1000), "nucleu inexistent respins");
}

void test_backends() {
    std::puts("Tabelul de backend-uri");

    check(BACKEND_COUNT == 4, "tabelul are 4 backend-uri");
    check(std::strcmp(BACKENDS[0].name, "scalar") == 0, "primul backend este referinta scalara");
    check(BACKENDS[0].supported(), "scalar este mereu disponibil");
}

} // namespace

int main() {
    test_aligned_buffer();
    test_rng();
    test_timer();
    test_validate();
    test_affinity();
    test_backends();

    if (failures != 0) {
        std::printf("\n%d teste au esuat\n", failures);
        return 1;
    }
    std::puts("\nToate testele au trecut");
    return 0;
}
