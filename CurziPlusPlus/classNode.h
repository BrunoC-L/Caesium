#pragma once
#include "classInheritanceNode.h"
#include "classElementsNode.h"

class ClassNode : public Node {
public:
	baseCtor(ClassNode);

	virtual bool build(Grammarizer* g) override {
		return Node::build(g);
	}

	virtual void build() override {
		nodes = {
		_AND_
				MAKE(TokenNode)(CLASS),
				MAKE(TokenNode)(WORD),
				_OPT_
					MAKE(ClassInheritanceNode)()
				___,
				MAKE(TokenNode)(BRACEOPEN),
				_STAR_
					MAKE(ClassElementNode)()
				___,
				MAKE(TokenNode)(BRACECLOSE),
		__,
		};
	}
};
