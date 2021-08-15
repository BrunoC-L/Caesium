#pragma once
#include "node.h"
#include "grammarizer.h"
#include "tokenNode.h"
#include "typenameNode.h"
#include "classMemberQualifiers.h"
#include "argumentsNode.h"
#include "codeBlockNode.h"

class ConstructorNode : public Node {
public:
	baseCtor(ConstructorNode);

	virtual void build() override {
		this->nodes = {
			_AND_
				MAKE(ClassMemberQualifiers)(),
				MAKE(TokenNode)(WORD),
				MAKE(TokenNode)(PARENOPEN),
				MAKE(ArgumentsSignatureNode)(),
				MAKE(TokenNode)(PARENCLOSE),
				MAKE(CodeBlockNode)(),
			__
		};
	}

	virtual void accept(NodeVisitor* v) override {
		v->visit(this);
	}
};
