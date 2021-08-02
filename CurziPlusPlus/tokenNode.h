#pragma once
#include "node.h"
#include "grammarizer.h"

class TokenNode : public Node {
public:
	TOKEN t;
	std::string value;
	std::stringstream stream;
	TokenNode(TOKEN t) : t(t) { name = "TokenNode"; }

	bool _build(Grammarizer* g) {
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

	virtual JSON toJSON() override {
		JSON res;
		res[Tokenizer::tokenLookup[t]] = '"' + value + '"';
		return res;
	}

	virtual bool build(Grammarizer* g) override {
#ifdef DEBUG
		const auto& indent = g->getIndent();
		g->inc();
		bool worked = _build(g);
		g->dec();
		if (worked)
			std::cout << indent << "TokenNode " << Tokenizer::tokenLookup[t] << " worked\n";
		else
			std::cout << indent << "TokenNode " << Tokenizer::tokenLookup[t] << " was " << Tokenizer::tokenLookup[g->it->first] << "\n";
		return worked;
#else
		return _build(g);
#endif // DEBUG
	}
};
