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
				MAKE_NAMED(TypenameNode, "Typename"),
				WORD_TOKEN("word"),
				TOKEN(PARENOPEN),
				MAKE_NAMED(ArgumentsSignatureNode, "ArgumentsSignature"),
				TOKEN(PARENCLOSE),
				MAKE_NAMED(ColonIndentCodeBlockNode, "ColonIndentCodeBlock"),
			__,
		};
	}

	virtual void accept(NodeVisitor* v) override {
		v->visit(this);
	}
};
