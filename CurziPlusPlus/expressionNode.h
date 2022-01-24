#pragma once
#include "node.h"
#include "macros.h"
#include "nodeVisitor.h"

#define nodeclass(N) class N : public Node {\
public:\
	baseCtor(N);\
	virtual void build() override;\
	virtual void accept(NodeVisitor* v) override {\
		v->visit(this);\
	}\
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
