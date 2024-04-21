#pragma once
#include <ranges>
#include <vector>
#include <type_traits>
#include <variant>

#define FORWARD(expr) std::forward<decltype(expr)>(expr)

static auto to_vec() {
	return std::ranges::to<std::vector>();
}

#define LIFT(...) [&](auto&&... e) { return __VA_ARGS__(std::forward<decltype(e)>(e)...); }
#define LIFT_FILTER(...) std::views::filter(LIFT(__VA_ARGS__))
#define LIFT_TRANSFORM(...) std::views::transform(LIFT(__VA_ARGS__))

template <typename T>
auto filter_variant_type_eq = LIFT_FILTER(std::holds_alternative<T>);

template <typename T>
auto tranform_variant_type_eq = LIFT_TRANSFORM(std::get<T>);

#define filter_transform_variant_type_eq(...) filter_variant_type_eq<__VA_ARGS__> | tranform_variant_type_eq<__VA_ARGS__>

#define LIFT_FILTER_TRAIL(...) std::views::filter([&](const auto& e) { return e __VA_ARGS__ ; })
#define LIFT_TRANSFORM_TRAIL(...) std::views::transform([&](const auto& e) { return e __VA_ARGS__ ; })

#define LIFT_FILTER_X(operand_name_here, ...) std::views::filter([&](auto&& operand_name_here) { return __VA_ARGS__ ; })
#define LIFT_TRANSFORM_X(operand_name_here, ...) std::views::transform([&](auto&& operand_name_here) { return __VA_ARGS__ ; })
