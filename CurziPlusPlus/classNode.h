#pragma once
#include "classInheritanceNode.h"
#include "classElementsNode.h"

class ClassNode : public Node {
public:
	baseCtor(ClassNode);
	virtual bool build(Grammarizer* g) override {
		return
		_AND_(vNode({
				MAKE(TokenNode)(CLASS),
				MAKE(TokenNode)(WORD),
				_OR_(
					vNode({
						MAKE(ClassInheritanceNode)(),
						MAKE(EmptyNode)()
					})
				),
				MAKE(TokenNode)(BRACEOPEN),
			//MAKE(ClassElementsNode)(),
		}))
#ifdef DEBUG
			->debugbuild(g);
#else
			->build(g);
#endif // DEBUG
	}
};
