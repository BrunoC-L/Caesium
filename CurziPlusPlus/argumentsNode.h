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
					MAKE2(TypenameNode),
					TOKEN(WORD),
			____
		};
	}
};

class InnerArgumentsNode : public Node {
public:
	baseCtor(InnerArgumentsNode);

	virtual void build() override {
		this->nodes = {
			_COMMA_STAR_
				MAKE2(ExpressionNode)
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
				MAKE2(InnerArgumentsNode),
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
				MAKE2(InnerArgumentsNode),
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
				MAKE2(InnerArgumentsNode),
				TOKEN(BRACECLOSE),
			__
		};
	}
};
