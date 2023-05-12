#pragma once
#include "primitives.h"

// forward declare recursive rules
struct Typename;
struct Statement;
using Expression = struct AssignmentExpression;
struct TemplateDeclaration;
struct ElseStatement;
// each other these rules requires a struct definition and to avoid trouble
// we inherit from and_t or or_t and require supertype initialization
// so we use a macro instead since the alternative is longer to type
#define makeinherit(rulename, ...) struct rulename : public __VA_ARGS__{rulename(int x) : __VA_ARGS__({ x }) {};};

// to save some space since these are the most used token
using Word = Token<WORD>;
using String = Token<STRING>;
using Newline = Token<NEWLINE>;

template <typename... Ts> struct and_t {
	And<Ts...> _value;
	template <typename T>
	const T& get() const {
		return _value.get<T>();
	}
	const auto& value() const {
		return _value.value();
	}
};
template <typename... Ts> struct or_t {
	Or<Ts...> _value;
	template <typename T>
	const T& get() const {
		return _value.get<T>();
	}
	const auto& value() const {
		return _value.value();
	}
};

using Import = and_t<Token<IMPORT>, Or<Word, String>, Newline>;
using Alias = and_t<Token<USING>, Word, Token<EQUAL>, Typename, Newline>;
using ArgumentsSignature = CommaStar<And<Typename, Word>>;
using ColonIndentCodeBlock = and_t<Token<COLON>, Newline, Indent<Star<Statement>>>;
using Function = and_t<Typename, Word, Token<PARENOPEN>, ArgumentsSignature, Token<PARENCLOSE>, ColonIndentCodeBlock>;
using ParenArguments = and_t<Token<PARENOPEN>, CommaStar<Expression>, Token<PARENCLOSE>>;
using BracketArguments = and_t<Token<BRACKETOPEN>, CommaStar<Expression>, Token<BRACKETCLOSE>>;
using NSTypename = and_t<Token<NS>, Alloc<Typename>>;
using TemplateTypename = and_t<Token<LT>, CommaStar<Alloc<Typename>>, Token<GT>, Opt<NSTypename>>;
makeinherit(Typename, and_t<Word, Opt<Or<NSTypename, TemplateTypename>>>);
using TemplateTypenameDeclaration = and_t<Alloc<TemplateDeclaration>, Token<TYPE>, Word>;
makeinherit(TemplateDeclaration, and_t<Token<TEMPLATE>, Token<LT>, CommaPlus<Or<TemplateTypenameDeclaration, Token<TYPE>>>, Token<GT>>);

using ClassInheritance = and_t<Token<EXTENDS>, CommaPlus<Typename>>;
using MemberVariable = and_t<Typename, Word, Newline>;
using Constructor = and_t<Word, Token<PARENOPEN>, ArgumentsSignature, Token<PARENCLOSE>, ColonIndentCodeBlock>;
using ClassElement = or_t<Alias, Function, MemberVariable, Constructor>;

using ExpressionStatement = and_t<Expression, Newline>;
using VariableDeclarationStatement = and_t<Typename, Word, Newline>;
using IfStatement = and_t<Token<IF>, Expression, ColonIndentCodeBlock, Opt<Alloc<ElseStatement>>> ;
makeinherit(ElseStatement, and_t<IndentToken, Token<ELSE>, Or<Alloc<IfStatement>, ColonIndentCodeBlock>>);
using BreakStatement = and_t<Token<BREAK>, Opt<And<Token<IF>, Expression>>, Newline>;
using ForStatement = and_t<
		Token<FOR>,
		CommaPlus<Or<And<Typename, Word>, Word>>,
		Token<IN>,
		Expression,
		Opt<And<Token<IF>, Expression>>,
		Opt<And<Token<WHILE>, Expression>>,
		ColonIndentCodeBlock
	>;
using IForStatement = and_t<
		Token<IFOR>,
		Word, // require a variable for the index
		Token<COMMA>,
		CommaPlus<Or<And<Typename, Word>, Word>>, // and at least 1 variable iterating
		Token<IN>,
		Expression,
		Opt<And<Token<IF>, Expression>>,
		Opt<And<Token<WHILE>, Expression>>,
		ColonIndentCodeBlock
	>;
using WhileStatement = and_t<Token<WHILE>, Expression, ColonIndentCodeBlock>;
using ReturnStatement = and_t<
		Token<RETURN>,
		CommaStar<Expression>,
		Opt<And<
			Token<IF>,
			Expression
		>>,
		Newline
	>;
makeinherit(Statement,
and_t<
	IndentToken,
	Alloc<Or<
		ExpressionStatement,
		VariableDeclarationStatement,
		IfStatement,
		ForStatement,
		IForStatement,
		WhileStatement,
		BreakStatement,
		ReturnStatement
	>>
>);
using Type = and_t<
		Opt<TemplateDeclaration>,
		Token<TYPE>,
		Word,
		Opt<ClassInheritance>,
		Token<COLON>,
		Newline,
		Indent<Star<And<
            IndentToken,
            ClassElement
		>>>
    >;
using File = and_t<Star<Import>, Star<Or<Type, Function>>, Token<END>>;
/*
EXPRESSIONS
*/
using ParenExpression = or_t<
		And<
			Token<PARENOPEN>,
			Alloc<Expression>,
			Token<PARENCLOSE>
		>,
		Typename,
		Token<NUMBER>
	// todo string token
	>;
using PostfixExpression = and_t<
		ParenExpression,
		Star<Or<
            And<
				Token<DOT>,
                Word
            >,
            ParenArguments,
            BracketArguments,
			Token<PLUSPLUS>,
			Token<MINUSMINUS>
		>>
    >;

makeinherit(UnaryExpression,
or_t<
	And<
		Or<	// has to be recursive because of the type cast operator taking the same shape as a ParenExpression
			// so instead of `Star<Or> ... ____` we refer to UnaryExpression inside the Or
			Token<NOT>,
			Token<PLUS>,
			Token<DASH>,
			Token<PLUSPLUS>,
			Token<MINUSMINUS>,
			Token<TILDE>,
			Token<ASTERISK>,
			Token<AMPERSAND>,
			And< // type cast operator
				Token<PARENOPEN>,
				Typename,
				Token<PARENCLOSE>
			>
		>,
		Alloc<UnaryExpression> // recursive here
	>,
	PostfixExpression
>);

using MultiplicativeExpression = and_t<UnaryExpression, Star<And<Or<Token<ASTERISK>, Token<SLASH>, Token<PERCENT>>, UnaryExpression>>>;
using AdditiveExpression = and_t<MultiplicativeExpression, Star<And<Or<Token<PLUS>, Token<DASH>>, MultiplicativeExpression>>>;
using CompareExpression = and_t<AdditiveExpression, Star<And<Or<Token<LT>, Token<LTE>, Token<GT>, Token<GTE>>, AdditiveExpression>>>;
using EqualityExpression = and_t<CompareExpression, Star<And<Token<EQUALEQUAL>, CompareExpression>>>;
using AndExpression = and_t<EqualityExpression, Star<And<Token<AND>, EqualityExpression>>>;
using OrExpression = and_t<AndExpression, Star<And<Token<OR>, AndExpression>>>;
using ConditionalExpression = and_t<
		OrExpression,
		Opt<And<
			Token<IF>,
			OrExpression,
			Token<ELSE>,
			OrExpression
		>>
	>;

makeinherit(AssignmentExpression,
and_t<
	ConditionalExpression,
	Star<And<
		Or<
			Token<EQUAL>,
			Token<PLUSEQUAL>,
			Token<MINUSEQUAL>,
			Token<TIMESEQUAL>,
			Token<DIVEQUAL>,
			Token<MODEQUAL>,
			Token<ANDEQUAL>,
			Token<OREQUAL>,
			Token<XOREQUAL>
		>,
		ConditionalExpression
	>>
>);
