#pragma once
#include "classNode.h"
#include "functionNode.h"
#include <fstream>
#include "node_structs.h"

class ImportNode : public Node {
public:
	baseCtor(ImportNode);

	virtual void prepare() override {
		this->nodes = {
			_AND_
				TOKEN(IMPORT),
				TOKEN(WORD),
				TOKEN(FROM),
				TOKEN(WORD),
				TOKEN(NEWLINE)
			__
		};
	}
};
