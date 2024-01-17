#pragma once
#include <variant>
#include <string>
#include <optional>
#include "box.hpp"

#define return_if_error(expr) if (expr.has_error()) return expr.error();

struct error {
	std::string error_class;
	std::string message;
	std::optional<Box<error>> cause;

	error() = delete;
	error& operator=(const error&) = delete;
	error& operator=(error&&) = delete;
	error(const error&) = default;
	error(error&&) = default;

	error(std::string error_class, std::string message) :
		error_class(std::move(error_class)), message(std::move(message)), cause(std::nullopt) {}

	template <typename E>
	error(std::string error_class, std::string message, E&& cause) :
		error_class(std::move(error_class)), message(std::move(message)), cause(std::forward<E>(cause)) {}

	bool has_underlying_error() const {
		return cause.has_value();
	}

	const error& underlying_error(std::string error_class, std::string message) const& {
		return cause.value().get();
	}

	error& underlying_error(std::string error_class, std::string message) & {
		return cause.value().get();
	}

	error wrap(std::string error_class, std::string message) && {
		return error{ error_class, message, std::move(*this) };
	}

	error wrap(std::string error_class, std::string message) const& {
		return error{ error_class, message, error{ *this } };
	}
};

template <typename Result>
struct expected {
	static_assert(!is_specialization<Result, expected>::value, "expected<expected<E>> should be replaced with expected<E>");
public:
	using value_type = Result;
	using error_type = error;
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
	expected(error&& err) : value_or_error(std::move(err)) {}
	expected(const error& err) : value_or_error(err) {}

	template <typename T>
		requires (std::is_constructible<Result, T>::value && !std::is_same_v<std::remove_cvref_t<T>, error>)
	expected(T&& t) : value_or_error(std::forward<T>(t)) {}

	bool has_value() const {
		return std::holds_alternative<value_type>(value_or_error);
	}

	bool has_error() const {
		return !has_value();
	}

	template <typename Self>
	decltype(auto) raw(this Self&& self) {
		return std::forward<Self>(self).value_or_error;
	}

	template <typename Self>
	decltype(auto) value(this Self&& self) {
		return std::get<value_type>(std::forward<Self>(self).value_or_error);
	}

	template <typename Self>
	decltype(auto) error(this Self&& self) {
		return std::get<error_type>(std::forward<Self>(self).value_or_error);
	}

	decltype(auto) transform(auto&& f) {
		using transform_return_type = decltype(f(std::declval<value_type>()));
		if constexpr (is_specialization<transform_return_type, expected>::value) {
			using return_type = transform_return_type;
			return has_value() ? return_type{ f(value()) } : return_type{ error() };
		}
		else {
			using return_type = expected<transform_return_type>;
			return has_value() ? return_type{ f(value()) } : return_type{ error() };
		}
	}
};
