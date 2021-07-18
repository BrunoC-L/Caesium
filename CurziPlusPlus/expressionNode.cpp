#include "expressionNode.h"
#include "kNode.h"
#include "grammarizer.h"
#include "tokenNode.h"
#include "typenameNode.h"
#include "emptyNode.h"
#include "codeBlockNode.h"
#include "argumentsNode.h"
#include "macros.h"

void ExpressionNode::build() {
	nodes = {
		MAKE(AssignmentExpressionNode)(),
	};
}

void AssignmentExpressionNode::build() {
	nodes = {
		_AND_
			MAKE(ConditionalExpressionNode)(),
			_STAR_
				_AND_
					_OR_
						MAKE(TokenNode)(EQUAL),
						MAKE(TokenNode)(PLUSEQUAL),
						MAKE(TokenNode)(MINUSEQUAL),
						MAKE(TokenNode)(TIMESEQUAL),
						MAKE(TokenNode)(DIVEQUAL),
						MAKE(TokenNode)(MODEQUAL),
						MAKE(TokenNode)(ANDEQUAL),
						MAKE(TokenNode)(OREQUAL),
						MAKE(TokenNode)(XOREQUAL),
					__,
					MAKE(ConditionalExpressionNode)(),
				__
			___
		__
	};
}

void ConditionalExpressionNode::build() {
	nodes = {
		_AND_
			MAKE(OrExpressionNode)(),
			_STAR_
				_AND_
					MAKE(TokenNode)(QUESTION),
					MAKE(OrExpressionNode)(),
					MAKE(TokenNode)(COLON),
					MAKE(OrExpressionNode)(),
				__
			___
		__
	};
}

void OrExpressionNode::build() {
	nodes = {
		_AND_
			MAKE(AndExpressionNode)(),
			_STAR_
				_AND_
					MAKE(TokenNode)(OROR),
					MAKE(AndExpressionNode)(),
				__
			___
		__
	};
}

void AndExpressionNode::build() {
	nodes = {
		_AND_
			MAKE(BitOrExpressionNode)(),
			_STAR_
				_AND_
					MAKE(TokenNode)(ANDAND),
					MAKE(BitOrExpressionNode)(),
				__
			___
		__
	};
}

void BitOrExpressionNode::build() {
	nodes = {
		_AND_
			MAKE(BitXorExpressionNode)(),
			_STAR_
				_AND_
					MAKE(TokenNode)(BITOR),
					MAKE(BitXorExpressionNode)(),
				__
			___
		__
	};
}

void BitXorExpressionNode::build() {
	nodes = {
		_AND_
			MAKE(BitAndExpressionNode)(),
			_STAR_
				_AND_
					MAKE(TokenNode)(CARET),
					MAKE(BitAndExpressionNode)(),
				__
			___
		__
	};
}

void BitAndExpressionNode::build() {
	nodes = {
		_AND_
			MAKE(EqualityExpressionNode)(),
			_STAR_
				_AND_
					MAKE(TokenNode)(AMPERSAND),
					MAKE(EqualityExpressionNode)(),
				__
			___
		__
	};
}

void EqualityExpressionNode::build() {
	nodes = {
		_AND_
			MAKE(CompareExpressionNode)(),
			_STAR_
				_AND_
					MAKE(TokenNode)(EQUALEQUAL),
					MAKE(CompareExpressionNode)(),
				__
			___
		__
	};
}

void CompareExpressionNode::build() {
	nodes = {
		_AND_
			MAKE(BitShiftExpressionNode)(),
			_STAR_
				_AND_
					_OR_
						MAKE(TokenNode)(LT),
						MAKE(TokenNode)(LTE),
						MAKE(TokenNode)(GT),
						MAKE(TokenNode)(GTE),
					__,
					MAKE(BitShiftExpressionNode)(),
				__
			___
		__
	};
}

void BitShiftExpressionNode::build() {
	nodes = {
		_AND_
			MAKE(AdditiveExpressionNode)(),
			_STAR_
				_AND_
					_OR_
						MAKE(TokenNode)(LSHIFT),
						MAKE(TokenNode)(RSHIFT),
					__,
					MAKE(AdditiveExpressionNode)(),
				__
			___
		__
	};
}

void AdditiveExpressionNode::build() {
	nodes = {
		_AND_
			MAKE(MultiplicativeExpressionNode)(),
			_STAR_
				_AND_
					_OR_
						MAKE(TokenNode)(PLUS),
						MAKE(TokenNode)(DASH),
					__,
					MAKE(MultiplicativeExpressionNode)(),
				__
			___
		__
	};
}

void MultiplicativeExpressionNode::build() {
	nodes = {
		_AND_
			MAKE(UnaryExpressionNode)(),
			_STAR_
				_AND_
					_OR_
						MAKE(TokenNode)(ASTERISK),
						MAKE(TokenNode)(SLASH),
						MAKE(TokenNode)(PERCENT),
					__,
					MAKE(UnaryExpressionNode)(),
				__
			___
		__
	};
}

void UnaryExpressionNode::build() {
	nodes = {
		_AND_
			_STAR_
				_OR_
					MAKE(TokenNode)(NOT),
					MAKE(TokenNode)(PLUS),
					MAKE(TokenNode)(DASH),
					MAKE(TokenNode)(PLUSPLUS),
					MAKE(TokenNode)(MINUSMINUS),
					MAKE(TokenNode)(TILDE),
					MAKE(TokenNode)(ASTERISK),
					MAKE(TokenNode)(AMPERSAND),
					_AND_
						MAKE(TokenNode)(PARENOPEN),
						MAKE(TypenameNode)(),
						MAKE(TokenNode)(PARENCLOSE),
					__,
				__
			___,
			MAKE(PostfixExpressionNode)(),
		__
	};
}

void PostfixExpressionNode::build() {
	nodes = {
		_AND_
			MAKE(ParenExpressionNode)(),
			_STAR_
				_OR_
					_AND_
						_OR_
							MAKE(TokenNode)(DOT),
							MAKE(TokenNode)(ARROW),
						__,
						MAKE(TokenNode)(WORD),
					__,
					MAKE(ParenArgumentsNode)(),
					MAKE(BracketArgumentsNode)(),
					MAKE(TokenNode)(PLUSPLUS),
					MAKE(TokenNode)(MINUSMINUS),
				__
			___
		__,
	};
}

void ParenExpressionNode::build() {
	nodes = {
		_OR_
			_AND_
				MAKE(TokenNode)(PARENOPEN),
				MAKE(ExpressionNode)(),
				MAKE(TokenNode)(PARENCLOSE),
			__,
						MAKE(TokenNode)(WORD),
		__
	};
}
