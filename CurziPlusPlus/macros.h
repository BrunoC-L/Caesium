#pragma once
#include <memory>

#define MAKE(T) std::make_shared<T>
#define MAKE2(T) std::make_shared<T>(n_indent)
#define MAKE_INDENTED(T, INDENT) std::make_shared<T>(INDENT)
#define TOKEN(T) std::make_shared<TokenNode<T>>()

#define _AND_			MAKE(AndNode      )(vNode({
#define _OR_			MAKE(OrNode       )(vNode({
#define _STAR_			MAKE(StarNode     )([&](){ return
#define _PLUS_			MAKE(PlusNode     )([&](){ return
#define _OPT_			MAKE(OPTNode      )([&](){ return
#define _COMMA_STAR_	MAKE(CommaStarNode)([&](){ return
#define _COMMA_PLUS_	MAKE(CommaPlusNode)([&](){ return

// for closing _AND_ & _OR_
#define __ }))
// for closing the others
#define ___ ;})
// for closing both at once
#define ____ __ ___

#define baseCtor(T) T(int n_indent = 0) : Node(n_indent) {}
