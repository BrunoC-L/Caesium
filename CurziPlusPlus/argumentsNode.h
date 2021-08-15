#pragma once
#include "node.h"
#include "grammarizer.h"
#include "tokenNode.h"
#include "typenameNode.h"
#include "expressionNode.h"
#include "kNode.h"
#include "macros.h"

class ArgumentsSignatureNode : public Node {
public:
	baseCtor(ArgumentsSignatureNode);

	virtual void build() override {
		this->nodes = {
			_OPT_ _AND_
				MAKE(TypenameNode)(),
				MAKE(TokenNode)(WORD),
				_OPT_ _AND_
						MAKE(TokenNode)(COMMA),
						MAKE(ArgumentsSignatureNode)(),
				____
			____
		};
	}

	virtual void accept(NodeVisitor* v) override {
		v->visit(this);
	}
};

class InnerArgumentsNode : public Node {
public:
	baseCtor(InnerArgumentsNode);

	virtual void build() override {
		this->nodes = {
			_OPT_ _AND_
				MAKE(ExpressionNode)(),
				_OPT_ _AND_
					_PLUS_ _AND_
						MAKE(TokenNode)(COMMA),
						MAKE(ExpressionNode)(),
					____,
					_OPT_
						MAKE(TokenNode)(COMMA)
					___
				____
			____
		};
	}

	virtual void accept(NodeVisitor* v) override {
		v->visit(this);
	}
};

class ParenArgumentsNode : public Node {
public:
	baseCtor(ParenArgumentsNode);

	virtual void build() override {
		this->nodes = {
			_AND_
				MAKE(TokenNode)(PARENOPEN),
				MAKE(InnerArgumentsNode)(),
				MAKE(TokenNode)(PARENCLOSE),
			__
		};
	}

	virtual void accept(NodeVisitor* v) override {
		v->visit(this);
	}
};

class BracketArgumentsNode : public Node {
public:
	baseCtor(BracketArgumentsNode);

	virtual void build() override {
		this->nodes = {
			_AND_
				MAKE(TokenNode)(BRACKETOPEN),
				MAKE(InnerArgumentsNode)(),
				MAKE(TokenNode)(BRACKETCLOSE),
			__
		};
	}

	virtual void accept(NodeVisitor* v) override {
		v->visit(this);
	}
};

class BraceArgumentsNode : public Node {
public:
	baseCtor(BraceArgumentsNode);

	virtual void build() override {
		this->nodes = {
			_AND_
				MAKE(TokenNode)(BRACEOPEN),
				MAKE(InnerArgumentsNode)(),
				MAKE(TokenNode)(BRACECLOSE),
			__
		};
	}

	virtual void accept(NodeVisitor* v) override {
		v->visit(this);
	}
};
