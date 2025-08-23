#pragma once
#include <variant>
#include <string>
#include <optional>
#include "box.hpp"

#define return_if_error(expr) if (expr.has_error()) return expr.error();

namespace select_ {
	template <bool, typename, typename>
	struct select;

	template <typename T, typename U>
	struct select<true, T, U> {
		using type = T;
	};

	template <typename T, typename U>
	struct select<false, T, U> {
		using type = U;
	};
}

template <bool b, typename T, typename U>
using select_t = typename select_::select<b, T, U>::type;

struct error {
	std::string error_class;
	std::string message;
	std::optional<Box<error>> cause;

	error() = delete;
	error& operator=(const error&) = delete;
	error& operator=(error&&) = delete;
	error(const error&) = default;
	error(error&&) = default;

	error(std::string error_class_, std::string message_) :
		error_class(std::move(error_class_)), message(std::move(message_)), cause(std::nullopt) {
	}

	error(std::string error_class_, std::string message_, error cause) :
		error_class(std::move(error_class_)), message(std::move(message_)), cause(std::move(cause)) {
	}

	bool has_underlying_error() const {
		return cause.has_value();
	}

	const error& underlying_error(std::string error_class_, std::string message_) const& {
		return cause.value().get();
	}

	error& underlying_error(std::string error_class_, std::string message_)& {
		return cause.value().get();
	}

	error wrap(std::string error_class_, std::string message_)&& {
		return error{ std::move(error_class_), std::move(message_), std::move(*this) };
	}

	error wrap(std::string error_class_, std::string message_) const& {
		return error{ std::move(error_class_), std::move(message_), *this };
	}
};

template <typename Result, typename error_t = error>
struct expected {
	static_assert(!is_specialization<Result, expected>::value, "expected<expected<E>> should be replaced with expected<E>");
public:
	using value_type = Result;
	using error_type = error_t;
protected:
	std::variant<value_type, error_type> value_or_error;
public:
	expected() = delete;
	expected& operator=(const expected&) = default;
	expected(const expected&) = default;
	expected& operator=(expected&&) = default;
	expected(expected&&) = default;

	expected(value_type&& value) : value_or_error(std::move(value)) {}
	expected(const value_type& value) : value_or_error(value) {}
	expected(error_type&& err) : value_or_error(std::move(err)) {}
	expected(const error_type& err) : value_or_error(err) {}

	template <typename T>
		requires (std::is_constructible<Result, T>::value && !std::is_same_v<std::remove_cvref_t<T>, error_type>)
	expected(T&& t) : value_or_error(std::forward<T>(t)) {}

	bool has_value() const {
		return std::holds_alternative<value_type>(value_or_error);
	}

	bool has_error() const {
		return !has_value();
	}

	template <typename Self>
	decltype(auto) value(this Self&& self) {
		return std::get<value_type>(std::forward<Self>(self).value_or_error);
	}

	template <typename Self>
	decltype(auto) error(this Self&& self) {
		return std::get<error_type>(std::forward<Self>(self).value_or_error);
	}

	template <typename Self>
	decltype(auto) transform(this Self&& self, auto&& f) {
		using transform_return_type = decltype(f(std::declval<value_type>()));
		using return_type = select_t<
			is_specialization<transform_return_type, expected>::value,
			transform_return_type,
			expected<transform_return_type>
		>;
		return std::forward<Self>(self).has_value() ? return_type{ f(std::forward<Self>(self).value()) } : return_type{ std::forward<Self>(self).error() };
	}
};
