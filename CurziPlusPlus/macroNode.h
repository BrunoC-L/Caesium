#pragma once
#include "node.h"
#include "grammarizer.h"
#include "tokenNode.h"
#include "typenameNode.h"
#include "untilTokenNode.h"

template <typename T>
class MacroNode : public Node<T> {
public:
	baseCtor(MacroNode);

	virtual void build() override {
		this->nodes = {
			_AND_
				MAKE(TokenNode)(DEFINE),
				MAKE(TokenNode)(WORD),
				MAKE(UntilTokenNode)(NEWLINE),
			__
		};
	}

	virtual T accept(NodeVisitor<T>* v) override {
		return v->visit(this);
	}
};
