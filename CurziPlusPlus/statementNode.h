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

	virtual void build() override {
		this->nodes = {
			_STAR_("statements")
				MAKE_NAMED(StatementNode, "Statement")
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
				MAKE_NAMED_INDENTED(CodeBlockNode, "CodeBlock", n_indent + 1)
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
				MAKE_NAMED(ExpressionNode, "Expression"),
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
				MAKE_NAMED(ColonIndentCodeBlockNode, "ColonIndentCodeBlock"),
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
				MAKE_NAMED(ExpressionNode, "Expression"),
				MAKE_NAMED(ColonIndentCodeBlockNode, "ColonIndentCodeBlock"),
				_OPT_("opt_else")
					MAKE_NAMED(ElseStatementNode, "ElseStatement")
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
				_COMMA_PLUS_("iterators")
					_OR_
						_AND_
							MAKE_NAMED(TypenameNode, "Typename"),
							WORD_TOKEN("iteratorName"),
						__,
						WORD_TOKEN("iteratorName"),
					__
				___,
				TOKEN(IN),
				MAKE_NAMED(ExpressionNode, "Expression"),
				_OPT_("opt_if") _AND_
					TOKEN(IF),
					MAKE_NAMED(ExpressionNode, "Expression"),
				____,
				_OPT_("opt_while") _AND_
					TOKEN(WHILE),
					MAKE_NAMED(ExpressionNode, "Expression"),
				____,
				MAKE_NAMED(ColonIndentCodeBlockNode, "ColonIndentCodeBlock"),
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
				WORD_TOKEN("indexIteratorName"),
				TOKEN(COMMA),
				_OR_
					_AND_
						MAKE_NAMED(TypenameNode, "Typename"),
						WORD_TOKEN("iteratorName"),
					__,
					WORD_TOKEN("iteratorName"),
				__,
				TOKEN(IN),
				MAKE_NAMED(ExpressionNode, "Expression"),
				_OPT_("opt_if") _AND_
					TOKEN(IF),
					MAKE_NAMED(ExpressionNode, "Expression"),
				____,
				_OPT_("opt_while") _AND_
					TOKEN(WHILE),
					MAKE_NAMED(ExpressionNode, "Expression"),
				____,
				MAKE_NAMED(ColonIndentCodeBlockNode, "ColonIndentCodeBlock"),
			__
		};
	}

	virtual void accept(NodeVisitor* v) override {
		v->visit(this);
	}
};

class WhileStatementNode : public Node {
public:
	baseCtor(WhileStatementNode);

	virtual void build() override {
		this->nodes = {
			_AND_
				TOKEN(WHILE),
				MAKE_NAMED(ExpressionNode, "Expression"),
				MAKE_NAMED(ColonIndentCodeBlockNode, "ColonIndentCodeBlock"),
			__
		};
	}

	virtual void accept(NodeVisitor* v) override {
		v->visit(this);
	}
};

class ReturnStatementNode : public Node {
public:
	baseCtor(ReturnStatementNode);

	virtual void build() override {
		this->nodes = {
			_AND_
				TOKEN(RETURN),
				_COMMA_STAR_("values")
					MAKE_NAMED(ExpressionNode, "Expression")
				___,
				_OPT_("opt_if") _AND_
					TOKEN(IF),
					MAKE_NAMED(ExpressionNode, "Expression"),
					_OPT_("opt_else") _AND_
						TOKEN(ELSE),
						MAKE_NAMED(ExpressionNode, "Expression"),
					____,
				____,
				TOKEN(NEWLINE),
			__
		};
	}

	virtual void accept(NodeVisitor* v) override {
		v->visit(this);
	}
};

class BreakStatementNode : public Node {
public:
	baseCtor(BreakStatementNode);

	virtual void build() override {
		this->nodes = {
			_AND_
				TOKEN(BREAK),
				_OPT_("opt_if") _AND_
					TOKEN(IF),
					MAKE_NAMED(ExpressionNode, "Expression"),
				____,
				TOKEN(NEWLINE),
			__
		};
	}

	virtual void accept(NodeVisitor* v) override {
		v->visit(this);
	}
};

class StatementNode : public Node {
public:
	baseCtor(StatementNode);

	virtual void build() override {
		this->nodes = {
			_AND_
				MAKE(IndentNode)(n_indent),
				_OR_
					MAKE_NAMED(ExpressionStatementNode, "ExpressionStatement"),
					MAKE_NAMED(IfStatementNode, "IfStatement"),
					MAKE_NAMED(ForStatementNode, "ForStatement"),
					MAKE_NAMED(IForStatementNode, "IForStatement"),
					MAKE_NAMED(WhileStatementNode, "WhileStatement"),
					MAKE_NAMED(ReturnStatementNode, "ReturnStatement"),
					MAKE_NAMED(BreakStatementNode, "BreakStatement"),
				__
			__
		};
	}

	virtual void accept(NodeVisitor* v) override {
		v->visit(this);
	}
};
