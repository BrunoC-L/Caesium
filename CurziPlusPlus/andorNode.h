#pragma once
#include "node.h"
#include "grammarizer.h"

class AndNode : public Node {
public:
	AndNode(vNode v) {
		name = "AndNode";
		nodes = v;
	}

	virtual bool build(Grammarizer* g) override {
		return g->And(nodes);
	}
};

class OrNode : public Node {
public:
	OrNode(vNode v) {
		name = "OrNode";
		nodes = v;
	}

	virtual bool build(Grammarizer* g) override {
		return g->Or(nodes);
	}
};
