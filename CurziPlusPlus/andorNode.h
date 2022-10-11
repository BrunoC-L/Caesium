#pragma once
#include "node.h"
#include "grammarizer.h"
#include "tokenNode.h"

class AndNode : public Node {
public:
	AndNode(vNode v, int n_indent = 0) : Node(n_indent) {
		this->nodes = v;
	}

	virtual bool build(Grammarizer* g) override {
		auto temp = g->it;
		for (const auto& node : this->nodes)
			if (!node->build(g)) {
				g->it = temp;
				this->nodes = {};
				return false;
			}
		return true;
	}
};

class OrNode : public Node {
public:
	OrNode(vNode v, int n_indent = 0) : Node(n_indent) {
		this->nodes = v;
	}

	virtual bool build(Grammarizer* g) override {
		for (const auto& node : this->nodes)
			if (node->build(g)) {
				this->nodes = { node };
				return true;
			}
		this->nodes = {};
		return false;
	}
};

template <typename... Ors>
class OrNode2 {
public:
	int n_indent;
	using variant_t = std::variant<Ors...>;
	variant_t value;
	OrNode2(int n_indent = 0) : n_indent(n_indent) {}

	bool build(Grammarizer* g) {
		bool populated = false;
		([&]{
			if (populated)
				return;
			Ors node = Ors(0);
			bool built = node.build(g);
			if (built) {
				value = std::move(node);
				populated = true;
			}
		}(), ...);
		return populated;
	}
};

// https://blog.tartanllama.xyz/exploding-tuples-fold-expressions/
template <std::size_t... Idx> auto make_index_dispatcher(std::index_sequence<Idx...>) {return [](auto&& f) { (f(std::integral_constant<std::size_t, Idx>{}), ...); };}
template <std::size_t N> auto make_index_dispatcher() {return make_index_dispatcher(std::make_index_sequence<N>{});}
template <typename Tuple, typename Func> void for_each(Tuple&& t, Func&& f) {make_index_dispatcher<std::tuple_size<std::decay_t<Tuple>>::value>()([&f, &t](auto idx) { f(std::get<idx>(std::forward<Tuple>(t))); });}

template <typename... Ands>
class AndNode2 {
public:
	int n_indent;
	using tuple_t = std::tuple<Ands...>;
	tuple_t value;
	AndNode2(int n_indent = 0) : n_indent(n_indent) {
		value = { Ands(n_indent)... };
	}

	bool build(Grammarizer* g) {
		bool failed = false;
		auto temp = g->it;
		for_each(value, [&](auto& node) {
			if (!failed && !node.build(g)) {
				g->it = temp;
				failed = true;
			}
		});
		return !failed;
	}
};
