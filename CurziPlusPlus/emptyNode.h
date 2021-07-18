#pragma once
#include "node.h"
#include "grammarizer.h"
#include "macros.h"

class EmptyNode : public Node {
public:
	baseCtor(EmptyNode);
	virtual void build() override {
		nodes = { _AND_})) };
	}
};
