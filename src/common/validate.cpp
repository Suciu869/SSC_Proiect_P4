#include "common/validate.h"

#include <algorithm>
#include <cmath>
#include <cstring>

bool nearly_equal(float a, float b, float rel_tol) {
    if (a == b) {
        return true;
    }
    if (!std::isfinite(a) || !std::isfinite(b)) {
        return false;
    }
    return std::fabs(a - b) <= rel_tol * std::max(std::fabs(a), std::fabs(b));
}

bool nearly_equal(const float* a, const float* b, std::size_t n, float rel_tol) {
    for (std::size_t i = 0; i < n; ++i) {
        if (!nearly_equal(a[i], b[i], rel_tol)) {
            return false;
        }
    }
    return true;
}

bool exactly_equal(const std::uint8_t* a, const std::uint8_t* b, std::size_t n) {
    return std::memcmp(a, b, n) == 0;
}
