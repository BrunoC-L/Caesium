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

	std::unique_ptr<NodeStructs::Function> getStruct() {
		std::unique_ptr<NodeStructs::Function> res = std::make_unique<NodeStructs::Function>();
		return res;
	}
};
