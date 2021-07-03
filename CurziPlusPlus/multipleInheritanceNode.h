#pragma once
#include "singleInheritanceNode.h"

class MultipleInheritanceNode : public Node {
public:
	baseCtor(MultipleInheritanceNode);
	virtual bool build(Grammarizer* g) {
		return
		_OR_(vNode({
			_AND_(
				vNode({
					MAKE(SingleInheritanceNode)(),
					MAKE(TokenNode)(COMMA),
					MAKE(MultipleInheritanceNode)()
				})
			),
			MAKE(SingleInheritanceNode)(),
		}))
#ifdef DEBUG
			->debugbuild(g);
#else
			->build(g);
#endif // DEBUG
	}
};
