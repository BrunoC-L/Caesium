#pragma once
#include "tokenizer.h"
#include "node.h"

//#define DEBUG

#define MAX_INDENTS 1000

class Grammarizer {
protected:
	std::string indents[MAX_INDENTS];
	int indent = 0;
public:
	std::forward_list<TOKENVALUE>& tokens;
	std::forward_list<TOKENVALUE>::iterator it;

	const std::string& getIndent() {
		_ASSERT(indent < MAX_INDENTS);
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
};
