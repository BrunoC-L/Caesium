#pragma once
#include "grammar.hpp"

inline void parse_whitespaces(auto& it) {
	while (it->first == TAB || it->first == SPACE)
		it++;
}

template <int token>
bool build_end_token(Token<token>& t, auto& it) {
	bool ok = it->first == token;
	if (ok)
		it++;
	return ok;
}

inline bool build_newline_token(auto& it) {
	auto beg = it;
	auto checkpoint = it;
	while (true) {
		parse_whitespaces(it);
		bool ok = it->first == NEWLINE;
		if (ok) {
			it++;
			checkpoint = it;
		}
		else {
			it = checkpoint;
			break;
		}
	}
	return it != beg;
}

template <int token>
bool build_normal_token(Token<token>& t, auto& it) {
	bool ok = it->first == token;
	if (ok) {
		t.value = it->second;
		it++;
		parse_whitespaces(it);
	}
	return ok;
}

template <int token>
bool build(Token<token>& t, auto& it) {
	if constexpr (token == END)
		return build_end_token(t, it);
	else if constexpr (token == NEWLINE)
		return build_newline_token(it);
	else
		return build_normal_token(t, it);
}

template <typename T>
bool build(Until<T>& until, auto& it) {
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

inline bool build(IndentToken& indent, auto& it) {
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
bool build(Alloc<T>& alloc, auto& it) {
	T t(alloc.n_indent);
	if (build(t, it)) {
		alloc.value = std::move(t);
		return true;
	}
	return false;
}
//
//template <typename T>
//bool build(Expect<T>& expect, auto& it) {
//	bool built = build(expect.expected, it);
//	if (built)
//		return true;
//	else
//		throw parse_error{
//			grammar::name_of_rule<T>(),
//			it
//	};
//}

template <typename T>
bool build(Opt<T>& opt, auto& it) {
	T _node(opt.n_indent);
	bool parsed = build(_node, it);
	if (parsed)
		opt.node.emplace(std::move(_node));
	return true;
}

template <typename...>
struct commit_t;

template <typename T>
struct commit_t<T> {
	using type = void;
	using is_correct = std::true_type;
};

template <typename T>
struct commit_t<Commit<T>> {
	using type = T;
	using is_correct = std::true_type;
};

template <typename T, typename... Ts>
struct commit_t<T, Ts...> {
	using type = commit_t<Ts...>::type;
	using is_correct = commit_t<Ts...>::is_correct;
};

template <typename T, typename... Ts>
struct commit_t<Commit<T>, Ts...> {
	using type = T;
	using is_correct = std::is_same<typename commit_t<Ts...>::type, void>::type;
};

template <typename... Ands>
bool build(And<Ands...>& and_, auto& it) {
	using commit_t_ = commit_t<Ands...>::type;
	static_assert(std::is_same_v<typename commit_t<Ands...>::is_correct, std::true_type>, "don't use Commit<> twice in 1 And<>");
	if constexpr (std::is_same_v<commit_t_, void>) {
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
	else {
		bool failed = false;
		auto temp = it;
		bool commited = false;
		for_each(and_.value, [&](auto& node) {
			if (failed)
				return;
			if (!build(node, it)) {
				if (commited)
					throw parse_error{
						grammar::name_of_rule<And<Ands...>>(),
						it
				};
				it = temp;
				failed = true;
			}
			if constexpr (is_specialization<std::remove_cvref_t<decltype(node)>, Commit>::value)
				commited = true;
			});
		return !failed;
	}
}

template <typename... Ors>
bool build(Or<Ors...>& or_, auto& it) {
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

inline bool parse_empty_line(auto& it) {
	auto newline = Token<NEWLINE>{ 0 };
	return build(newline, it);
}

inline bool parse_indented_line(TemplateBody& body, auto& it) {
	auto cp = it;
	for (int i = 0; i < body.n_indent + 1; ++i) {
		bool is_indented = it->first != END && it->first == TAB;
		if (!is_indented) {
			it = cp;
			return false;
		}
		it++;
	}
	it--;
	auto until_newline = Until<Token<NEWLINE>>{ body.n_indent };
	bool has_newline = build(until_newline, it);
	_ASSERT(has_newline); // if that doesnt work something is just wrong with the tokenizer.
	return true;
}

inline bool parse_one_line(TemplateBody& body, auto& it) {
	return parse_empty_line(it) || parse_indented_line(body, it);
}

inline bool build(TemplateBody& body, auto& it) {
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
bool build(KNode<T, CND, requiresComma>& knode, auto& it) {
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

template <typename T>
bool build(Expect<T>& x, auto& it) {
	if (!build((T&)x, it))
		throw parse_error{
			grammar::name_of_rule<T>(),
			it
		};
	return true;
}

// specialize build for statements or newlines
bool build(Or<Token<NEWLINE>, Expect<grammar::Statement>>& x, auto& it) {
	// if its a new line OK
	{
		auto node = Token<NEWLINE>(x.n_indent);
		bool built = build(node, it);
		if (built) {
			x._value.emplace(std::move(node));
			return true;
		}
	}
	// if its not a new line
	{
		auto temp = it;
		auto indent_node = IndentToken{ x.n_indent };
		bool built = build(indent_node, temp);
		// and it is properly indented (neither less nor more indent)
		if (built) {
			// then we try to parse the statement (note the Expect<> which throws if it fails)
			auto node = Expect<grammar::Statement>(x.n_indent);
			bool built = build(node, it);
			if (built) {
				x._value.emplace(std::move(node));
				return true;
			}
		}
	}
	return false;
}
