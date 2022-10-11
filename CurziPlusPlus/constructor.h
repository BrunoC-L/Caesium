#pragma once
#include "node.h"
#include "grammarizer.h"
#include "tokenNode.h"
#include "typenameNode.h"
#include "classMemberQualifiers.h"
#include "argumentsNode.h"

class ConstructorNode : public Node {
public:
	baseCtor(ConstructorNode);

	virtual void prepare() override {
		this->nodes = {
			_AND_
				MAKE2(ClassMemberQualifiers),
				TOKEN(WORD),
				TOKEN(PARENOPEN),
				MAKE2(ArgumentsSignatureNode),
				TOKEN(PARENCLOSE),
				MAKE2(ColonIndentCodeBlockNode),
			__
		};
	}
};
