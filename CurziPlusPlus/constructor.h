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

	virtual void build() override {
		this->nodes = {
			_AND_
				MAKE_NAMED(ClassMemberQualifiers, "ClassMemberQualifiers"),
				WORD_TOKEN("word"),
				TOKEN(PARENOPEN),
				MAKE_NAMED(ArgumentsSignatureNode, "ArgumentsSignature"),
				TOKEN(PARENCLOSE),
				MAKE_NAMED(ColonIndentCodeBlockNode, "ColonIndentCodeBlock"),
			__
		};
	}
};
