#pragma once
#include "singleInheritanceNode.h"
#include "macros.h"

template <typename T>
class MultipleInheritanceNode : public Node<T> {
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

	virtual T accept(NodeVisitor<T>* v) override {
		return v->visit(this);
	}
};
