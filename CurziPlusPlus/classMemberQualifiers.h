#pragma once
#include "node.h"
#include "grammarizer.h"
#include "pppQualifierNode.h"
#include "kNode.h"

template <typename T>
class ClassMemberQualifiers : public Node<T> {
public:
	baseCtor(ClassMemberQualifiers);

	virtual void build() override {
		this->nodes = {
			_AND_
				_OPT_
					MAKE(PPPQualifierNode)()
				___,
				_OPT_
					MAKE(TokenNode)(STATIC)
				___,
			__
		};
	}

	virtual T accept(NodeVisitor<T>* v) override {
		return v->visit(this);
	}
};
