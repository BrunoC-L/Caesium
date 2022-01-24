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
			/*_OPT_ _AND_
				TOKEN(QUESTION),
				MAKE_NAMED(OrExpressionNode, "OrExpression"),
				TOKEN(COLON),
				MAKE_NAMED(OrExpressionNode, "OrExpression")
			____*/
			_OPT_ _AND_
				TOKEN(IF),
				MAKE_NAMED(OrExpressionNode, "OrExpression"),
				_OPT_ _AND_
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
			_STAR_
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
			MAKE_NAMED(BitOrExpressionNode, "BitOrExpression"),
			_STAR_
				_AND_
					TOKEN(AND), // AND = 'and', ANDAND = '&&'
					MAKE_NAMED(BitOrExpressionNode, "BitOrExpression"),
				__
			___
		__
	};
}

void BitOrExpressionNode::build() {
	this->nodes = {
		_AND_
			MAKE_NAMED(BitXorExpressionNode, "BitXorExpression"),
			_STAR_
				_AND_
					TOKEN(BITOR),
					MAKE_NAMED(BitXorExpressionNode, "BitXorExpression"),
				__
			___
		__
	};
}

void BitXorExpressionNode::build() {
	this->nodes = {
		_AND_
			MAKE_NAMED(BitAndExpressionNode, "BitAndExpression"),
			_STAR_
				_AND_
					TOKEN(CARET),
					MAKE_NAMED(BitAndExpressionNode, "BitAndExpression"),
				__
			___
		__
	};
}

void BitAndExpressionNode::build() {
	this->nodes = {
		_AND_
			MAKE_NAMED(EqualityExpressionNode, "EqualityExpression"),
			_STAR_
				_AND_
					TOKEN(AMPERSAND),
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
			_STAR_
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
			MAKE_NAMED(BitShiftExpressionNode, "BitShiftExpression"),
			_STAR_
				_AND_
					_OR_
						TOKEN(LT),
						TOKEN(LTE),
						TOKEN(GT),
						TOKEN(GTE),
					__,
					MAKE_NAMED(BitShiftExpressionNode, "BitShiftExpression"),
				__
			___
		__
	};
}

void BitShiftExpressionNode::build() {
	this->nodes = {
		_AND_
			MAKE_NAMED(AdditiveExpressionNode, "AdditiveExpression"),
			_STAR_
				_AND_
					_OR_
						TOKEN(LSHIFT),
						TOKEN(RSHIFT),
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
			_STAR_
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
			_STAR_
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
				_OR_
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
						MAKE_NAMED(TypenameNode, "Typename"),
						TOKEN(PARENCLOSE),
					__,
				__,
				MAKE_NAMED(UnaryExpressionNode, "UnaryExpression"), // has to be recursive because of the type cast operator taking the same shape as a ParenExpression
			__,
			MAKE_NAMED(PostfixExpressionNode, "PostfixExpression"),
		__
	};
}

void PostfixExpressionNode::build() {
	this->nodes = {
		_AND_
			MAKE_NAMED(ParenExpressionNode, "ParenExpression"),
			_STAR_ _OR_
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
			MAKE_NAMED(TypenameNode, "Typename"),
			NUMBER_TOKEN("num"),
		__
	};
}
