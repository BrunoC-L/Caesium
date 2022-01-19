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
				MAKE(ClassMemberQualifiers)(),
				TOKEN(WORD),
				TOKEN(PARENOPEN),
				MAKE(ArgumentsSignatureNode)(),
				TOKEN(PARENCLOSE),
				MAKE(ColonIndentCodeBlockNode)(n_indent),
			__
		};
	}

	virtual void accept(NodeVisitor* v) override {
		v->visit(this);
	}
};
