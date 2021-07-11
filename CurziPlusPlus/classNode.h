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
				_OR_
					MAKE(ClassInheritanceNode)(),
					MAKE(EmptyNode)()
				__,
				MAKE(TokenNode)(BRACEOPEN),
			MAKE(ClassElementsNode)(),
		__,
		};
	}
};
