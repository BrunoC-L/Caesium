#pragma once
#include "node.h"
#include "grammarizer.h"
#include "andorNode.h"
#include "tokenNode.h"
#include "macros.h"

class TypenameNode : public Node {
public:
	baseCtor(TypenameNode);
	virtual void build() override;
};

class NSTypenameNode : public Node {
public:
	baseCtor(NSTypenameNode);

	virtual void build() override {
		nodes = {
			_AND_
				MAKE(TokenNode)(NS),
				MAKE(TypenameNode)()
			__
		};
	}
};

class TypenameListNode : public Node {
public:
	baseCtor(TypenameListNode);

	virtual void build() override {
		nodes = {
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
};

class TemplateTypenameNode : public Node {
public:
	baseCtor(TemplateTypenameNode);

	virtual void build() override {
		nodes = {
			_AND_
				MAKE(TokenNode)(LT),
				MAKE(TypenameListNode)(),
				MAKE(TokenNode)(GT),
			__
		};
	}
};

class ParenthesisTypenameNode : public Node {
public:
	baseCtor(ParenthesisTypenameNode);

	virtual void build() override {
		nodes = {
			_AND_
				MAKE(TokenNode)(PARENOPEN),
				MAKE(TypenameListNode)(),
				MAKE(TokenNode)(PARENCLOSE),
			__
		};
	}
};

class PointerTypenameNode : public Node {
public:
	baseCtor(PointerTypenameNode);

	virtual void build() override {
		nodes = {
			_OR_
				MAKE(TokenNode)(ASTERISK),
				MAKE(TokenNode)(AMPERSAND),
			__
		};
	}
};
