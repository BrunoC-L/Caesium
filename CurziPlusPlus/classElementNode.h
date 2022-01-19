#pragma once
#include "node.h"
#include "grammarizer.h"
#include "usingNode.h"
#include "pppQualifierNode.h"
#include "macroNode.h"
#include "classMember.h"
#include "constructor.h"

class ClassElementNode : public Node {
public:
	baseCtor(ClassElementNode);

	virtual void build() override {
		this->nodes = {
			_OR_
				MAKE(UsingNode)(n_indent),
				MAKE(MacroNode)(n_indent),
				MAKE(ClassMemberNode)(n_indent),
				MAKE(ConstructorNode)(n_indent),
			__,
		};
	}

	virtual void accept(NodeVisitor* v) override {
		v->visit(this);
	}
};
