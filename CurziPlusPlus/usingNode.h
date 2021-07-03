#pragma once
#include "node.h"
#include "grammarizer.h"
#include "tokenNode.h"
#include "typenameNode.h"

class UsingNode : public Node {
public:
	baseCtor(UsingNode);

	virtual bool build(Grammarizer* g) override {
		return
		_AND_(vNode({
			MAKE(TokenNode)(USING),
			MAKE(TokenNode)(WORD),
			MAKE(TokenNode)(EQUAL),
			MAKE(TypenameNode)(),
		}))
#ifdef DEBUG
		->debugbuild(g);
#else
		->build(g);
#endif // DEBUG
	}
};
