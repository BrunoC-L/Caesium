#pragma once
#include "tokenNode.h"
#include "andorNode.h"

class PPPQualifierNode : public Node {
public:
	baseCtor(PPPQualifierNode);
	virtual bool build(Grammarizer* g) override {
		return
		_OR_(vNode({
			MAKE(TokenNode)(PRIVATE),
			MAKE(TokenNode)(PROTECTED),
			MAKE(TokenNode)(PUBLIC)
		}))
#ifdef DEBUG
			->debugbuild(g);
#else
			->build(g);
#endif // DEBUG
	}
};
