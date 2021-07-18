#pragma once
#include "node.h"
#include "grammarizer.h"
#include "emptyNode.h"
#include "pppQualifierNode.h"
#include "kNode.h"

class ClassMemberQualifiers : public Node {
public:
	baseCtor(ClassMemberQualifiers);

	virtual void build() override {
		nodes = {
			_AND_
				_OPT_
					MAKE(PPPQualifierNode)()
				___,
				_OPT_
					MAKE(TokenNode)(STATIC)
				___,
			__
		};
	}
};
