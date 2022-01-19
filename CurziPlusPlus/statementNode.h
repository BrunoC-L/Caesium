#pragma once
#include "node.h"
#include "grammarizer.h"
#include "tokenNode.h"
#include "typenameNode.h"
#include "argumentsNode.h"
#include "expressionNode.h"
#include "indentNode.h"

class StatementNode : public Node {
public:
	baseCtor(StatementNode);

	virtual void build() override;

	virtual void accept(NodeVisitor* v) override {
		v->visit(this);
	}
};

class CodeBlockNode : public Node {
public:
	baseCtor(CodeBlockNode);

	virtual void build() override {
		this->nodes = {
			_STAR_
				MAKE(StatementNode)(n_indent)
			___
		};
	}

	virtual void accept(NodeVisitor* v) override {
		v->visit(this);
	}
};

class ColonIndentCodeBlockNode : public Node {
public:
	baseCtor(ColonIndentCodeBlockNode);

	virtual void build() override {
		this->nodes = {
			_AND_
				TOKEN(COLON),
				TOKEN(NEWLINE),
				MAKE(CodeBlockNode)(n_indent + 1)
			__
		};
	}

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
				TOKEN(NEWLINE),
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
				MAKE(IndentNode)(n_indent),
				TOKEN(ELSE),
				MAKE(ColonIndentCodeBlockNode)(n_indent),
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
				TOKEN(IF),
				MAKE(ExpressionNode)(n_indent),
				MAKE(ColonIndentCodeBlockNode)(n_indent),
				_OPT_
					MAKE(ElseStatementNode)(n_indent)
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
				TOKEN(FOR),
				_OR_
					_AND_
						MAKE(TypenameNode)(),
						TOKEN(WORD),
					__,
					TOKEN(WORD),
				__,
				TOKEN(IN),
				MAKE(ExpressionNode)(),
				MAKE(ColonIndentCodeBlockNode)(n_indent),
			__
		};
	}

	virtual void accept(NodeVisitor* v) override {
		v->visit(this);
	}
};

class IForStatementNode : public Node {
public:
	baseCtor(IForStatementNode);

	virtual void build() override {
		this->nodes = {
			_AND_
				TOKEN(IFOR),
				TOKEN(WORD),
				TOKEN(COMMA),
				_OR_
					_AND_
						MAKE(TypenameNode)(),
						TOKEN(WORD),
					__,
					TOKEN(WORD),
				__,
				TOKEN(IN),
				MAKE(ExpressionNode)(),
				MAKE(ColonIndentCodeBlockNode)(n_indent),
			__
		};
	}

	virtual void accept(NodeVisitor* v) override {
		v->visit(this);
	}
};
