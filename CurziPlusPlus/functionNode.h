#pragma once
#include "node.h"
#include "macros.h"
#include "statementNode.h"

class FunctionNode : public Node {
public:
	baseCtor(FunctionNode);

	virtual void build() override {
		this->nodes = {
			_AND_
				MAKE(TypenameNode)(),
				WORD_TOKEN(),
				TOKEN(PARENOPEN),
				MAKE(ArgumentsSignatureNode)(),
				TOKEN(PARENCLOSE),
				MAKE(ColonIndentCodeBlockNode)(n_indent),
			__,
		};
	}

	virtual void accept(NodeVisitor* v) override {
		v->visit(this);
	}
};
