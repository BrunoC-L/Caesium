#pragma once
#include "node.h"
#include "macros.h"

#define nodeclass(T) class T : public Node {\
public:\
	baseCtor(T);\
	virtual void build() override;\
};

class CollapsableJSONNodeOutput : public Node {
	virtual JSON toJSON() override {
		JSON res = nodes[0]->toJSON();
		try {
			if (res.getChildren()[1].asString() == "[]")
				return res.getChildren()[0];
		}
		catch (...) {

		}
		return res;
	}
};

#define collapsablejsonnodeoutput(T) class T : public CollapsableJSONNodeOutput {\
public:\
	baseCtor(T);\
	virtual void build() override;\
};

nodeclass(ExpressionNode)
collapsablejsonnodeoutput(AssignmentExpressionNode)
collapsablejsonnodeoutput(ConditionalExpressionNode)
collapsablejsonnodeoutput(OrExpressionNode)
collapsablejsonnodeoutput(AndExpressionNode)
collapsablejsonnodeoutput(BitOrExpressionNode)
collapsablejsonnodeoutput(BitXorExpressionNode)
collapsablejsonnodeoutput(BitAndExpressionNode)
collapsablejsonnodeoutput(EqualityExpressionNode)
collapsablejsonnodeoutput(CompareExpressionNode)
collapsablejsonnodeoutput(BitShiftExpressionNode)
collapsablejsonnodeoutput(AdditiveExpressionNode)
collapsablejsonnodeoutput(MultiplicativeExpressionNode)
nodeclass(UnaryExpressionNode)
collapsablejsonnodeoutput(PostfixExpressionNode)
nodeclass(ParenExpressionNode)
