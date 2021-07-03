#pragma once
#include "node.h"
#include "grammarizer.h"
#include "tokenNode.h"
#include "typenameNode.h"
#include "emptyNode.h"

class CodeBlockNode : public Node {
public:
	baseCtor(CodeBlockNode);

	virtual bool build(Grammarizer* g) override {
		return false;
	}
};
