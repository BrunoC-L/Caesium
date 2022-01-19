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
				TOKEN(NS),
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
			_COMMA_STAR_
				MAKE(TypenameNode)()
			___
		};
	}

	virtual void accept(NodeVisitor* v) override {
		v->visit(this);
	}
};

class TypenameListNodeEndingWithRShift : public Node {
public:
	baseCtor(TypenameListNodeEndingWithRShift);

	virtual void build() override {
		this->nodes = {
			_AND_
				_STAR_ _AND_
					MAKE(TypenameNode)(),
					TOKEN(COMMA),
				____,
				TOKEN(WORD),
				TOKEN(LT),
				MAKE(TypenameListNode)(),
				TOKEN(RSHIFT),
				_OPT_
					TOKEN(COMMA)
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
				TOKEN(LT),
				_OR_
					_AND_
						MAKE(TypenameListNode)(),
						TOKEN(GT),
					__,
					MAKE(TypenameListNodeEndingWithRShift)(),
				__
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
				TOKEN(PARENOPEN),
				MAKE(TypenameListNode)(),
				TOKEN(PARENCLOSE),
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
				TOKEN(ASTERISK),
				TOKEN(AMPERSAND),
			__
		};
	}

	virtual void accept(NodeVisitor* v) override {
		v->visit(this);
	}
};
