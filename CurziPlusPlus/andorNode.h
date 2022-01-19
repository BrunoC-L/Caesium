#pragma once
#include "node.h"
#include "grammarizer.h"
#include "tokenNode.h"

class AndNode : public Node {
public:
	AndNode(vNode v, int n_indent = 0) : Node(n_indent) {
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

	virtual void accept(NodeVisitor* v) override {
		v->visit(this);
	}
};

class OrNode : public Node {
public:
	OrNode(vNode v, int n_indent = 0) : Node(n_indent) {
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

	virtual void accept(NodeVisitor* v) override {
		v->visit(this);
	}
};
