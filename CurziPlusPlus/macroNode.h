#pragma once
#include "node.h"
#include "grammarizer.h"
#include "tokenNode.h"
#include "typenameNode.h"
#include "untilTokenNode.h"

class MacroNode : public Node {
public:
	baseCtor(MacroNode);

	virtual bool build(Grammarizer* g) override {
		return
		_AND_(vNode({
			MAKE(TokenNode)(DEFINE),
			MAKE(TokenNode)(WORD),
			MAKE(TokenNode)(EQUAL),
			MAKE(UntilTokenNode)(NEWLINE),
		}))
#ifdef DEBUG
		->debugbuild(g);
#else
		->build(g);
#endif // DEBUG
	}
};
