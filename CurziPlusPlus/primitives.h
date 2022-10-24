#pragma once
#include "grammarizer.h"
#include <functional>
#include <optional>
#include <variant>

template <typename...> class And;
template <typename...> class Or;
template <typename, typename, typename> class KNode;
template <typename> class OPT;
template <typename> struct Indent;
struct IndentToken;
template <int> struct Token;

template <class T> struct is_primitive_node_type;

template <class T, template <class...> class Template>
struct is_specialization : std::false_type {};

template <template <class...> class Template, class... Args>
struct is_specialization<Template<Args...>, Template> : std::true_type {};

template <class T, template <int...> class Template>
struct is_specialization_int : std::false_type {};

template <template <int> class Template, int Arg>
struct is_specialization_int<Template<Arg>, Template> : std::true_type {};

template <class T>
struct is_specialization_indent : std::false_type {};

template <class T>
struct is_specialization_indent<Indent<T>> : std::true_type {};

template <class T>
struct is_primitive_indent : std::false_type {};

template <class T>
struct is_primitive_indent<Indent<T>> : is_primitive_node_type<T> {};

template <class T>
struct is_primitive_node_type : std::disjunction<
	is_specialization<T, And>,
	is_specialization<T, Or>,
	is_specialization<T, OPT>,
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
class KNode {
	std::vector<T> nodes;
	int n_indent;
public:
	KNode(int n_indent) : n_indent(n_indent) {}

	bool build(Grammarizer* g) {
		while (true) {
			auto node = T(n_indent);
			bool parsed = build_optional_primitive(node, g);
			if (parsed) {
				nodes.push_back(node);
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
	template <typename T>
	std::vector<T> get() const {
		std::vector<T> res;
		for (const auto& node : nodes)
			res.push_back(node.get<T>());
		return res;
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
class OPT {
public:
	std::optional<T> node;
	int n_indent;
	OPT(int n_indent) : n_indent(n_indent) {}

	bool build(Grammarizer* g) {
		T node(n_indent);
		bool parsed = build_optional_primitive(node, g);
		if (parsed)
			this->node.emplace(node);
		return true;
	}
};

// https://blog.tartanllama.xyz/exploding-tuples-fold-expressions/
template <std::size_t... Idx> auto make_index_dispatcher(std::index_sequence<Idx...>) { return [](auto&& f) { (f(std::integral_constant<std::size_t, Idx>{}), ...); }; }
template <std::size_t N> auto make_index_dispatcher() { return make_index_dispatcher(std::make_index_sequence<N>{}); }
template <typename Tuple, typename Func> void for_each(Tuple&& t, Func&& f) { make_index_dispatcher<std::tuple_size<std::decay_t<Tuple>>::value>()([&f, &t](auto idx) { f(std::get<idx>(std::forward<Tuple>(t))); }); }


template <typename... Ands>
class And {
public:
	int n_indent;
	using tuple_t = std::tuple<Ands...>;
	std::unique_ptr<tuple_t> value = nullptr;
	And(int n_indent) : n_indent(n_indent) {}
	And(const And<Ands...>& other) : n_indent(other.n_indent), value(other.value.get() ? std::make_unique<tuple_t>(*other.value.get()) : nullptr) {};

	template <typename T>
	const T& get() const {
		return std::get<T>(*value.get());
	}

	template <int i>
	decltype(std::get<i>(*value.get())) get() {
		return std::get<i>(*value.get());
	}

	bool build(Grammarizer* g) {
		bool failed = false;
		auto temp = g->it;
		value = std::move(std::make_unique<tuple_t>(tuple_t{ Ands(n_indent)... }));
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
class Or {
public:
	int n_indent;
	using variant_t = std::variant<std::monostate, Ors...>;
	std::unique_ptr<variant_t> value;
	Or(int n_indent) : n_indent(n_indent) {}
	Or(const Or<Ors...>& other) : n_indent(other.n_indent), value(other.value.get() ? std::make_unique<variant_t>(*other.value.get()) : nullptr) {};

	bool build(Grammarizer* g) {
		bool populated = false;
		([&] {
			if (populated)
				return;
			Ors node = Ors(n_indent);
			bool built = build_optional_primitive(node, g);
			if (built) {
				value = std::make_unique<variant_t>(std::move(node));
				populated = true;
			}
			}(), ...);
		return populated;
	}
};