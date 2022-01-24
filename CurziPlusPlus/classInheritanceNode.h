#pragma once
#include "multipleInheritanceNode.h"

class ClassInheritanceNode : public Node {
public:
	baseCtor(ClassInheritanceNode);

	virtual void build() override {
		this->nodes = {
			_AND_
				TOKEN(EXTENDS),
				MAKE_NAMED(MultipleInheritanceNode, ""),
			__
		};
	}

	virtual void accept(NodeVisitor* v) override {
		v->visit(this);
	}
};
