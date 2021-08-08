#pragma once
#include "node.h"
#include "grammarizer.h"
#include "tokenNode.h"
#include "typenameNode.h"
#include "expressionNode.h"
#include "kNode.h"
#include "macros.h"

template <typename T>
class ArgumentsSignatureNode : public Node<T> {
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

	virtual T accept(NodeVisitor<T>* v) override {
		return v->visit(this);
	}
};

template <typename T>
class InnerArgumentsNode : public Node<T> {
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
						MAKE(TokenNode)(COMMA) // JAVA & C++ don't even support this lol...
					___
				____
			____
		};
	}

	virtual T accept(NodeVisitor<T>* v) override {
		return v->visit(this);
	}
};

template <typename T>
class ParenArgumentsNode : public Node<T> {
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

	virtual T accept(NodeVisitor<T>* v) override {
		return v->visit(this);
	}
};

template <typename T>
class BracketArgumentsNode : public Node<T> {
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

	virtual T accept(NodeVisitor<T>* v) override {
		return v->visit(this);
	}
};

template <typename T>
class BraceArgumentsNode : public Node<T> {
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

	virtual T accept(NodeVisitor<T>* v) override {
		return v->visit(this);
	}
};
