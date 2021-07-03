#pragma once
#include "tokenizer.h"
#include "node.h"

#define DEBUG

#define MAX_INDENTS 200

class Grammarizer {
protected:
	std::string indents[MAX_INDENTS];
	int indent = 0;
public:
	std::forward_list<TOKENVALUE>& tokens;
	std::forward_list<TOKENVALUE>::iterator it;

	const std::string& getIndent() {
		return indents[indent];
	}
	void inc() {
		++indent;
	}
	void dec() {
		--indent;
	}

	Grammarizer(std::forward_list<TOKENVALUE>& tokens) : tokens(tokens), it(tokens.begin()) {
		indents[0] = "";
		for (int i = 1; i < MAX_INDENTS; ++i)
			indents[i] = indents[i - 1] + "|";
	}

#ifdef DEBUG
	bool And(const vNode& wishlist) {
		auto temp = it;
		for (const auto& w : wishlist)
			if (!w->debugbuild(this)) {
				it = temp;
				return false;
			}
		return true;
	}

	bool Or(const vNode& wishlist) {
		for (const auto& w : wishlist)
			if (w->debugbuild(this))
				return true;
		return false;
	}
#else
	bool And(const vNode& wishlist) {
		auto temp = it;
		for (const auto& w : wishlist)
			if (!w->build(this)) {
				it = temp;
				return false;
			}
		return true;
	}

	bool Or(const vNode& wishlist) {
		for (const auto& w : wishlist)
			if (w->build(this))
				return true;
		return false;
	}
#endif // DEBUG
};
