#pragma once
#include "tokenNode.h"
#include "andorNode.h"
#include "macros.h"

template <typename T>
class PPPQualifierNode : public Node<T> {
public:
	baseCtor(PPPQualifierNode);

	virtual void build() override {
		this->nodes = {
			_OR_
				MAKE(TokenNode)(PRIVATE),
				MAKE(TokenNode)(PROTECTED),
				MAKE(TokenNode)(PUBLIC)
			__
		};
	}

	virtual T accept(NodeVisitor<T>* v) override {
		return v->visit(this);
	}
};
