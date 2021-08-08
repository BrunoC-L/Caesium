#pragma once
#include "node.h"
#include "macros.h"

template <typename T>
class CodeBlockNode : public Node<T> {
public:
	baseCtor(CodeBlockNode);

	virtual void build() override;

	virtual T accept(NodeVisitor<T>* v) override {
		return v->visit(this);
	}
};
