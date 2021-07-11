#pragma once
#include "node.h"
#include "grammarizer.h"
#include "andorNode.h"
#include "tokenNode.h"
#include "emptyNode.h"

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
		}))
		};
	}
};

class TemplateListNode : public Node {
public:
	baseCtor(TemplateListNode);

	virtual void build() override {
		nodes = {
		_OR_
			_AND_
				_AND_
					MAKE(TypenameNode)(),
					MAKE(TokenNode)(COMMA),
					MAKE(TemplateListNode)(),
				})),
			})),
			MAKE(TypenameNode)(),
		}))
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
			MAKE(TemplateListNode)(),
			MAKE(TokenNode)(GT),
		}))
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
		}))
		};
	}
};
