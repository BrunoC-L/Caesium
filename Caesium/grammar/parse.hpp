#pragma once
#include "grammar.hpp"

void parse_whitespaces(Iterator& it);

template <token_t token>
bool build_end_token(Token<token>& t, Iterator& it) {
	t.beg_offset = it.index;
	bool ok = it.vec[it.index].first == token;
	if (ok)
		it.index++;
	t.end_offset = it.index;
	return ok;
}

bool build_newline_token(Iterator& it);

template <token_t token>
bool build_normal_token(Token<token>& t, Iterator& it) {
	bool ok = it.vec[it.index].first == token;
	if (ok) {
		t.value = it.vec[it.index].second;
		it.index++;
		parse_whitespaces(it);
	}
	return ok;
}

template <token_t token>
bool build(Token<token>& t, Iterator& it) {
	t.beg_offset = it.index;
	auto res = [&]() {
		if constexpr (token == END)
			return build_end_token(t, it);
		else if constexpr (token == NEWLINE)
			return build_newline_token(it);
		else
			return build_normal_token(t, it);
		}();
	t.end_offset = it.index;
	return res;
}

template <typename T>
bool build(Until<T>& until, Iterator& it) {
	until.beg_offset = it.index;
	T t{ until.n_indent };
	auto save = it.index;
	while (it.vec[it.index].first != END) {
		if (build(t, it)) {
			until.end_offset = it.index;
			return true;
		}
		it.index++;
	}
	it.index = save;
	return false;
}

bool build(IndentToken& indent, Iterator& it);

template <typename T>
bool build(Alloc<T>& alloc, Iterator& it) {
	alloc.beg_offset = it.index;
	T t(alloc.n_indent);
	if (build(t, it)) {
		alloc.value = std::move(t);
		alloc.end_offset = it.index;
		return true;
	}
	return false;
}

template <typename T>
bool build(Opt<T>& opt, Iterator& it) {
	opt.beg_offset = it.index;
	T _node(opt.n_indent);
	bool parsed = build(_node, it);
	if (parsed)
		opt.node.emplace(std::move(_node));
	opt.end_offset = it.index;
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
bool build(And<Ands...>& and_, Iterator& it) {
	using commit_t_ = commit_t<Ands...>::type;
	static_assert(std::is_same_v<typename commit_t<Ands...>::is_correct, std::true_type>, "don't use Commit<> twice in 1 And<>");
	and_.beg_offset = it.index;
	if constexpr (std::is_same_v<commit_t_, void>) {
		bool failed = false;
		auto temp = it.index;
		for_each(and_.value, [&](auto& node) {
			if (failed)
				return;
			if (!build(node, it)) {
				it.index = temp;
				failed = true;
			}
		});
		and_.end_offset = it.index;
		return !failed;
	}
	else {
		bool failed = false;
		auto temp = it.index;
		bool commited = false;
		for_each(and_.value, [&](auto& node) {
			if (failed)
				return;
			if (!build(node, it)) {
				if (commited)
					throw parse_error{
						.name_of_rule = grammar::name_of_rule<And<Ands...>>(),
						.beg_offset = temp,
						.index = it.index,
						.line = it.line,
						.col = it.col,
						.vec = it.vec,
						.file_name = it.file_name
				};
				it.index = temp;
				failed = true;
			}
			if constexpr (is_specialization<std::remove_cvref_t<decltype(node)>, Commit>::value)
				commited = true;
		});
		and_.end_offset = it.index;
		return !failed;
	}
}

template <typename... Ors>
bool build(Or<Ors...>& or_, Iterator& it) {
	or_.beg_offset = it.index;
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
	or_.end_offset = it.index;
	return populated;
}

bool parse_empty_line(Iterator& it);

bool parse_indented_line(TemplateBody& body, Iterator& it);

bool parse_one_line(TemplateBody& body, Iterator& it);

bool build(TemplateBody& body, Iterator& it);

template <typename T, typename CND, typename requiresComma>
bool build(KNode<T, CND, requiresComma>& knode, Iterator& it) {
	knode.beg_offset = it.index;
	while (true) {
		auto node = T{ knode.n_indent };
		node.beg_offset = it.index;
		bool parsed = build(node, it);
		node.end_offset = it.index;
		if (parsed) {
			knode.nodes.push_back(std::move(node));
			if constexpr (requiresComma::value) {
				auto comma = Token<COMMA>(0);
				comma.beg_offset = it.index;
				parsed = build(comma, it);
				comma.beg_offset = it.index;
			}
		}
		if (!parsed)
			break;
	}
	knode.end_offset = it.index;
	return CND::cnd(knode.nodes);
}

template <typename T>
bool build(Expect<T>& x, Iterator& it) {
	x.beg_offset = it.index;
	if (!build((T&)x, it))
		throw parse_error{
			.name_of_rule = grammar::name_of_rule<T>(),
			.beg_offset = x.beg_offset,
			.index = it.index,
			.line = it.line,
			.col = it.col,
			.vec = it.vec,
			.file_name = it.file_name
		};
	x.end_offset = it.index;
	return true;
}

// specialize build for following types:
bool build(grammar::TypenameOrExpression& x, Iterator& it);
bool build(grammar::CompareOperator&, Iterator&);

template <typename context>
bool build(Or<Token<NEWLINE>, Expect<grammar::Statement<context>>>& x, Iterator& it) {
	x.beg_offset = it.index;
	// if its a new line OK
	{
		auto node = Token<NEWLINE>(x.n_indent);
		bool built = build(node, it);
		if (built) {
			x._value.emplace(std::move(node));
			x.end_offset = it.index;
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
			auto node = Expect<grammar::Statement<context>>(x.n_indent);
			bool built = build(node, it);
			if (built) {
				x._value.emplace(std::move(node));
				x.end_offset = it.index;
				return true;
			}
		}
	}
	return false;
}
