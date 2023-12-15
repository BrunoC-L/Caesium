#include "fn_util.hpp"

decltype(std::ranges::to<std::vector>()) to_vec() {
    return std::ranges::to<std::vector>();
}
