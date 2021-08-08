#pragma once
#include "multipleInheritanceNode.h"

template <typename T>
class ClassInheritanceNode : public Node<T> {
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

	virtual T accept(NodeVisitor<T>* v) override {
		return v->visit(this);
	}
};
