#include "expressionNode.h"
#include "kNode.h"
#include "grammarizer.h"
#include "tokenNode.h"
#include "typenameNode.h"
#include "codeBlockNode.h"
#include "argumentsNode.h"
#include "macros.h"

template <typename T>
void ExpressionNode<T>::build() {
	this->nodes = {
		MAKE(AssignmentExpressionNode)(),
	};
}

template <typename T>
void AssignmentExpressionNode<T>::build() {
	this->nodes = {
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

template <typename T>
void ConditionalExpressionNode<T>::build() {
	this->nodes = {
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

template <typename T>
void OrExpressionNode<T>::build() {
	this->nodes = {
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

template <typename T>
void AndExpressionNode<T>::build() {
	this->nodes = {
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

template <typename T>
void BitOrExpressionNode<T>::build() {
	this->nodes = {
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

template <typename T>
void BitXorExpressionNode<T>::build() {
	this->nodes = {
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

template <typename T>
void BitAndExpressionNode<T>::build() {
	this->nodes = {
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

template <typename T>
void EqualityExpressionNode<T>::build() {
	this->nodes = {
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

template <typename T>
void CompareExpressionNode<T>::build() {
	this->nodes = {
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

template <typename T>
void BitShiftExpressionNode<T>::build() {
	this->nodes = {
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

template <typename T>
void AdditiveExpressionNode<T>::build() {
	this->nodes = {
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

template <typename T>
void MultiplicativeExpressionNode<T>::build() {
	this->nodes = {
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

template <typename T>
void UnaryExpressionNode<T>::build() {
	this->nodes = {
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
					_AND_ // type cast operator
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

template <typename T>
void PostfixExpressionNode<T>::build() {
	this->nodes = {
		_AND_
			MAKE(ParenExpressionNode)(),
			_STAR_ _OR_
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
			____
		__,
	};
}

template <typename T>
void ParenExpressionNode<T>::build() {
	this->nodes = {
		_OR_
			_AND_
				MAKE(TokenNode)(PARENOPEN),
				MAKE(ExpressionNode)(),
				MAKE(TokenNode)(PARENCLOSE),
			__,
			MAKE(TypenameNode)(),
			MAKE(TokenNode)(NUMBER),
		__
	};
}
