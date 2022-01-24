#pragma once
#include "node.h"
#include "grammarizer.h"
#include "tokenNode.h"
#include "typenameNode.h"
#include "untilTokenNode.h"

class MacroNode : public Node {
public:
	baseCtor(MacroNode);

	virtual void build() override {
		this->nodes = {
			_AND_
				TOKEN(DEFINE),
				WORD_TOKEN("word"),
				MAKE(UntilTokenNode)(NEWLINE),
			__
		};
	}

	virtual void accept(NodeVisitor* v) override {
		v->visit(this);
	}
};
