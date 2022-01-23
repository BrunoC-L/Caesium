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
		MAKE(AssignmentExpressionNode)(),
	};
}

void AssignmentExpressionNode::build() {
	this->nodes = {
		_AND_
			MAKE(ConditionalExpressionNode)(),
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
					MAKE(ConditionalExpressionNode)(),
				__
			___
		__
	};
}

void ConditionalExpressionNode::build() {
	this->nodes = {
		_AND_
			MAKE(OrExpressionNode)(),
			/*_OPT_ _AND_
				TOKEN(QUESTION),
				MAKE(OrExpressionNode)(),
				TOKEN(COLON),
				MAKE(OrExpressionNode)()
			____*/
			_OPT_ _AND_
				TOKEN(IF),
				MAKE(OrExpressionNode)(),
				_OPT_ _AND_
					TOKEN(ELSE),
					MAKE(OrExpressionNode)(),
				____,
			____
		__
	};
}

void OrExpressionNode::build() {
	this->nodes = {
		_AND_
			MAKE(AndExpressionNode)(),
			_STAR_
				_AND_
					TOKEN(OR), // OR = 'or', OROR = '||'
					MAKE(AndExpressionNode)(),
				__
			___
		__
	};
}

void AndExpressionNode::build() {
	this->nodes = {
		_AND_
			MAKE(BitOrExpressionNode)(),
			_STAR_
				_AND_
					TOKEN(AND), // AND = 'and', ANDAND = '&&'
					MAKE(BitOrExpressionNode)(),
				__
			___
		__
	};
}

void BitOrExpressionNode::build() {
	this->nodes = {
		_AND_
			MAKE(BitXorExpressionNode)(),
			_STAR_
				_AND_
					TOKEN(BITOR),
					MAKE(BitXorExpressionNode)(),
				__
			___
		__
	};
}

void BitXorExpressionNode::build() {
	this->nodes = {
		_AND_
			MAKE(BitAndExpressionNode)(),
			_STAR_
				_AND_
					TOKEN(CARET),
					MAKE(BitAndExpressionNode)(),
				__
			___
		__
	};
}

void BitAndExpressionNode::build() {
	this->nodes = {
		_AND_
			MAKE(EqualityExpressionNode)(),
			_STAR_
				_AND_
					TOKEN(AMPERSAND),
					MAKE(EqualityExpressionNode)(),
				__
			___
		__
	};
}

void EqualityExpressionNode::build() {
	this->nodes = {
		_AND_
			MAKE(CompareExpressionNode)(),
			_STAR_
				_AND_
					TOKEN(EQUALEQUAL),
					MAKE(CompareExpressionNode)(),
				__
			___
		__
	};
}

void CompareExpressionNode::build() {
	this->nodes = {
		_AND_
			MAKE(BitShiftExpressionNode)(),
			_STAR_
				_AND_
					_OR_
						TOKEN(LT),
						TOKEN(LTE),
						TOKEN(GT),
						TOKEN(GTE),
					__,
					MAKE(BitShiftExpressionNode)(),
				__
			___
		__
	};
}

void BitShiftExpressionNode::build() {
	this->nodes = {
		_AND_
			MAKE(AdditiveExpressionNode)(),
			_STAR_
				_AND_
					_OR_
						TOKEN(LSHIFT),
						TOKEN(RSHIFT),
					__,
					MAKE(AdditiveExpressionNode)(),
				__
			___
		__
	};
}

void AdditiveExpressionNode::build() {
	this->nodes = {
		_AND_
			MAKE(MultiplicativeExpressionNode)(),
			_STAR_
				_AND_
					_OR_
						TOKEN(PLUS),
						TOKEN(DASH),
					__,
					MAKE(MultiplicativeExpressionNode)(),
				__
			___
		__
	};
}

void MultiplicativeExpressionNode::build() {
	this->nodes = {
		_AND_
			MAKE(UnaryExpressionNode)(),
			_STAR_
				_AND_
					_OR_
						TOKEN(ASTERISK),
						TOKEN(SLASH),
						TOKEN(PERCENT),
					__,
					MAKE(UnaryExpressionNode)(),
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
						MAKE(TypenameNode)(),
						TOKEN(PARENCLOSE),
					__,
				__,
				MAKE(UnaryExpressionNode)(), // has to be recursive because of the type cast operator taking the same shape as a ParenExpression
			__,
			MAKE(PostfixExpressionNode)(),
		__
	};
}

void PostfixExpressionNode::build() {
	this->nodes = {
		_AND_
			MAKE(ParenExpressionNode)(),
			_STAR_ _OR_
				_AND_
					_OR_
						TOKEN(DOT),
						TOKEN(ARROW),
					__,
					WORD_TOKEN(),
				__,
				MAKE(ParenArgumentsNode)(),
				MAKE(BracketArgumentsNode)(),
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
				MAKE(ExpressionNode)(),
				TOKEN(PARENCLOSE),
			__,
			MAKE(TypenameNode)(),
			NUMBER_TOKEN(),
		__
	};
}
