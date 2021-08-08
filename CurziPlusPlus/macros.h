#pragma once
#include <memory>

#define MAKE(N) std::make_shared<N<T>>

#define _AND_ MAKE(AndNode)(vNode<T>({
#define _OR_ MAKE(OrNode)(vNode<T>({

#define _STAR_ MAKE(StarNode)([&](){ return
#define _PLUS_ MAKE(PlusNode)([&](){ return
#define _OPT_  MAKE(OPTNode )([&](){ return

#define __ }))
#define ___ ;})
#define ____ __ ___

#define baseCtor(T) T() {\
	this->name = #T;\
}
