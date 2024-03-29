#pragma once
#include <functional>
#include <optional>
#include <variant>

#include "tokenizer.hpp"
#include "../utility/fn_util.hpp"
#include "../utility/is_specialization.hpp"
#include "../utility/box.hpp"

template <int _token>
struct Token {
	static constexpr auto token = _token;
	static_assert(token != TAB, "Using Token<TAB> will not work, trailing tabs are ignored, use IndentNode");
	static_assert(token != SPACE, "Using Token<SPACE> will not work, trailing spaces are ignored");

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
};

struct IndentToken {
	int n_indent;
};

template <typename T>
struct Indent : public T {
	Indent(int n_indent) : T(n_indent + 1) {}
};

template <typename T>
struct Alloc {
	std::optional<Box<T>> value = std::nullopt;
	int n_indent;
	Alloc(int n_indent) : n_indent(n_indent) {}

	const T& get() const {
		return value.value().get();
	}
};

template <typename T>
struct Opt {
	std::optional<T> node;
	int n_indent;
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
	Or(int n_indent) : n_indent(n_indent) {};

	const variant_t& value() const {
		return _value.value();
	}
};

struct TemplateBody {
	int n_indent;
	std::string value;
	TemplateBody(int n_indent) : n_indent(n_indent) {};
};

template <typename T, typename CND, typename requiresComma>
struct KNode {
	std::vector<T> nodes;
	int n_indent;
	KNode(int n_indent) : n_indent(n_indent) {}

	// to get `b*` from `(abc)*` for example
	template <typename U>
	std::vector<U> get() const {
		return get_view<U>() | to_vec();
	}

	// to get `b*` from `(abc)*` for example
	template <typename U>
	auto get_view() const {
		if constexpr (std::is_same_v<U, T>)
			return nodes
			| LIFT_TRANSFORM_X(node, U{ node })
			;
		else if constexpr (is_specialization<T, And>::value)
			return nodes
			| LIFT_TRANSFORM_TRAIL(.get<U>())
			;
		else if constexpr (is_specialization<T, Alloc>::value)
			return nodes
			| LIFT_TRANSFORM_TRAIL(.get())
			;
		else if constexpr (is_specialization<T, Or>::value)
			return nodes
			| LIFT_TRANSFORM_TRAIL(.value())
			| filter_variant_type_eq<U>
			| tranform_variant_type_eq<U>
			;
		else
			static_assert(!sizeof(T*), "T is not supported");
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









inline void parse_whitespaces(Iterator& it) {
	while (it->first == TAB || it->first == SPACE)
		it++;
}

template <int token>
bool build_end_token(Token<token>& t, Iterator& it) {
	bool ok = it->first == token;
	if (ok)
		it++;
	return ok;
}

template <int token>
bool build_newline_token(Token<token>& t, Iterator& it) {
	auto save = it;
	parse_whitespaces(it);
	bool ok = it->first == token;
	if (ok)
		it++;
	else
		it = save;
	return ok;
}

template <int token>
bool build_normal_token(Token<token>& t, Iterator& it) {
	bool ok = it->first == token;
	if (ok) {
		t.value = it->second;
		it++;
		parse_whitespaces(it);
	}
	return ok;
}

template <int token>
bool build(Token<token>& t, Iterator& it) {
	if constexpr (token == END)
		return build_end_token(t, it);
	else if constexpr (token == NEWLINE)
		return build_newline_token(t, it);
	else
		return build_normal_token(t, it);
}

template <typename T>
bool build(Until<T>& until, Iterator& it) {
	T t{ until.n_indent };
	auto save = it;
	while (it->first != END) {
		if (build(t, it))
			return true;
		++it;
	}
	it = save;
	return false;
}

inline bool build(IndentToken& indent, Iterator& it) {
	bool correct = true;
	for (int i = 0; i < indent.n_indent; ++i) {
		correct &= it->first == TAB;
		if (it->first == END)
			return false;
		it++;
	}
	correct &= it->first != TAB && it->first != NEWLINE && it->first != SPACE;
	return correct;
}

template <typename T>
bool build(Alloc<T>& alloc, Iterator& it) {
	T t(alloc.n_indent);
	if (build(t, it)) {
		alloc.value = std::move(t);
		return true;
	}
	return false;
}

template <typename T>
bool build(Opt<T>& opt, Iterator& it) {
	T _node(opt.n_indent);
	bool parsed = build(_node, it);
	if (parsed)
		opt.node.emplace(std::move(_node));
	return true;
}

template <typename... Ands>
bool build(And<Ands...>& and_, Iterator& it) {
	bool failed = false;
	auto temp = it;
	for_each(and_.value, [&](auto& node) {
		if (failed)
			return;
		if (!build(node, it)) {
			it = temp;
			failed = true;
		}
	});
	return !failed;
}

template <typename... Ors>
bool build(Or<Ors...>& or_, Iterator& it) {
	bool populated = false;
	([&] {
		if (populated)
			return;
		Ors node = Ors(or_.n_indent);
		bool built = build(node, it);
		if (built) {
			or_._value.emplace(std::move(node));
			populated = true;
		}
	}(), ...);
	return populated;
}

inline bool parse_empty_line(Iterator& it) {
	auto newline = Token<NEWLINE>{ 0 };
	return build(newline, it);
}

inline bool parse_indented_line(TemplateBody& body, Iterator& it) {
	_ASSERT(body.n_indent == 0); // change code later to accomodate if needed but recursive templates or templates inside interfaces are not on the menu
	bool is_indented = it->first != END && it->first == TAB;
	if (!is_indented)
		return false;
	auto until_newline = Until<Token<NEWLINE>>{ body.n_indent };
	bool has_newline = build(until_newline, it);
	_ASSERT(has_newline); // if that doesnt work something is just wrong with the tokenizer.
	return true;
}

inline bool parse_one_line(TemplateBody& body, Iterator& it) {
	return parse_empty_line(it) || parse_indented_line(body, it);
}

inline bool build(TemplateBody& body, Iterator& it) {
	// look for first line that isnt just spaces/tabs/newline and doesnt begin with a tab, leave the iterator at the beginning of that line
	auto beg = it;
	while (parse_one_line(body, it));

	std::stringstream ss;
	while (beg != it) {
		ss << beg->second;
		++beg;
	}

	body.value = std::move(ss).str();
	return true;
}

template <typename T, typename CND, typename requiresComma>
bool build(KNode<T, CND, requiresComma>& knode, Iterator& it) {
	while (true) {
		auto node = T(knode.n_indent);
		bool parsed = build(node, it);
		if (parsed) {
			knode.nodes.push_back(std::move(node));
			if constexpr (requiresComma::value) {
				auto comma = Token<COMMA>(0);
				parsed = build(comma, it);
			}
		}
		if (!parsed)
			break;
	}
	return CND::cnd(knode.nodes);
}
