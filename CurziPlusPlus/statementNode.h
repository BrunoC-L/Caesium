#pragma once
#include "node.h"
#include "grammarizer.h"
#include "tokenNode.h"
#include "typenameNode.h"
#include "codeBlockNode.h"
#include "argumentsNode.h"
#include "expressionNode.h"

class StatementNode : public Node {
public:
	baseCtor(StatementNode);

	virtual void build() override;

	virtual void accept(NodeVisitor* v) override {
		v->visit(this);
	}
};

class ExpressionStatementNode : public Node {
public:
	baseCtor(ExpressionStatementNode);

	virtual void build() override {
		this->nodes = {
			_AND_
				MAKE(ExpressionNode)(),
				MAKE(TokenNode)(SEMICOLON),
			__
		};
	}

	virtual void accept(NodeVisitor* v) override {
		v->visit(this);
	}
};

class ElseStatementNode : public Node {
public:
	baseCtor(ElseStatementNode);

	virtual void build() override {
		this->nodes = {
			_AND_
				MAKE(TokenNode)(ELSE),
				MAKE(CodeBlockNode)(),
			__
		};
	}

	virtual void accept(NodeVisitor* v) override {
		v->visit(this);
	}
};

class IfStatementNode : public Node {
public:
	baseCtor(IfStatementNode);

	virtual void build() override {
		this->nodes = {
			_AND_
				MAKE(TokenNode)(IF),
				MAKE(TokenNode)(PARENOPEN),
				MAKE(ExpressionNode)(),
				MAKE(TokenNode)(PARENCLOSE),
				MAKE(CodeBlockNode)(),
				_OPT_
					MAKE(ElseStatementNode)()
				___
			__
		};
	}

	virtual void accept(NodeVisitor* v) override {
		v->visit(this);
	}
};

class ForStatementNode : public Node {
public:
	baseCtor(ForStatementNode);

	virtual void build() override {
		this->nodes = {
			_AND_
				MAKE(TokenNode)(FOR),
				MAKE(TokenNode)(PARENOPEN),
				_OPT_ _OR_
					_AND_
						MAKE(TypenameNode)(),
						MAKE(TokenNode)(WORD),
						_OPT_ _AND_
							MAKE(TokenNode)(EQUAL),
							MAKE(ExpressionNode)(),
						____
					__,
					MAKE(ExpressionNode)(),
				____,
				MAKE(TokenNode)(SEMICOLON),
				_OPT_
					MAKE(ExpressionNode)()
				___,
				MAKE(TokenNode)(SEMICOLON),
				_OPT_
					MAKE(ExpressionNode)()
				___,
				MAKE(TokenNode)(PARENCLOSE),
				MAKE(CodeBlockNode)(),
			__
		};
	}

	virtual void accept(NodeVisitor* v) override {
		v->visit(this);
	}
};
