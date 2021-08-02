#pragma once
#include "node.h"
#include "macros.h"

class CodeBlockNode : public Node {
public:
	baseCtor(CodeBlockNode);

	virtual void build() override;
};
