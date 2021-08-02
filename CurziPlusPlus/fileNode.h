#pragma once
#include "classNode.h"

class FileNode : public Node {
public:
	baseCtor(FileNode);

	virtual void build() override {
		nodes = {
			_AND_
				MAKE(ClassNode)(),
				MAKE(TokenNode)(END)
			__
		};
	}
};
