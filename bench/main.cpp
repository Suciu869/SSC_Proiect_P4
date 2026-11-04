// Parcurge tabelul algoritm x backend si sare peste variantele indisponibile.
// Masuratorile se adauga in Lab 4.

#include "common/backends.h"

#include <cstddef>
#include <cstdio>

namespace {

int report(const char* algorithm, const Backend& backend, bool implemented) {
    const bool usable = implemented && backend.supported();
    const char* status = "disponibil";
    if (!implemented) {
        status = "sarit (neimplementat)";
    } else if (!usable) {
        status = "sarit (procesorul nu il suporta)";
    }

    std::printf("  %-7s %-7s %s\n", algorithm, backend.name, status);
    return usable ? 1 : 0;
}

} // namespace

int main() {
    int available = 0;

    for (std::size_t i = 0; i < BACKEND_COUNT; ++i) {
        const Backend& backend = BACKENDS[i];
        available += report("dot", backend, backend.dot != nullptr);
        available += report("matmul", backend, backend.matmul != nullptr);
        available += report("conv2d", backend, backend.conv2d != nullptr);
    }

    std::printf("Variante disponibile: %d din %d\n", available,
                static_cast<int>(BACKEND_COUNT) * 3);
    return 0;
}
