#pragma once
#include "node.h"
#include "grammarizer.h"

class IndentNode : public Node {
public:
	IndentNode(int n_indent) : Node("indent", n_indent) {}

	virtual void accept(NodeVisitor* v) override {
		v->visit(this);
	}

	virtual bool build(Grammarizer* g) override {
		bool correct = true;
		for (int i = 0; i < n_indent; ++i) {
			correct &= g->it->first == TAB;
			g->it++;
			if (g->it == g->tokens.end())
				return false;
		}
		correct &= g->it->first != TAB && g->it->first != NEWLINE && g->it->first != SPACE;
		return correct;
	}
};
