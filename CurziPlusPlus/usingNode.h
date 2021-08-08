#pragma once
#include "node.h"
#include "grammarizer.h"
#include "tokenNode.h"
#include "typenameNode.h"

template <typename T>
class UsingNode : public Node<T> {
public:
	baseCtor(UsingNode);

	virtual void build() override {
		this->nodes = {
			_AND_
				MAKE(TokenNode)(USING),
				MAKE(TokenNode)(WORD),
				MAKE(TokenNode)(EQUAL),
				MAKE(TypenameNode)(),
			__
		};
	}

	virtual T accept(NodeVisitor<T>* v) override {
		return v->visit(this);
	}
};
