#pragma once

#include <cstddef>
#include <cstdint>

const float DEFAULT_REL_TOLERANCE = 1e-5f;

// float: toleranta relativa, pentru ca varianta vectorizata aduna in alta ordine
bool nearly_equal(float a, float b, float rel_tol = DEFAULT_REL_TOLERANCE);
bool nearly_equal(const float* a, const float* b, std::size_t n,
                  float rel_tol = DEFAULT_REL_TOLERANCE);

// uint8: egalitate exacta
bool exactly_equal(const std::uint8_t* a, const std::uint8_t* b, std::size_t n);
