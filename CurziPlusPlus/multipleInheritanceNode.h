#pragma once
#include "singleInheritanceNode.h"
#include "macros.h"

class MultipleInheritanceNode : public Node {
public:
	baseCtor(MultipleInheritanceNode);

	virtual void build() override {
		this->nodes = {
		_OR_
			_AND_
				MAKE(SingleInheritanceNode)(),
				MAKE(TokenNode)(COMMA),
				MAKE(MultipleInheritanceNode)()
			__,
			MAKE(SingleInheritanceNode)(),
		__,
		};
	}

	virtual void accept(NodeVisitor* v) override {
		v->visit(this);
	}
};
