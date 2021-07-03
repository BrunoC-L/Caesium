#pragma once
#include "node.h"
#include "grammarizer.h"

class EmptyNode : public Node {
public:
	baseCtor(EmptyNode);
	virtual bool build(Grammarizer* g) override {
		return true;
	}
};
