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
				MAKE_NAMED(TypenameNode, "NSTypename")
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
			_COMMA_STAR_("types")
				MAKE_NAMED(TypenameNode, "typename")
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
				_STAR_("typenames") _AND_
					MAKE_NAMED(TypenameNode, "typename"),
					TOKEN(COMMA),
				____,
				std::make_shared<WordTokenNode>("word"),
				TOKEN(LT),
				MAKE_NAMED(TypenameListNode, "list"),
				TOKEN(RSHIFT),
				_OPT_("opt_trailing_comma")
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
						MAKE_NAMED(TypenameListNode, "TypenameList"),
						TOKEN(GT),
					__,
					MAKE_NAMED(TypenameListNodeEndingWithRShift, "TypenameListNodeEndingWithRShift"),
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
				MAKE_NAMED(TypenameListNode, "TypenameList"),
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
