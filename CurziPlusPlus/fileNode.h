#pragma once
#include "classNode.h"

class FileNode : public Node {
public:
	baseCtor(FileNode);

	virtual void build() override {
		this->nodes = {
			_AND_
				MAKE(ClassNode)(),
				MAKE(TokenNode)(END)
			__
		};
	}

	virtual void accept(NodeVisitor* v) override {
		v->visit(this);
	}
};
