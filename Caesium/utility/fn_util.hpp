#pragma once
#include <ranges>
#include <vector>

static inline auto to_vec() {
	return std::ranges::to<std::vector>();
}
