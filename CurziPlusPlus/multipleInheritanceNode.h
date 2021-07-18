#pragma once
#include "singleInheritanceNode.h"
#include "macros.h"

class MultipleInheritanceNode : public Node {
public:
	baseCtor(MultipleInheritanceNode);

	virtual void build() override {
		nodes = {
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
};
