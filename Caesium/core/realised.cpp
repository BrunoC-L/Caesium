#include "realised.hpp"
#include "../structured/helpers.hpp"

#define CMP_N(N, T) inline std::strong_ordering operator<=>(const Realised::T& left, const Realised::T& right) { return cmp##N (left, right); }
ExpandAll(CMP_N);
#undef CMP_N
