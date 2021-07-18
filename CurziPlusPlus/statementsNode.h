#pragma once
#include "node.h"
#include "grammarizer.h"
#include "tokenNode.h"
#include "typenameNode.h"
#include "emptyNode.h"
#include "statementNode.h"
#include "kNode.h"

class StatementsNode : public Node {
public:
	baseCtor(StatementsNode);

	virtual void build() override {
		nodes = {
		_OPT_
			_AND_
				MAKE(StatementNode)(),
				MAKE(StatementsNode)(),
			__
		___
		};
	}
};
