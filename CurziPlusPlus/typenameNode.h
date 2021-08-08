#pragma once
#include "node.h"
#include "grammarizer.h"
#include "andorNode.h"
#include "tokenNode.h"
#include "macros.h"

template <typename T>
class TypenameNode : public Node<T> {
public:
	baseCtor(TypenameNode);
	virtual void build() override;

	virtual T accept(NodeVisitor<T>* v) override {
		return v->visit(this);
	}
};

template <typename T>
class NSTypenameNode : public Node<T> {
public:
	baseCtor(NSTypenameNode);

	virtual void build() override {
		this->nodes = {
			_AND_
				MAKE(TokenNode)(NS),
				MAKE(TypenameNode)()
			__
		};
	}

	virtual T accept(NodeVisitor<T>* v) override {
		return v->visit(this);
	}
};

template <typename T>
class TypenameListNode : public Node<T> {
public:
	baseCtor(TypenameListNode);

	virtual void build() override {
		this->nodes = {
			_OR_
				_AND_
					_AND_
						MAKE(TypenameNode)(),
						MAKE(TokenNode)(COMMA),
						MAKE(TypenameListNode)(),
					__
				__,
				MAKE(TypenameNode)(),
			__
		};
	}

	virtual T accept(NodeVisitor<T>* v) override {
		return v->visit(this);
	}
};

template <typename T>
class TemplateTypenameNode : public Node<T> {
public:
	baseCtor(TemplateTypenameNode);

	virtual void build() override {
		this->nodes = {
			_AND_
				MAKE(TokenNode)(LT),
				MAKE(TypenameListNode)(),
				MAKE(TokenNode)(GT),
			__
		};
	}

	virtual T accept(NodeVisitor<T>* v) override {
		return v->visit(this);
	}
};

template <typename T>
class ParenthesisTypenameNode : public Node<T> {
public:
	baseCtor(ParenthesisTypenameNode);

	virtual void build() override {
		this->nodes = {
			_AND_
				MAKE(TokenNode)(PARENOPEN),
				MAKE(TypenameListNode)(),
				MAKE(TokenNode)(PARENCLOSE),
			__
		};
	}

	virtual T accept(NodeVisitor<T>* v) override {
		return v->visit(this);
	}
};

template <typename T>
class PointerTypenameNode : public Node<T> {
public:
	baseCtor(PointerTypenameNode);

	virtual void build() override {
		this->nodes = {
			_OR_
				MAKE(TokenNode)(ASTERISK),
				MAKE(TokenNode)(AMPERSAND),
			__
		};
	}

	virtual T accept(NodeVisitor<T>* v) override {
		return v->visit(this);
	}
};
