#pragma once
#include "node.h"
#include "grammarizer.h"
#include "usingNode.h"
#include "pppQualifierNode.h"
#include "macroNode.h"
#include "classMember.h"
#include "constructor.h"

template <typename T>
class ClassElementNode : public Node<T> {
public:
	baseCtor(ClassElementNode);

	virtual void build() override;

	virtual T accept(NodeVisitor<T>* v) override {
		return v->visit(this);
	}
};
