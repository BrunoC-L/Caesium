#pragma once
#include "andorNode.h"
#include "tokenNode.h"
#include "pppQualifierNode.h"
#include "typenameNode.h"

class SingleInheritanceNode : public Node {
public:
	baseCtor(SingleInheritanceNode);
	virtual bool build(Grammarizer* g) {
		return
		_AND_(vNode({
			MAKE(PPPQualifierNode)(),
			MAKE(TypenameNode)()
		}))
#ifdef DEBUG
			->debugbuild(g);
#else
			->build(g);
#endif // DEBUG
	}
};
