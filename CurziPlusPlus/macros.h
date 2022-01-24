#pragma once
#include <memory>

#define MAKE(T) std::make_shared<T>
#define MAKE_NAMED(T, NAME) std::make_shared<T>(NAME, n_indent)
#define MAKE_NAMED_INDENTED(T, NAME, INDENT) std::make_shared<T>(NAME, INDENT)
#define TOKEN(T) std::make_shared<TokenNode<T>>("")
#define WORD_TOKEN(NAME) MAKE(WordTokenNode)(NAME)
#define NUMBER_TOKEN(NAME) MAKE(NumberTokenNode)(NAME)

#define _AND_				MAKE(AndNode      )(vNode({
#define _OR_				MAKE(OrNode       )(vNode({
#define _STAR_(NAME)		MAKE(StarNode     )(NAME, [&](){ return
#define _PLUS_(NAME)		MAKE(PlusNode     )(NAME, [&](){ return
#define _OPT_(NAME)			MAKE(OPTNode      )(NAME, [&](){ return
#define _COMMA_STAR_(NAME)	MAKE(CommaStarNode)(NAME, [&](){ return
#define _COMMA_PLUS_(NAME)	MAKE(CommaPlusNode)(NAME, [&](){ return

// for closing _AND_ & _OR_
#define __ }))
// for closing the others
#define ___ ;})
// for closing both at once
#define ____ __ ___

#define baseCtor(T)\
T(std::string name, int n_indent = 0) : Node(name, n_indent) {\
	this->name = #T;\
}
