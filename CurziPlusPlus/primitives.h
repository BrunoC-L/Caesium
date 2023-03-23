#pragma once
#include "grammarizer.h"
#include <functional>
#include <optional>
#include <variant>

template <typename...> struct And;
template <typename...> struct Or;
template <typename, typename, typename> struct KNode;
template <typename> struct Opt;
template <typename> struct Indent;
struct IndentToken;
template <int> struct Token;

template <typename T> struct is_primitive_node_type;

template <typename T, template <typename...> typename Template>
struct is_specialization : std::false_type {};

template <template <typename...> typename Template, typename... Args>
struct is_specialization<Template<Args...>, Template> : std::true_type {};

template <typename T, template <int...> typename Template>
struct is_specialization_int : std::false_type {};

template <template <int> typename Template, int Arg>
struct is_specialization_int<Template<Arg>, Template> : std::true_type {};

template <typename T>
struct is_specialization_indent : std::false_type {};

template <typename T>
struct is_specialization_indent<Indent<T>> : std::true_type {};

template <typename T>
struct is_primitive_indent : std::false_type {};

template <typename T>
struct is_primitive_indent<Indent<T>> : is_primitive_node_type<T> {};

template <typename T>
struct is_primitive_node_type : std::disjunction<
	is_specialization<T, And>,
	is_specialization<T, Or>,
	is_specialization<T, Opt>,
	is_specialization<T, KNode>,
	is_specialization_int<T, Token>,
	std::is_same<T, IndentToken>,
	is_primitive_indent<T>> {};

template <typename T>
bool build_optional_primitive(T& node, Grammarizer* g) {
	if constexpr (is_primitive_node_type<std::remove_reference_t<T>>::value)
		return node.build(g);
	else
		return build_optional_primitive(node.value, g);
}

template <int token>
struct Token {
	std::string value;
	int n_indent;
	static_assert(token != TAB, "Using Token<TAB> will not work, trailing tabs are ignored, use IndentNode");
	static_assert(token != SPACE, "Using Token<SPACE> will not work, trailing spaces are ignored");
	Token(int n_indent) : n_indent(n_indent) {}
	bool build(Grammarizer* g) {
		bool isT = g->it->first == token;
		if (isT)
			value = g->it->second;
		if constexpr (token == END) {
			while (g->it != g->tokens.end()) {
				if (g->it->first == TAB || g->it->first == SPACE || g->it->first == NEWLINE)
					g->it++;
				else if (g->it->first == END)
					return true;
				else break;
			}
			return false;
		}
		else if (!isT)
			return false;

		g->it++;
		if constexpr (token == NEWLINE) {
			auto savepoint = g->it;
			while (g->it != g->tokens.end() && (g->it->first == TAB || g->it->first == SPACE || g->it->first == NEWLINE)) {
				if (g->it->first == NEWLINE) {
					g->it++;
					savepoint = g->it;
				}
				else {
					g->it++;
				}
			}
			g->it = savepoint;
			return true;
		}
		while (g->it != g->tokens.end() && (g->it->first == TAB || g->it->first == SPACE)) // ignoring trailing tabs & spaces
			g->it++;
		return true;
	}
};

struct UntilToken {
	TOKENS t;
	std::string value;
	UntilToken(TOKENS t) : t(t) {}

	UntilToken(UntilToken&&) = default;
	UntilToken(const UntilToken&) = default;

	bool build(Grammarizer* g) {
		while (g->it != g->tokens.end()) {
			value += g->it->second;
			bool isT = g->it->first == t;
			g->it++;
			if (isT)
				return true;
		}
		return false;
	}
};

struct IndentToken {
	int n_indent;
	IndentToken(int n_indent) : n_indent(n_indent) {}
	bool build(Grammarizer* g) {
		bool correct = true;
		for (int i = 0; i < n_indent; ++i) {
			correct &= g->it->first == TAB;
			g->it++;
			if (g->it == g->tokens.end())
				return false;
		}
		correct &= g->it->first != TAB && g->it->first != NEWLINE && g->it->first != SPACE;
		return correct;
	}
};

template <typename T>
struct Indent : public T {
	Indent(int n_indent): T(n_indent + 1) {}
};

template <typename T, typename CND, typename requiresComma>
struct KNode {
	std::vector<T> nodes;
	int n_indent;
	KNode(int n_indent) : n_indent(n_indent) {}

	KNode(KNode&&) = default;
	KNode(const KNode&) = default;

	bool build(Grammarizer* g) {
		while (true) {
			auto node = T(n_indent);
			bool parsed = build_optional_primitive(node, g);
			if (parsed) {
				nodes.emplace_back(std::move(node));
				if constexpr (requiresComma::value) {
					auto comma = Token<COMMA>(0);
					parsed = build_optional_primitive(comma, g);
				}
			}
			if (!parsed)
				break;
		}
		return CND::cnd(nodes);
	}

	// to get `b*` from `(abc)*` for example
	template <typename U>
	std::vector<U> get() const {
		if constexpr (std::is_same_v<U, T>) {
			std::vector<U> res;
			for (const auto& node : nodes)
				res.push_back(U{ node });
			return res;
		}
		else {
			std::vector<U> res;
			for (const auto& node : nodes) {
				if constexpr (is_specialization<T, And>::value) {
					int x = 0;
					res.push_back(node.get<U>());
				}
				else if constexpr (is_specialization<T, Or>::value) {
					if (std::holds_alternative<U>(node.value.value()))
						res.push_back(std::get<U>(node.value.value()));
				}
				else
					static_assert(!sizeof(T*), "T is not supported");
			}
			return res;
		}
	}
};

struct StarCnd {
	template <typename T>
	static bool cnd(const std::vector<T>& nodes) {
		return true;
	}
};

struct PlusCnd {
	template <typename T>
	static bool cnd(const std::vector<T>& nodes) {
		return nodes.size();
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

template <typename T>
struct Opt {
	std::optional<T> node;
	int n_indent;
	Opt(int n_indent) : n_indent(n_indent) {}

	Opt(Opt&&) = default;
	Opt(const Opt&) = default;

	bool build(Grammarizer* g) {
		T node(n_indent);
		bool parsed = build_optional_primitive(node, g);
		if (parsed)
			this->node.emplace(std::move(node));
		return true;
	}
};

// https://blog.tartanllama.xyz/exploding-tuples-fold-expressions/
template <std::size_t... Idx> auto make_index_dispatcher(std::index_sequence<Idx...>) { return [](auto&& f) { (f(std::integral_constant<std::size_t, Idx>{}), ...); }; }
template <std::size_t N> auto make_index_dispatcher() { return make_index_dispatcher(std::make_index_sequence<N>{}); }
template <typename Tuple, typename Func> void for_each(Tuple&& t, Func&& f) { make_index_dispatcher<std::tuple_size<std::decay_t<Tuple>>::value>()([&f, &t](auto idx) { f(std::get<idx>(std::forward<Tuple>(t))); }); }

template <typename TUPLE, typename T, int i, int cursor, typename U>
const T& get_tuple_smart_cursor(const TUPLE& tuple) {
	return std::get<cursor>(tuple);
}

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

	// using unique_ptr because we can't use std::optional, it requires the size
	// and some rules are recursive so we would end up generating
	// types similar to: SomeStruct { SomeStruct value; };
	std::unique_ptr<tuple_t> value = nullptr;

	And(int n_indent) : n_indent(n_indent) {}

	And(And&&) = default;
	And(const And& other) : n_indent(other.n_indent), value(std::make_unique<tuple_t>(*other.value.get())) {};

	template <typename T>
	const T& get() const {
		return std::get<T>(*value.get());
	}

	template <int i>
	decltype(std::get<i>(*value.get())) get() const {
		return std::get<i>(*value.get());
	}

	template <typename T, int i>
	const T& get() const {
		return get_tuple_smart_cursor<tuple_t, T, i, 0, Ands...>(*value.get());
	}

	bool build(Grammarizer* g) {
		bool failed = false;
		auto temp = g->it;
		value = std::make_unique<tuple_t>(tuple_t{ Ands(n_indent)... });
		for_each(*value.get(), [&](auto& node) {
			if (failed)
				return;
			if (!build_optional_primitive(node, g)) {
				g->it = temp;
				failed = true;
			}
		});
		return !failed;
	}
};

template <typename... Ors>
struct Or {
	int n_indent;
	using variant_t = std::variant<Ors...>;
	std::optional<variant_t> value;
	Or(int n_indent) : n_indent(n_indent) {};

	Or(Or&&) = default;
	Or(const Or&) = default;

	bool build(Grammarizer* g) {
		bool populated = false;
		([&] {
			if (populated)
				return;
			Ors node = Ors(n_indent);
			bool built = build_optional_primitive(node, g);
			if (built) {
				value.emplace(std::move(node));
				populated = true;
			}
		}(), ...);
		return populated;
	}
};
