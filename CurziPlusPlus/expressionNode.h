#pragma once
#include "node.h"
#include "macros.h"

#define nodeclass(N) class N : public Node {\
public:\
	baseCtor(N);\
	virtual void build() override;\
};

nodeclass(ExpressionNode)
nodeclass(AssignmentExpressionNode)
nodeclass(ConditionalExpressionNode)
nodeclass(OrExpressionNode)
nodeclass(AndExpressionNode)
nodeclass(EqualityExpressionNode)
nodeclass(CompareExpressionNode)
nodeclass(AdditiveExpressionNode)
nodeclass(MultiplicativeExpressionNode)
nodeclass(UnaryExpressionNode)
nodeclass(PostfixExpressionNode)
nodeclass(ParenExpressionNode)
