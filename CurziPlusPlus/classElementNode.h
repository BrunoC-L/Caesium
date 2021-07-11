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

	virtual void build() override {
		nodes = { _OR_
			_AND_MAKE(PPPQualifierNode)(), MAKE(TokenNode)(COLON)})),
			MAKE(UsingNode)(),
			MAKE(MacroNode)(),
			MAKE(ClassMemberNode)(),
		})) };
	}
};
