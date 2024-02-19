#pragma once
#include <functional>
#include <optional>
#include <variant>
#include <ranges>

#include "tokenizer.hpp"
#include "fn_util.hpp"
#include "is_specialization.hpp"
#include "box.hpp"

template <typename...> struct And;
template <typename...> struct Or;
template <typename, typename, typename> struct KNode;
template <typename> struct Opt;
template <typename> struct Alloc;
template <typename> struct Indent;
struct IndentToken;
template <int> struct Token;

template <typename T> struct is_primitive_node_type;

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
	is_specialization<T, Alloc>,
	is_specialization_int<T, Token>,
	std::is_same<T, IndentToken>,
	is_primitive_indent<T>> {};

template <int _token>
struct Token {
	static constexpr auto token = _token;
	static_assert(token != TAB, "Using Token<TAB> will not work, trailing tabs are ignored, use IndentNode");
	static_assert(token != SPACE, "Using Token<SPACE> will not work, trailing spaces are ignored");

	std::string value;
	int n_indent;
	Token(int n_indent) : n_indent(n_indent) {}

	void parse_whitespaces(Iterator& it) {
		while (it->first == TAB || it->first == SPACE)
			it++;
	}

	bool build_end_token(Iterator& it) {
		bool ok = it->first == token;
		if (ok)
			it++;
		return ok;
	}
	
	bool build_newline_token(Iterator& it) {
		auto save = it;
		parse_whitespaces(it);
		bool ok = it->first == token;
		if (ok)
			it++;
		else
			it = save;
		return ok;
	}

	bool build_normal_token(Iterator& it) {
		bool ok = it->first == token;
		if (ok) {
			value = it->second;
			it++;
			parse_whitespaces(it);
		}
		return ok;
	}

	bool build(Iterator& it) {
		if constexpr (token == END)
			return build_end_token(it);
		else if constexpr (token == NEWLINE)
			return build_newline_token(it);
		else
			return build_normal_token(it);
	}

	std::weak_ordering operator<=>(const Token& other) const {
		// might want to trim tokens for comparison but i cant figure if thats a problem right now, lets throw in case and fix later if it is
		if (std::find(value.begin(), value.end(), ' ') != value.end())
			throw;
		return value <=> other.value;
	}
};

template <typename T>
struct Until {
	int n_indent;
	Until(int n_indent) : n_indent(n_indent) {}
	bool build(Iterator& it) {
		T t{ n_indent };
		auto save = it;
		while (it->first != END) {
			if (t.build(it))
				return true;
			++it;
		}
		it = save;
		return false;
	}
};

struct IndentToken {
	int n_indent;
	IndentToken(int n_indent) : n_indent(n_indent) {}
	bool build(Iterator& it) {
		bool correct = true;
		for (int i = 0; i < n_indent; ++i) {
			correct &= it->first == TAB;
			if (it->first == END)
				return false;
			it++;
		}
		correct &= it->first != TAB && it->first != NEWLINE && it->first != SPACE;
		return correct;
	}
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

	bool build(Iterator& it) {
		T t(n_indent);
		if (t.build(it)) {
			value = std::move(t);
			return true;
		}
		return false;
	}
};

template <typename T, typename CND, typename requiresComma>
struct KNode {
	std::vector<T> nodes;
	int n_indent;
	KNode(int n_indent) : n_indent(n_indent) {}

	bool build(Iterator& it) {
		while (true) {
			auto node = T(n_indent);
			bool parsed = node.build(it);
			if (parsed) {
				nodes.push_back(std::move(node));
				if constexpr (requiresComma::value)
					parsed = Token<COMMA>(0).build(it);
			}
			if (!parsed)
				break;
		}
		return CND::cnd(nodes);
	}

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

	bool build(Iterator& it) {
		T _node(n_indent);
		bool parsed = _node.build(it);
		if (parsed)
			node.emplace(std::move(_node));
		return true;
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

	bool build(Iterator& it) {
		bool failed = false;
		auto temp = it;
		for_each(value, [&](auto& node) {
			if (failed)
				return;
			if (!node.build(it)) {
				it = temp;
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
	std::optional<variant_t> _value;
	Or(int n_indent) : n_indent(n_indent) {};

	const variant_t& value() const {
		return _value.value();
	}

	bool build(Iterator& it) {
		bool populated = false;
		([&] {
			if (populated)
				return;
			Ors node = Ors(n_indent);
			bool built = node.build(it);
			if (built) {
				_value.emplace(std::move(node));
				populated = true;
			}
		}(), ...);
		return populated;
	}
};

struct TemplateBody {
	int n_indent;
	std::string value;
	TemplateBody(int n_indent) : n_indent(n_indent) {};
	bool build(Iterator& it) {
		// look for first line that isnt just spaces/tabs/newline and doesnt begin with a tab, leave the iterator at the beginning of that line
		auto beg = it;
		while (parse_one_line(it));

		std::stringstream ss;
		while (beg != it) {
			ss << beg->second;
			++beg;
		}

		value = std::move(ss).str();
		return true;
	}

	bool parse_one_line(Iterator& it) {
		return parse_empty_line(it) || parse_indented_line(it);
	}

	bool parse_empty_line(Iterator& it) {
		return Token<NEWLINE>{0}.build(it);
	}

	bool parse_indented_line(Iterator& it) {
		_ASSERT(n_indent == 0); // change code later to accomodate if needed but recursive templates or templates inside classes are not on the menu
		bool is_indented = it->first != END && it->first == TAB;
		if (!is_indented)
			return false;
		bool has_newline = Until<Token<NEWLINE>>{ n_indent }.build(it);
		_ASSERT(has_newline); // if that doesnt work something is just wrong with the tokenizer.
		return true;
	}
};
