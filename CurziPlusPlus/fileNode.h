#pragma once
#include "classNode.h"
#include "functionNode.h"

class FileNode : public Node {
public:
	baseCtor(FileNode);

	virtual void build() override {
		this->nodes = {
			_AND_
				_STAR_ _OR_
					MAKE_NAMED(ClassNode, "Class"),
					MAKE_NAMED(FunctionNode, "Function"),
				____,
				TOKEN(END)
			__
		};
	}

	virtual void accept(NodeVisitor* v) override {
		v->visit(this);
	}
};
