#pragma once
#include "node.h"
#include "grammarizer.h"
#include "tokenNode.h"
#include "typenameNode.h"
#include "emptyNode.h"
#include "codeBlockNode.h"
#include "argumentsNode.h"
#include "expressionNode.h"

class StatementNode : public Node {
public:
	baseCtor(StatementNode);

	virtual void build() override;
};

class IfStatementNode : public Node {
public:
	baseCtor(IfStatementNode);

	virtual void build() override {
		nodes = {
			_AND_
				MAKE(TokenNode)(IF),
				MAKE(TokenNode)(PARENOPEN),
				MAKE(ExpressionNode)(),
				MAKE(TokenNode)(PARENCLOSE),
				MAKE(CodeBlockNode)(),
			__
		};
	}
};
