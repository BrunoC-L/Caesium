#pragma once
#include "node.h"
#include "grammarizer.h"
#include "tokenNode.h"

template <typename T>
class UntilTokenNode : public Node<T> {
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

	virtual T accept(NodeVisitor<T>* v) override {
		return v->visit(this);
	}
};
