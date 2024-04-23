#pragma once
#include <ranges>
#include <vector>

static auto to_vec() {
	return std::ranges::to<std::vector>();
}
