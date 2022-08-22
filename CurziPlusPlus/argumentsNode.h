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
			_COMMA_STAR_("arguments")
				_AND_
					MAKE_NAMED(TypenameNode, "Typename"),
					WORD_TOKEN("word"),
			____
		};
	}
};

class InnerArgumentsNode : public Node {
public:
	baseCtor(InnerArgumentsNode);

	virtual void build() override {
		this->nodes = {
			_COMMA_STAR_("arguments")
				MAKE_NAMED(ExpressionNode, "expr")
			___
		};
	}
};

class ParenArgumentsNode : public Node {
public:
	baseCtor(ParenArgumentsNode);

	virtual void build() override {
		this->nodes = {
			_AND_
				TOKEN(PARENOPEN),
				MAKE_NAMED(InnerArgumentsNode, "args"),
				TOKEN(PARENCLOSE),
			__
		};
	}
};

class BracketArgumentsNode : public Node {
public:
	baseCtor(BracketArgumentsNode);

	virtual void build() override {
		this->nodes = {
			_AND_
				TOKEN(BRACKETOPEN),
				MAKE_NAMED(InnerArgumentsNode, "args"),
				TOKEN(BRACKETCLOSE),
			__
		};
	}
};

class BraceArgumentsNode : public Node {
public:
	baseCtor(BraceArgumentsNode);

	virtual void build() override {
		this->nodes = {
			_AND_
				TOKEN(BRACEOPEN),
				MAKE_NAMED(InnerArgumentsNode, "args"),
				TOKEN(BRACECLOSE),
			__
		};
	}
};
