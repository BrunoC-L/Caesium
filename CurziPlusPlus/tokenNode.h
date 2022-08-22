#pragma once
#include "node.h"
#include "grammarizer.h"

class TokenNode_ : public Node {
public:
	std::string value;
	TokenNode_(std::string name = "") : Node(name) { this->name = "TokenNode"; }
};

template <int token>
class TokenNode : public TokenNode_ {
	static_assert(token != TAB  , "Using TokenNode<TAB> will not work, trailing tabs are ignored, use IndentNode");
	static_assert(token != SPACE, "Using TokenNode<SPACE> will not work, trailing spaces are ignored");
public:
	TokenNode(std::string name) : TokenNode_(name) {}
	virtual bool build(Grammarizer* g) override {
		bool isT = g->it->first == token;
		if (isT && token != END)
			value = g->it->second;
		if (!isT)
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

class PublicToken : public TokenNode<PUBLIC> {
public:
	PublicToken() : TokenNode<PUBLIC>("") {}
};

class ProtectedToken : public TokenNode<PROTECTED> {
public:
	ProtectedToken() : TokenNode<PROTECTED>("") {}
};

class PrivateToken : public TokenNode<PRIVATE> {
public:
	PrivateToken() : TokenNode<PRIVATE>("") {}
};

class WordTokenNode : public TokenNode<WORD> {
public:
	WordTokenNode(std::string name) : TokenNode<WORD>(name) {}
};

class NumberTokenNode : public TokenNode<NUMBER> {
public:
	NumberTokenNode(std::string name) : TokenNode<NUMBER>(name) {}
};
