#pragma once
#include "node.h"
#include "grammarizer.h"
#include "emptyNode.h"
#include "pppQualifierNode.h"

class ClassMemberQualifiers : public Node {
public:
	baseCtor(ClassMemberQualifiers);

	virtual void build() override {
		nodes = {
		_AND_
			_OR_
				MAKE(PPPQualifierNode)(),
				MAKE(EmptyNode)(),
			})),
			_OR_
				MAKE(TokenNode)(STATIC),
				MAKE(EmptyNode)(),
			})),
		}))
		};
	}
};
