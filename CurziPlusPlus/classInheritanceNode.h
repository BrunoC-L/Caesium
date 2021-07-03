#pragma once
#include "multipleInheritanceNode.h"
#include "emptyNode.h"

class ClassInheritanceNode : public Node {
public:
	baseCtor(ClassInheritanceNode);
	virtual bool build(Grammarizer* g) {
		return
		_AND_(vNode({
			MAKE(TokenNode)(COLON),
			MAKE(MultipleInheritanceNode)(),
		}))
#ifdef DEBUG
		->debugbuild(g);
#else
		->build(g);
#endif // DEBUG
	}
};
