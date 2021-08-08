#pragma once
#include "node.h"
#include "macros.h"
#include "nodeVisitor.h"

#define nodeclass(N) template <typename T>\
class N : public Node<T> {\
public:\
	baseCtor(N);\
	virtual void build() override;\
	virtual T accept(NodeVisitor<T>* v) override {\
		return v->visit(this);\
	}\
};

nodeclass(ExpressionNode)
nodeclass(AssignmentExpressionNode)
nodeclass(ConditionalExpressionNode)
nodeclass(OrExpressionNode)
nodeclass(AndExpressionNode)
nodeclass(BitOrExpressionNode)
nodeclass(BitXorExpressionNode)
nodeclass(BitAndExpressionNode)
nodeclass(EqualityExpressionNode)
nodeclass(CompareExpressionNode)
nodeclass(BitShiftExpressionNode)
nodeclass(AdditiveExpressionNode)
nodeclass(MultiplicativeExpressionNode)
nodeclass(UnaryExpressionNode)
nodeclass(PostfixExpressionNode)
nodeclass(ParenExpressionNode)
