#pragma once
#include "node.h"
#include "grammarizer.h"
#include "pppQualifierNode.h"
#include "kNode.h"

class ClassMemberQualifiers : public Node {
public:
	baseCtor(ClassMemberQualifiers);

	virtual void build() override {
		this->nodes = {
			_AND_
				_OPT_
					MAKE_NAMED(PPPQualifierNode, "PPPQualifier")
				___,
				_OPT_
					TOKEN(STATIC)
				___,
			__
		};
	}

	virtual void accept(NodeVisitor* v) override {
		v->visit(this);
	}
};
