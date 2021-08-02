#pragma once
#include <memory>

#define MAKE(T) std::make_shared<T>

#define _AND_ MAKE(AndNode)(vNode({
#define _OR_ MAKE(OrNode)(vNode({

#define _STAR_ MAKE(StarNode)([&](){ return
#define _PLUS_ MAKE(PlusNode)([&](){ return
#define _OPT_  MAKE(OPTNode )([&](){ return

#define __ }))
#define ___ ;})
#define ____ __ ___

#define baseCtor(T) T() {\
	name = #T;\
}
