#pragma once
#include "node.h"
#include "grammarizer.h"
#include "tokenNode.h"
#include "typenameNode.h"
#include "codeBlockNode.h"
#include "argumentsNode.h"
#include "expressionNode.h"

template <typename T>
class StatementNode : public Node<T> {
public:
	baseCtor(StatementNode);

	virtual void build() override;

	virtual T accept(NodeVisitor<T>* v) override {
		return v->visit(this);
	}
};

template <typename T>
class ElseStatementNode : public Node<T> {
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

	virtual T accept(NodeVisitor<T>* v) override {
		return v->visit(this);
	}
};

template <typename T>
class IfStatementNode : public Node<T> {
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

	virtual T accept(NodeVisitor<T>* v) override {
		return v->visit(this);
	}
};

template <typename T>
class ForStatementNode : public Node<T> {
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

	virtual T accept(NodeVisitor<T>* v) override {
		return v->visit(this);
	}
};
