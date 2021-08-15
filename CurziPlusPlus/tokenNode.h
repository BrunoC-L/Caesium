#pragma once
#include "node.h"
#include "grammarizer.h"

class TokenNode : public Node {
public:
	TOKEN t;
	std::string value;
	std::stringstream stream;
	TokenNode(TOKEN t) : t(t) { this->name = "TokenNode"; }

	virtual void accept(NodeVisitor* v) override {
		v->visit(this);
	}

	virtual bool build(Grammarizer* g) override {
		while (
			g->it->first == NEWLINE ||
			g->it->first == TAB ||
			g->it->first == SPACE ||
			g->it->first == t
			) {
			stream << g->it->second;
			bool isT = g->it->first == t;
			g->it++;
			if (isT) {
				value = stream.str();
				return true;
			}
		}
		return false;
	}
};
