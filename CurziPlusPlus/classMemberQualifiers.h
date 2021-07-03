#pragma once
#include "node.h"
#include "grammarizer.h"
#include "emptyNode.h"
#include "pppQualifierNode.h"

class ClassMemberQualifiers : public Node {
public:
	baseCtor(ClassMemberQualifiers);

	virtual bool build(Grammarizer* g) override {
		return
		_AND_(vNode({
			_OR_(MAKE(PPPQualifierNode)(), MAKE(EmptyNode)()),
			_OR_(MAKE(TokenNode)(STATIC) , MAKE(EmptyNode)()),
		}))
#ifdef DEBUG
		->debugbuild(g);
#else
		->build(g);
#endif // DEBUG
	}
};
