#pragma once
#include "node.h"
#include "grammarizer.h"

class AndNode : public Node {
	vNode nodes;
public:
	AndNode(vNode nodes) : nodes(nodes) { name = "AndNode"; }

	virtual bool build(Grammarizer* g) override {
		return g->And(nodes);
	}
};

class OrNode : public Node {
	vNode nodes;
public:
	OrNode(vNode nodes) : nodes(nodes) { name = "OrNode"; }

	virtual bool build(Grammarizer* g) override {
		return g->Or(nodes);
	}
};

#define _AND_ MAKE(AndNode)
#define _OR_ MAKE(OrNode)
