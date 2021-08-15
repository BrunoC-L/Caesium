#pragma once
#include "multipleInheritanceNode.h"

class ClassInheritanceNode : public Node {
public:
	baseCtor(ClassInheritanceNode);

	virtual void build() override {
		this->nodes = {
			_AND_
				MAKE(TokenNode)(COLON),
				MAKE(MultipleInheritanceNode)(),
			__
		};
	}

	virtual void accept(NodeVisitor* v) override {
		v->visit(this);
	}
};
