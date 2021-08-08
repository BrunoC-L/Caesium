#pragma once
#include "classInheritanceNode.h"
#include "classelementnode.h"

template <typename T>
class ClassNode : public Node<T> {
public:
	baseCtor(ClassNode);

	virtual void build() override {
		this->nodes = {
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

	virtual T accept(NodeVisitor<T>* v) {
		return v->visit(this);
	}
};
