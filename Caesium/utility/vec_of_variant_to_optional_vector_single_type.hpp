#pragma once
#include "expected.hpp"
#include "fn_util.hpp"
#include <vector>

template <typename T, typename... Ts>
std::optional<std::vector<T>> vec_of_variant_to_optional_vector_single_type(const std::vector<std::variant<Ts...>> vec) {
	for (const auto& e : vec)
		if (!std::holds_alternative<T>(e))
			return std::nullopt;
	return vec | LIFT_TRANSFORM_X(e, std::get<T>(e)) | to_vec();
}
