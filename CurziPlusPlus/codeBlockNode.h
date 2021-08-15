#pragma once
#include "node.h"
#include "macros.h"

class CodeBlockNode : public Node {
public:
	baseCtor(CodeBlockNode);

	virtual void build() override;

	virtual void accept(NodeVisitor* v) override {
		v->visit(this);
	}
};

class BracedCodeBlockNode : public Node {
public:
	baseCtor(BracedCodeBlockNode);

	virtual void build() override;

	virtual void accept(NodeVisitor* v) override {
		v->visit(this);
	}
};
