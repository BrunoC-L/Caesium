#pragma once
#include "node.h"
#include "grammarizer.h"
#include "tokenNode.h"
#include "typenameNode.h"
#include "argumentsNode.h"
#include "expressionNode.h"
#include "indentNode.h"

class CodeBlockNode : public Node {
public:
	baseCtor(CodeBlockNode);

	virtual void prepare() override;
};

class ColonIndentCodeBlockNode : public Node {
public:
	baseCtor(ColonIndentCodeBlockNode);

	virtual void prepare() override {
		this->nodes = {
			_AND_
				TOKEN(COLON),
				TOKEN(NEWLINE),
				MAKE_INDENTED(CodeBlockNode, n_indent + 1)
			__
		};
	}
};

class ExpressionStatementNode : public Node {
public:
	baseCtor(ExpressionStatementNode);

	virtual void prepare() override {
		this->nodes = {
			_AND_
				MAKE2(ExpressionNode),
				TOKEN(NEWLINE),
			__
		};
	}
};

class ElseStatementNode : public Node {
public:
	baseCtor(ElseStatementNode);

	virtual void prepare() override {
		this->nodes = {
			_AND_
				MAKE(IndentNode)(n_indent),
				TOKEN(ELSE),
				MAKE2(ColonIndentCodeBlockNode),
			__
		};
	}
};

class IfStatementNode : public Node {
public:
	baseCtor(IfStatementNode);

	virtual void prepare() override {
		this->nodes = {
			_AND_
				TOKEN(IF),
				MAKE2(ExpressionNode),
				MAKE2(ColonIndentCodeBlockNode),
				_OPT_
					MAKE2(ElseStatementNode)
				___
			__
		};
	}
};

class ForStatementNode : public Node {
public:
	baseCtor(ForStatementNode);

	virtual void prepare() override {
		this->nodes = {
			_AND_
				TOKEN(FOR),
				_COMMA_PLUS_
					_OR_
						_AND_
							MAKE2(TypenameNode),
							TOKEN(WORD),
						__,
						TOKEN(WORD),
					__
				___,
				TOKEN(IN),
				MAKE2(ExpressionNode),
				_OPT_ _AND_
					TOKEN(IF),
					MAKE2(ExpressionNode),
				____,
				_OPT_ _AND_
					TOKEN(WHILE),
					MAKE2(ExpressionNode),
				____,
				MAKE2(ColonIndentCodeBlockNode),
			__
		};
	}
};

class IForStatementNode : public Node {
public:
	baseCtor(IForStatementNode);

	virtual void prepare() override {
		this->nodes = {
			_AND_
				TOKEN(IFOR),
				TOKEN(WORD),
				TOKEN(COMMA),
				_OR_
					_AND_
						MAKE2(TypenameNode),
						TOKEN(WORD),
					__,
					TOKEN(WORD),
				__,
				TOKEN(IN),
				MAKE2(ExpressionNode),
				_OPT_ _AND_
					TOKEN(IF),
					MAKE2(ExpressionNode),
				____,
				_OPT_ _AND_
					TOKEN(WHILE),
					MAKE2(ExpressionNode),
				____,
				MAKE2(ColonIndentCodeBlockNode),
			__
		};
	}
};

class WhileStatementNode : public Node {
public:
	baseCtor(WhileStatementNode);

	virtual void prepare() override {
		this->nodes = {
			_AND_
				TOKEN(WHILE),
				MAKE2(ExpressionNode),
				MAKE2(ColonIndentCodeBlockNode),
			__
		};
	}
};

class ReturnStatementNode : public Node {
public:
	baseCtor(ReturnStatementNode);

	virtual void prepare() override {
		this->nodes = {
			_AND_
				TOKEN(RETURN),
				_COMMA_STAR_
					MAKE2(ExpressionNode)
				___,
				_OPT_ _AND_
					TOKEN(IF),
					MAKE2(ExpressionNode),
					_OPT_ _AND_
						TOKEN(ELSE),
						MAKE2(ExpressionNode),
					____,
				____,
				TOKEN(NEWLINE),
			__
		};
	}
};

class BreakStatementNode : public Node {
public:
	baseCtor(BreakStatementNode);

	virtual void prepare() override {
		this->nodes = {
			_AND_
				TOKEN(BREAK),
				_OPT_ _AND_
					TOKEN(IF),
					MAKE2(ExpressionNode),
				____,
				TOKEN(NEWLINE),
			__
		};
	}
};

class StatementNode : public Node {
public:
	baseCtor(StatementNode);

	virtual void prepare() override {
		this->nodes = {
			_AND_
				MAKE(IndentNode)(n_indent),
				_OR_
					MAKE2(ExpressionStatementNode),
					MAKE2(IfStatementNode),
					MAKE2(ForStatementNode),
					MAKE2(IForStatementNode),
					MAKE2(WhileStatementNode),
					MAKE2(ReturnStatementNode),
					MAKE2(BreakStatementNode),
				__
			__
		};
	}
};
