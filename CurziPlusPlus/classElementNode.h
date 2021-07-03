#pragma once
#include "node.h"
#include "grammarizer.h"
#include "usingNode.h"
#include "pppQualifierNode.h"
#include "macroNode.h"
#include "classMember.h"

class ClassElementNode : public Node {
public:
	baseCtor(ClassElementNode);
	virtual bool build(Grammarizer* g) override {
		return
		_OR_(vNode({
			_AND_(vNode({MAKE(PPPQualifierNode)(), MAKE(TokenNode)(COLON)})),
			MAKE(UsingNode)(),
			MAKE(MacroNode)(),
			MAKE(ClassMemberNode)(),
		}))
#ifdef DEBUG
		->debugbuild(g);
#else
		->build(g);
#endif // DEBUG
	}
};
