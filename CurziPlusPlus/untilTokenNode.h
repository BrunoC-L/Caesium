#pragma once
#include "node.h"
#include "grammarizer.h"
#include "tokenNode.h"

class UntilTokenNode : public Node {
public:
	TOKEN t;
	std::string value;
	UntilTokenNode(TOKEN t) : t(t) { this->name = "UntilTokenNode"; }

	virtual bool build(Grammarizer* g) override {
		while (g->it != g->tokens.end()) {
			value += g->it->second;
			bool isT = g->it->first == t;
			g->it++;
			if (isT)
				return true;
		}
		return false;
	}

	virtual void accept(NodeVisitor* v) override {
		v->visit(this);
	}
};
