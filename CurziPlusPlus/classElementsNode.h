#pragma once
#include "node.h"
#include "grammarizer.h"
#include "usingNode.h"
#include "pppQualifierNode.h"
#include "macroNode.h"
#include "classMember.h"
#include "constructor.h"

class ClassElementNode : public Node {
public:
	baseCtor(ClassElementNode);

	virtual void build() override {
		nodes = {
			_OR_
				_AND_
					MAKE(PPPQualifierNode)(),
					MAKE(TokenNode)(COLON)
				__,
				MAKE(UsingNode)(),
				MAKE(MacroNode)(),
				MAKE(ClassMemberNode)(),
				MAKE(ConstructorNode)(),
			__,
		};
	}
};
