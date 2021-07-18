#pragma once
#include "node.h"
#include "grammarizer.h"
#include "tokenNode.h"
#include "typenameNode.h"
#include "emptyNode.h"
#include "expressionNode.h"
#include "kNode.h"
#include "macros.h"

class ArgumentsSignatureNode : public Node {
public:
	baseCtor(ArgumentsSignatureNode);

	virtual void build() override {
		nodes = {
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
};

class InnerArgumentsNode : public Node {
public:
	baseCtor(InnerArgumentsNode);

	virtual void build() override {
		nodes = {
			_OPT_
				_AND_
					MAKE(ExpressionNode)(),
					_OPT_
						_AND_
							_PLUS_
								_AND_
									MAKE(TokenNode)(COMMA),
									MAKE(ExpressionNode)(),
								__
							___,
							_OPT_
								MAKE(TokenNode)(COMMA) // JAVA & C++ don't even support this lol...
							___
						__
					___
				__
			___
		};
	}
};

class ParenArgumentsNode : public Node {
public:
	baseCtor(ParenArgumentsNode);

	virtual void build() override {
		nodes = {
			_AND_
				MAKE(TokenNode)(PARENOPEN),
				MAKE(InnerArgumentsNode)(),
				MAKE(TokenNode)(PARENCLOSE),
			__
		};
	}
};

class BracketArgumentsNode : public Node {
public:
	baseCtor(BracketArgumentsNode);

	virtual void build() override {
		nodes = {
			_AND_
				MAKE(TokenNode)(BRACKETOPEN),
				MAKE(InnerArgumentsNode)(),
				MAKE(TokenNode)(BRACKETCLOSE),
			__
		};
	}
};

class BraceArgumentsNode : public Node {
public:
	baseCtor(BraceArgumentsNode);

	virtual void build() override {
		nodes = {
			_AND_
				MAKE(TokenNode)(BRACEOPEN),
				MAKE(InnerArgumentsNode)(),
				MAKE(TokenNode)(BRACECLOSE),
			__
		};
	}
};
