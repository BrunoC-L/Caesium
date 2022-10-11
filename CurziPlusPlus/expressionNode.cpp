#include "expressionNode.h"
#include "kNode.h"
#include "grammarizer.h"
#include "tokenNode.h"
#include "typenameNode.h"
#include "statementNode.h"
#include "argumentsNode.h"
#include "macros.h"

void ExpressionNode::prepare() {
	this->nodes = {
		MAKE2(AssignmentExpressionNode),
	};
}

void AssignmentExpressionNode::prepare() {
	this->nodes = {
		_AND_
			MAKE2(ConditionalExpressionNode),
			_STAR_
				_AND_
					_OR_
						TOKEN(EQUAL),
						TOKEN(PLUSEQUAL),
						TOKEN(MINUSEQUAL),
						TOKEN(TIMESEQUAL),
						TOKEN(DIVEQUAL),
						TOKEN(MODEQUAL),
						TOKEN(ANDEQUAL),
						TOKEN(OREQUAL),
						TOKEN(XOREQUAL),
					__,
					MAKE2(ConditionalExpressionNode),
				__
			___
		__
	};
}

void ConditionalExpressionNode::prepare() {
	this->nodes = {
		_AND_
			MAKE2(OrExpressionNode),
			_OPT_ _AND_
				TOKEN(IF),
				MAKE2(OrExpressionNode),
				_OPT_ _AND_
					TOKEN(ELSE),
					MAKE2(OrExpressionNode),
				____,
			____
		__
	};
}

void OrExpressionNode::prepare() {
	this->nodes = {
		_AND_
			MAKE2(AndExpressionNode),
			_STAR_
				_AND_
					TOKEN(OR), // OR = 'or', OROR = '||'
					MAKE2(AndExpressionNode),
				__
			___
		__
	};
}

void AndExpressionNode::prepare() {
	this->nodes = {
		_AND_
			MAKE2(EqualityExpressionNode),
			_STAR_
				_AND_
					TOKEN(AND), // AND = 'and', ANDAND = '&&'
					MAKE2(EqualityExpressionNode),
				__
			___
		__
	};
}

void EqualityExpressionNode::prepare() {
	this->nodes = {
		_AND_
			MAKE2(CompareExpressionNode),
			_STAR_
				_AND_
					TOKEN(EQUALEQUAL),
					MAKE2(CompareExpressionNode),
				__
			___
		__
	};
}

void CompareExpressionNode::prepare() {
	this->nodes = {
		_AND_
			MAKE2(AdditiveExpressionNode),
			_STAR_
				_AND_
					_OR_
						TOKEN(LT),
						TOKEN(LTE),
						TOKEN(GT),
						TOKEN(GTE),
					__,
					MAKE2(AdditiveExpressionNode),
				__
			___
		__
	};
}

void AdditiveExpressionNode::prepare() {
	this->nodes = {
		_AND_
			MAKE2(MultiplicativeExpressionNode),
			_STAR_
				_AND_
					_OR_
						TOKEN(PLUS),
						TOKEN(DASH),
					__,
					MAKE2(MultiplicativeExpressionNode),
				__
			___
		__
	};
}

void MultiplicativeExpressionNode::prepare() {
	this->nodes = {
		_AND_
			MAKE2(UnaryExpressionNode),
			_STAR_
				_AND_
					_OR_
						TOKEN(ASTERISK),
						TOKEN(SLASH),
						TOKEN(PERCENT),
					__,
					MAKE2(UnaryExpressionNode),
				__
			___
		__
	};
}

void UnaryExpressionNode::prepare() {
	this->nodes = {
		_OR_
			_AND_
				_OR_ // has to be recursive because of the type cast operator taking the same shape as a ParenExpression
                     // so instead of _STAR_ _OR_ ... ____ we refer to unary expressions inside the _OR_
					TOKEN(NOT),
					TOKEN(PLUS),
					TOKEN(DASH),
					TOKEN(PLUSPLUS),
					TOKEN(MINUSMINUS),
					TOKEN(TILDE),
					TOKEN(ASTERISK),
					TOKEN(AMPERSAND),
					_AND_ // type cast operator
						TOKEN(PARENOPEN),
						MAKE2(TypenameNode),
						TOKEN(PARENCLOSE),
					__,
				__,
				MAKE2(UnaryExpressionNode),
			__,
			MAKE2(PostfixExpressionNode),
		__
	};
}

void PostfixExpressionNode::prepare() {
	this->nodes = {
		_AND_
			MAKE2(ParenExpressionNode),
			_STAR_ _OR_
				_AND_
					_OR_
						TOKEN(DOT),
						TOKEN(ARROW),
					__,
					TOKEN(WORD),
				__,
				MAKE2(ParenArgumentsNode),
				MAKE2(BracketArgumentsNode),
				TOKEN(PLUSPLUS),
				TOKEN(MINUSMINUS),
			____
		__,
	};
}

void ParenExpressionNode::prepare() {
	this->nodes = {
		_OR_
			_AND_
				TOKEN(PARENOPEN),
				MAKE2(ExpressionNode),
				TOKEN(PARENCLOSE),
			__,
			MAKE2(TypenameNode),
			TOKEN(NUMBER),
		__
	};
}
