#pragma once
#include "node.h"
#include "macros.h"
#include "statementNode.h"

class FunctionNode : public Node {
public:
	baseCtor(FunctionNode);

	virtual void prepare() override {
		this->nodes = {
			_AND_
				MAKE2(TypenameNode),
				TOKEN(WORD),
				TOKEN(PARENOPEN),
				MAKE2(ArgumentsSignatureNode),
				TOKEN(PARENCLOSE),
				MAKE2(ColonIndentCodeBlockNode),
			__,
		};
	}

	std::unique_ptr<NodeStructs::Function> getStruct() {
		std::unique_ptr<NodeStructs::Function> res = std::make_unique<NodeStructs::Function>();
		return res;
	}
};
