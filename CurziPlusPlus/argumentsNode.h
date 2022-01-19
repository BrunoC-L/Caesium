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
			_COMMA_STAR_
				_AND_
					MAKE(TypenameNode)(),
					TOKEN(WORD),
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
			_COMMA_STAR_
				MAKE(ExpressionNode)()
			___
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
				TOKEN(PARENOPEN),
				MAKE(InnerArgumentsNode)(),
				TOKEN(PARENCLOSE),
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
				TOKEN(BRACKETOPEN),
				MAKE(InnerArgumentsNode)(),
				TOKEN(BRACKETCLOSE),
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
				TOKEN(BRACEOPEN),
				MAKE(InnerArgumentsNode)(),
				TOKEN(BRACECLOSE),
			__
		};
	}

	virtual void accept(NodeVisitor* v) override {
		v->visit(this);
	}
};
