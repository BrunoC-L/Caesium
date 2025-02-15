#pragma once
#include <functional>
#include <optional>
#include <variant>
#include <utility>
#include <compare>

#include "tokenizer.hpp"
#include "../utility/fn_util.hpp"
#include "../utility/is_specialization.hpp"
#include "../utility/box.hpp"

struct parse_error {
	std::string name_of_rule;
	unsigned beg_offset;
};

namespace grammar {
	template <typename T>
	constexpr std::string name_of_rule();
}

template <token_t _token>
struct Token {
	static constexpr auto token = _token;
	static_assert(token != TAB, "Using Token<TAB> will not work, trailing tabs are ignored, use IndentNode");
	static_assert(token != SPACE, "Using Token<SPACE> will not work, trailing spaces are ignored");
	unsigned beg_offset;
	unsigned end_offset;

	std::string value;
	int n_indent;
	Token(int n_indent) : n_indent(n_indent) {}

	std::weak_ordering operator<=>(const Token& other) const {
		// might want to trim tokens for comparison but i cant figure if thats a problem right now, lets throw in case and fix later if it is
		if (value.size() > 0 && value[0] == ' ' || value[value.size() - 1] == ' ')
			throw;
		return value <=> other.value;
	}
};

template <typename T>
struct Until {
	int n_indent;
	unsigned beg_offset;
	unsigned end_offset;
};

struct IndentToken {
	int n_indent;
	unsigned beg_offset;
	unsigned end_offset;
};

template <typename T>
struct Indent : T {
	Indent(int n_indent) : T(n_indent + 1) {}
	unsigned beg_offset;
	unsigned end_offset;
};

template <typename T>
struct Commit : T {
	Commit(int n_indent) : T(n_indent) {}
	unsigned beg_offset;
	unsigned end_offset;
};

template <typename T>
struct Expect : T {
	Expect(int n_indent) : T(n_indent) {}
	unsigned beg_offset;
	unsigned end_offset;
};

template <typename T>
struct Alloc {
	std::optional<Box<T>> value = std::nullopt;
	int n_indent;
	unsigned beg_offset;
	unsigned end_offset;
	Alloc(int n_indent) : n_indent(n_indent) {}

	const T& get() const {
		return value.value().get();
	}
};

template <typename T>
struct Opt {
	std::optional<T> node;
	int n_indent;
	unsigned beg_offset;
	unsigned end_offset;
	Opt(int n_indent) : n_indent(n_indent) {}

	bool has_value() const {
		return node.has_value();
	}

	const T& value() const {
		return node.value();
	}
};

// https://blog.tartanllama.xyz/exploding-tuples-fold-expressions/
template <std::size_t... Idx> auto make_index_dispatcher(std::index_sequence<Idx...>) { return [](auto&& f) { (f(std::integral_constant<std::size_t, Idx>{}), ...); }; }
template <std::size_t N> auto make_index_dispatcher() { return make_index_dispatcher(std::make_index_sequence<N>{}); }
template <typename Tuple, typename Func> void for_each(Tuple&& t, Func&& f) { make_index_dispatcher<std::tuple_size<std::decay_t<Tuple>>::value>()([&f, &t](auto idx) { f(std::get<idx>(std::forward<Tuple>(t))); }); }

template <typename TUPLE, typename T, int i, int cursor, typename U, typename... Rest>
const T& get_tuple_smart_cursor(const TUPLE& tuple) {
	if constexpr (std::is_same_v<T, std::remove_reference_t<std::remove_const_t<U>>>) {
		if constexpr (i == 0)
			return std::get<cursor>(tuple);
		else
			return get_tuple_smart_cursor<TUPLE, T, i - 1, cursor + 1, Rest...>(tuple);
	}
	else
		return get_tuple_smart_cursor<TUPLE, T, i, cursor + 1, Rest...>(tuple);
}

template <typename... Ands>
struct And {
	int n_indent;
	using tuple_t = std::tuple<Ands...>;

	tuple_t value;
	unsigned beg_offset;
	unsigned end_offset;

	And(int n_indent) : n_indent(n_indent), value({ Ands(n_indent)... }) {}

	template <typename T>
	const T& get() const {
		return std::get<T>(value);
	}

	template <typename T, int i>
	const T& get() const {
		return get_tuple_smart_cursor<tuple_t, T, i, 0, Ands...>(value);
	}
};

template <typename... Ors>
struct Or {
	int n_indent;
	using variant_t = std::variant<Ors...>;

	std::optional<variant_t> _value;
	unsigned beg_offset;
	unsigned end_offset;

	Or(int n_indent) : n_indent(n_indent) {};

	const variant_t& value() const {
		return _value.value();
	}
};

struct TemplateBody {
	int n_indent;
	std::string value;
	unsigned beg_offset;
	unsigned end_offset;
	TemplateBody(int n_indent) : n_indent(n_indent) {};
};

template <typename T, typename CND, typename requiresComma>
struct KNode {
	std::vector<T> nodes;
	int n_indent;
	unsigned beg_offset;
	unsigned end_offset;
	KNode(int n_indent) : n_indent(n_indent) {}

	// to get `b*` from `(abc)*` for example
	template <typename U>
	decltype(auto) get() const {
		if constexpr (std::is_same_v<U, T>)
			return (const std::vector<T>&)nodes;
		else
			return get_view<U>() | to_vec();
	}
	
	// to get `b*` from `(abc)*` for example
	template <typename U>
	decltype(auto) get_view() const {
		if constexpr (std::is_same_v<U, T>)
			return (const std::vector<T>&)nodes;
		else if constexpr (is_specialization<T, And>::value)
			return nodes
			| std::views::transform([&](auto&& e) { return e.template get<U>(); })
			;
		else if constexpr (std::is_same_v<Alloc<U>, T>)
			return nodes
			| std::views::transform([&](auto&& e) { return e.get(); })
			;
		else if constexpr (is_specialization<T, Or>::value)
			return nodes
			| std::views::transform([&](auto&& e) { return e.value(); })
			| std::views::filter([&](auto&& e) { return std::holds_alternative<U>(e); })
			| std::views::transform([&](auto&& e) { return std::get<U>(e); })
			;
		else
			static_assert(!sizeof(U*), "U is not supported");
	}
};

struct StarCnd {
	template <typename T>
	static bool cnd(const std::vector<T>&) {
		return true;
	}
};

struct PlusCnd {
	template <typename T>
	static bool cnd(const std::vector<T>& nodes) {
		return nodes.size() > 0;
	}
};

template <typename T>
using Star = KNode<T, StarCnd, std::false_type>;

template <typename T>
using CommaStar = KNode<T, StarCnd, std::true_type>;

template <typename T>
using Plus = KNode<T, PlusCnd, std::false_type>;

template <typename T>
using CommaPlus = KNode<T, PlusCnd, std::true_type>;
