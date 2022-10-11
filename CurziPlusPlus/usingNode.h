#pragma once
#include "node.h"
#include "grammarizer.h"
#include "tokenNode.h"
#include "typenameNode.h"

class UsingNode : public Node {
public:
	baseCtor(UsingNode);

	virtual void prepare() override {
		this->nodes = {
			_AND_
				TOKEN(USING),
				TOKEN(WORD),
				TOKEN(EQUAL),
				MAKE2(TypenameNode),
			__
		};
	}
};
