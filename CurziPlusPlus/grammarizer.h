#pragma once
#include "tokenizer.h"

class Grammarizer {
public:
	std::forward_list<TOKENVALUE> tokens;
	std::forward_list<TOKENVALUE>::iterator it;

	Grammarizer(std::forward_list<TOKENVALUE>&& _tokens) : tokens(std::move(_tokens)) {
		it = tokens.begin();
	}
};
