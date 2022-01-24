#include "expressionNode.h"
#include "kNode.h"
#include "grammarizer.h"
#include "tokenNode.h"
#include "typenameNode.h"
#include "statementNode.h"
#include "argumentsNode.h"
#include "macros.h"

void ExpressionNode::build() {
	this->nodes = {
		MAKE_NAMED(AssignmentExpressionNode, "AssignmentExpression"),
	};
}

void AssignmentExpressionNode::build() {
	this->nodes = {
		_AND_
			MAKE_NAMED(ConditionalExpressionNode, "ConditionalExpression"),
			_STAR_("assignments")
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
					MAKE_NAMED(ConditionalExpressionNode, "ConditionalExpression"),
				__
			___
		__
	};
}

void ConditionalExpressionNode::build() {
	this->nodes = {
		_AND_
			MAKE_NAMED(OrExpressionNode, "OrExpression"),
			_OPT_("opt_conditional_if") _AND_
				TOKEN(IF),
				MAKE_NAMED(OrExpressionNode, "OrExpression"),
				_OPT_("opt_conditional_else") _AND_
					TOKEN(ELSE),
					MAKE_NAMED(OrExpressionNode, "OrExpression"),
				____,
			____
		__
	};
}

void OrExpressionNode::build() {
	this->nodes = {
		_AND_
			MAKE_NAMED(AndExpressionNode, "AndExpression"),
			_STAR_("ors")
				_AND_
					TOKEN(OR), // OR = 'or', OROR = '||'
					MAKE_NAMED(AndExpressionNode, "AndExpression"),
				__
			___
		__
	};
}

void AndExpressionNode::build() {
	this->nodes = {
		_AND_
			MAKE_NAMED(EqualityExpressionNode, "EqualityExpression"),
			_STAR_("ands")
				_AND_
					TOKEN(AND), // AND = 'and', ANDAND = '&&'
					MAKE_NAMED(EqualityExpressionNode, "EqualityExpression"),
				__
			___
		__
	};
}

void EqualityExpressionNode::build() {
	this->nodes = {
		_AND_
			MAKE_NAMED(CompareExpressionNode, "CompareExpression"),
			_STAR_("equalities")
				_AND_
					TOKEN(EQUALEQUAL),
					MAKE_NAMED(CompareExpressionNode, "CompareExpression"),
				__
			___
		__
	};
}

void CompareExpressionNode::build() {
	this->nodes = {
		_AND_
			MAKE_NAMED(AdditiveExpressionNode, "AdditiveExpression"),
			_STAR_("comparisons")
				_AND_
					_OR_
						TOKEN(LT),
						TOKEN(LTE),
						TOKEN(GT),
						TOKEN(GTE),
					__,
					MAKE_NAMED(AdditiveExpressionNode, "AdditiveExpression"),
				__
			___
		__
	};
}

void AdditiveExpressionNode::build() {
	this->nodes = {
		_AND_
			MAKE_NAMED(MultiplicativeExpressionNode, "MultiplicativeExpression"),
			_STAR_("addsubs")
				_AND_
					_OR_
						TOKEN(PLUS),
						TOKEN(DASH),
					__,
					MAKE_NAMED(MultiplicativeExpressionNode, "MultiplicativeExpression"),
				__
			___
		__
	};
}

void MultiplicativeExpressionNode::build() {
	this->nodes = {
		_AND_
			MAKE_NAMED(UnaryExpressionNode, "UnaryExpression"),
			_STAR_("muldivmod")
				_AND_
					_OR_
						TOKEN(ASTERISK),
						TOKEN(SLASH),
						TOKEN(PERCENT),
					__,
					MAKE_NAMED(UnaryExpressionNode, "UnaryExpression"),
				__
			___
		__
	};
}

void UnaryExpressionNode::build() {
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
						MAKE_NAMED(TypenameNode, "typename"),
						TOKEN(PARENCLOSE),
					__,
				__,
				MAKE_NAMED(UnaryExpressionNode, "UnaryExpression"),
			__,
			MAKE_NAMED(PostfixExpressionNode, "PostfixExpression"),
		__
	};
}

void PostfixExpressionNode::build() {
	this->nodes = {
		_AND_
			MAKE_NAMED(ParenExpressionNode, "ParenExpression"),
			_STAR_("postfixes") _OR_
				_AND_
					_OR_
						TOKEN(DOT),
						TOKEN(ARROW),
					__,
					WORD_TOKEN("word"),
				__,
				MAKE_NAMED(ParenArgumentsNode, "ParenArguments"),
				MAKE_NAMED(BracketArgumentsNode, "BracketArguments"),
				TOKEN(PLUSPLUS),
				TOKEN(MINUSMINUS),
			____
		__,
	};
}

void ParenExpressionNode::build() {
	this->nodes = {
		_OR_
			_AND_
				TOKEN(PARENOPEN),
				MAKE_NAMED(ExpressionNode, "Expression"),
				TOKEN(PARENCLOSE),
			__,
			MAKE_NAMED(TypenameNode, "typename"),
			NUMBER_TOKEN("num"),
		__
	};
}
