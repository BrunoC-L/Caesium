#pragma once
#include "node.h"
#include "grammarizer.h"

class TokenNode_ : public Node {
public:
	std::string value;
};

template <int token>
class TokenNode : public TokenNode_ {
	static_assert(token != TAB  , "Using TokenNode<TAB> will not work, trailing tabs are ignored, use IndentNode");
	static_assert(token != SPACE, "Using TokenNode<SPACE> will not work, trailing spaces are ignored");
public:
	TokenNode() : TokenNode_() {}
	virtual bool build(Grammarizer* g) override {
		bool isT = g->it->first == token;
		if (isT)
			value = g->it->second;
		if constexpr (token == END) {
			while (g->it != g->tokens.end()) {
				if (g->it->first == TAB || g->it->first == SPACE || g->it->first == NEWLINE)
					g->it++;
				else if (g->it->first == END)
					return true;
				else break;
			}
			return false;
		}
		else if (!isT)
			return false;

		g->it++;
		if constexpr (token == NEWLINE) {
			auto savepoint = g->it;
			while (g->it != g->tokens.end() && (g->it->first == TAB || g->it->first == SPACE || g->it->first == NEWLINE)) {
				if (g->it->first == NEWLINE) {
					g->it++;
					savepoint = g->it;
				}
				else {
					g->it++;
				}
			}
			g->it = savepoint;
			return true;
		}
		while (g->it != g->tokens.end() && (g->it->first == TAB || g->it->first == SPACE)) // ignoring trailing tabs & spaces
			g->it++;
		return true;
	}
};
