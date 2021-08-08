#pragma once
#include "tokenizer.h"
#include "node.h"

class Grammarizer {
public:
	std::forward_list<TOKENVALUE>& tokens;
	std::forward_list<TOKENVALUE>::iterator it;

	Grammarizer(std::forward_list<TOKENVALUE>& tokens) : tokens(tokens), it(tokens.begin()) {}
};
