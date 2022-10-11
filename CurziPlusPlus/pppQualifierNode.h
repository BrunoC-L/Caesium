#pragma once
#include "tokenNode.h"
#include "andorNode.h"
#include "macros.h"

class PPPQualifierNode : public Node {
public:
	baseCtor(PPPQualifierNode);

	virtual void prepare() override {
		this->nodes = {
			_OR_
				TOKEN(PRIVATE),
				TOKEN(PUBLIC),
				TOKEN(PROTECTED),
			__
		};
	}
};
