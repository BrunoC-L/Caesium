#pragma once
#include "node.h"
#include "grammarizer.h"
#include "tokenNode.h"

template <typename T>
class AndNode : public Node<T> {
public:
	AndNode(vNode<T> v) {
		this->name = "AndNode";
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

	virtual T accept(NodeVisitor<T>* v) override {
		throw 1;
	}
};

template <typename T>
class OrNode : public Node<T> {
public:
	OrNode(vNode<T> v) {
		this->name = "OrNode";
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

	virtual T accept(NodeVisitor<T>* v) override {
		return v->visit(this);
	}
};
