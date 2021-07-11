#pragma once
#include "node.h"
#include "grammarizer.h"
#include "usingNode.h"
#include "pppQualifierNode.h"
#include "macroNode.h"
#include "classMember.h"
#include "constructor.h"

class ClassElementNode : public Node {
public:
	baseCtor(ClassElementNode);

	virtual void build() override {
		nodes = {
			_OR_
				_AND_
					MAKE(PPPQualifierNode)(),
					MAKE(TokenNode)(COLON)
				__,
				MAKE(UsingNode)(),
				MAKE(MacroNode)(),
				MAKE(ClassMemberNode)(),
				MAKE(ConstructorNode)(),
			__,
		};
	}
};


class ClassElementsNode : public Node {
public:
	baseCtor(ClassElementsNode);
	virtual bool build(Grammarizer* g) override {
		return
		_OR_
			_AND_
				MAKE(ClassElementNode)(),
				MAKE(ClassElementsNode)(),
			})),
			MAKE(EmptyNode)(),
		}))
		->build(g);
	}
};
