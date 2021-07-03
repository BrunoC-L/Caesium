#pragma once
#include "node.h"
#include "grammarizer.h"

class TokenNode : public Node {
public:
	TOKEN t;
	std::string value;
	TokenNode(TOKEN t) : t(t) { name = "TokenNode"; }

	virtual bool build(Grammarizer* g) override {
		while (
			g->it->first == NEWLINE ||
			g->it->first == TAB ||
			g->it->first == SPACE ||
			g->it->first == t
			) {
			value += g->it->second;
			bool isT = g->it->first == t;
			g->it++;
			if (isT)
				return true;
		}
		return false;
	}

	virtual bool debugbuild(Grammarizer* g) override {
		const auto& indent = g->getIndent();
		g->inc();
		bool worked = build(g);
		g->dec();
		if (worked)
			std::cout << indent << "TokenNode " << Tokenizer::tokenLookup[t] << " worked\n";
		else
			std::cout << indent << "TokenNode " << Tokenizer::tokenLookup[t] << " was " << Tokenizer::tokenLookup[g->it->first] << "\n";
		return worked;
	}
};
