#pragma once
#include "grammarizer.h"
#include <functional>
#include <optional>
#include <variant>

template <typename...> class And;
template <typename...> class Or;
template <typename, typename, typename> class KNode;
template <typename> class Opt;
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


using End = Token<TOKENS::END>;
using Semicolon = Token<TOKENS::SEMICOLON>;
using Colon = Token<TOKENS::COLON>;
using Ns = Token<TOKENS::NS>;
using Backslash = Token<TOKENS::BACKSLASH>;
using Dot = Token<TOKENS::DOT>;
using Comma = Token<TOKENS::COMMA>;
using Equal = Token<TOKENS::EQUAL>;
using Lt = Token<TOKENS::LT>;
using Gt = Token<TOKENS::GT>;
using Dash = Token<TOKENS::DASH>;
using Space = Token<TOKENS::SPACE>;
using Tab = Token<TOKENS::TAB>;
using Newline = Token<TOKENS::NEWLINE>;
using Braceopen = Token<TOKENS::BRACEOPEN>;
using Braceclose = Token<TOKENS::BRACECLOSE>;
using Bracketopen = Token<TOKENS::BRACKETOPEN>;
using Bracketclose = Token<TOKENS::BRACKETCLOSE>;
using Parenopen = Token<TOKENS::PARENOPEN>;
using Parenclose = Token<TOKENS::PARENCLOSE>;
using Asterisk = Token<TOKENS::ASTERISK>;
using Slash = Token<TOKENS::SLASH>;
using Percent = Token<TOKENS::PERCENT>;
using Ampersand = Token<TOKENS::AMPERSAND>;
using Question = Token<TOKENS::QUESTION>;
using Pound = Token<TOKENS::POUND>;
using Not = Token<TOKENS::NOT>;
using Caret = Token<TOKENS::CARET>;
using Bitor = Token<TOKENS::BITOR>;
using Bitand = Token<TOKENS::BITAND>;
using PlusKW = Token<TOKENS::PLUS>;
using Tilde = Token<TOKENS::TILDE>;

using Equalequal = Token<TOKENS::EQUALEQUAL>;
using Nequal = Token<TOKENS::NEQUAL>;
using Plusequal = Token<TOKENS::PLUSEQUAL>;
using Minusequal = Token<TOKENS::MINUSEQUAL>;
using Timesequal = Token<TOKENS::TIMESEQUAL>;
using Divequal = Token<TOKENS::DIVEQUAL>;
using Modequal = Token<TOKENS::MODEQUAL>;
using Andequal = Token<TOKENS::ANDEQUAL>;
using Orequal = Token<TOKENS::OREQUAL>;
using Xorequal = Token<TOKENS::XOREQUAL>;

using Gte = Token<TOKENS::GTE>;
using Lte = Token<TOKENS::LTE>;
using Andand = Token<TOKENS::ANDAND>;
using Oror = Token<TOKENS::OROR>;

using Plusplus = Token<TOKENS::PLUSPLUS>;
using Minusminus = Token<TOKENS::MINUSMINUS>;

using Arrow = Token<TOKENS::ARROW>;

using Word = Token<TOKENS::WORD>;
using Number = Token<TOKENS::NUMBER>;
using String = Token<TOKENS::STRING>;

using ClassKW = Token<TOKENS::CLASS>;
using Return = Token<TOKENS::RETURN>;
using New = Token<TOKENS::NEW>;
using Switch = Token<TOKENS::SWITCH>;
using In = Token<TOKENS::IN>;
using Ifor = Token<TOKENS::IFOR>;
using ImportKW = Token<TOKENS::IMPORT>;
using From = Token<TOKENS::FROM>;
using For = Token<TOKENS::FOR>;
using While = Token<TOKENS::WHILE>;
using If = Token<TOKENS::IF>;
using Else = Token<TOKENS::ELSE>;
using Break = Token<TOKENS::BREAK>;
using Case = Token<TOKENS::CASE>;
using Do = Token<TOKENS::DO>;
using UsingKW = Token<TOKENS::USING>;
using Static = Token<TOKENS::STATIC>;
using Extends = Token<TOKENS::EXTENDS>;

using Public = Token<TOKENS::PUBLIC>;
using Private = Token<TOKENS::PRIVATE>;
using Protected = Token<TOKENS::PROTECTED>;

using AndKW = Token<TOKENS::AND>;
using OrKW = Token<TOKENS::OR>;
using Null_token = Token<TOKENS::NULL_TOKEN>;


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
class Opt {
public:
	std::optional<T> node;
	int n_indent;
	Opt(int n_indent) : n_indent(n_indent) {}

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
