#pragma once
#include "node.h"
#include "grammarizer.h"
#include "andorNode.h"
#include "tokenNode.h"
#include "macros.h"
#include "kNode.h"

class TypenameNode : public Node {
public:
	baseCtor(TypenameNode);
	virtual void build() override;

	virtual void accept(NodeVisitor* v) override {
		v->visit(this);
	}
};

class NSTypenameNode : public Node {
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

	virtual void accept(NodeVisitor* v) override {
		v->visit(this);
	}
};

class TypenameListNode : public Node {
public:
	baseCtor(TypenameListNode);

	virtual void build() override {
		this->nodes = {
			_AND_
				MAKE(TypenameNode)(),
				_STAR_ _AND_
					MAKE(TokenNode)(COMMA),
					MAKE(TypenameNode)()
				____,
				_OPT_
					MAKE(TokenNode)(COMMA)
				___
			__
		};
	}

	virtual void accept(NodeVisitor* v) override {
		v->visit(this);
	}
};

class TemplateTypenameNode : public Node {
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

	virtual void accept(NodeVisitor* v) override {
		v->visit(this);
	}
};

class ParenthesisTypenameNode : public Node {
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

	virtual void accept(NodeVisitor* v) override {
		v->visit(this);
	}
};

class PointerTypenameNode : public Node {
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

	virtual void accept(NodeVisitor* v) override {
		v->visit(this);
	}
};
