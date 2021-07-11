#pragma once
#include "node.h"
#include "grammarizer.h"
#include "tokenNode.h"
#include "typenameNode.h"
#include "untilTokenNode.h"

class MacroNode : public Node {
public:
	baseCtor(MacroNode);

	virtual void build() override {
		nodes = {
		_AND_
			MAKE(TokenNode)(DEFINE),
			MAKE(TokenNode)(WORD),
			MAKE(TokenNode)(EQUAL),
			MAKE(UntilTokenNode)(NEWLINE),
		}))
		};
	}
};
