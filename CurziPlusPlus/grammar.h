#pragma once
#include "primitives.h"

// forward declare recursive rules
struct Typename;
struct Statement;
using Expression = struct AssignmentExpression;

// to save some space since these are the most used token
using Word = Token<WORD>;
using String = Token<STRING>;
using Newline = Token<NEWLINE>;

template <typename T> struct value_t{ T value; };
template <typename... Ts> struct and_t { And<Ts...> value; };
template <typename... Ts> struct or_t { Or<Ts...> value; };

using Import = and_t<Token<IMPORT>, Or<Word, String>, Newline>;
using Alias = and_t<Token<USING>, Word, Token<EQUAL>, Typename, Newline>;
using ArgumentsSignature = value_t<CommaStar<And<Typename, Word>>>;
using ColonIndentCodeBlock = and_t<Token<COLON>, Newline, Indent<Star<Statement>>>;
using Function = and_t<Typename, Word, Token<PARENOPEN>, ArgumentsSignature, Token<PARENCLOSE>, ColonIndentCodeBlock>;
using ParenArguments = and_t<Token<PARENOPEN>, CommaStar<Expression>, Token<PARENCLOSE>>;
using BracketArguments = and_t<Token<BRACKETOPEN>, CommaStar<Expression>, Token<BRACKETCLOSE>>;
using NSTypename = and_t<Token<NS>, Typename>;
using TemplateTypename = and_t<Token<LT>, CommaStar<Typename>, Token<GT>, Opt<NSTypename>>;
struct Typename { And<Word, Opt<Or<NSTypename, TemplateTypename>>> value; };
struct TemplateDeclaration;
using TemplateTypenameDeclaration = and_t<TemplateDeclaration, Token<TYPE>, Word>;
struct TemplateDeclaration { And<Token<TEMPLATE>, Token<LT>, CommaPlus<Or<TemplateTypenameDeclaration, Token<TYPE>>>, Token<GT>> value; };
using ExpressionStatement = and_t<Expression, Newline>;
using VariableDeclarationStatement = and_t<Typename, Word, Newline>;
//using ElseStatement = and_t<IndentToken, Token<ELSE>, Opt<And<Token<IF>, Expression>>, ColonIndentCodeBlock>;
//using IfStatement = and_t<Token<IF>, Expression, ColonIndentCodeBlock, Opt<ElseStatement>>;
struct ElseStatement;
using IfStatement = and_t<Token<IF>, Expression, ColonIndentCodeBlock, Opt<Alloc<ElseStatement>>> ;
struct ElseStatement { And<IndentToken, Token<ELSE>, Or<Alloc<IfStatement>, ColonIndentCodeBlock >> value; };
using BreakStatement = and_t<Token<BREAK>, Opt<And<Token<IF>, Expression>>, Newline>;
using ClassInheritance = and_t<Token<EXTENDS>, CommaPlus<Typename>>;
using MemberVariable = and_t<Typename, Word, Newline>;
using Constructor = and_t<Word, Token<PARENOPEN>, ArgumentsSignature, Token<PARENCLOSE>, ColonIndentCodeBlock>;
using ClassElement = or_t<Alias, Function, MemberVariable, Constructor>;
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
		And<Or<And<Typename, Word>, Word>, Token<COMMA>>, // require a variable for the index
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
			Expression,
			Opt<And<
				Token<ELSE>,
				Expression
			>>
		>>,
		Newline
	>;
struct Statement {
	And<
		IndentToken,
		Or<
			ExpressionStatement,
			VariableDeclarationStatement,
			IfStatement,
			ForStatement,
			IForStatement,
			WhileStatement,
			BreakStatement,
			ReturnStatement
		>
	> value;
};
using Class = and_t<
		Opt<TemplateDeclaration>,
		Token<CLASS>,
		Word,
		Opt<ClassInheritance>,
		Token<COLON>,
		Newline,
		Indent<Star<And<
            IndentToken,
            ClassElement
		>>>
    >;
using File = and_t<Star<Import>, Star<Or<Class, Function>>, Token<END>>;
/*
EXPRESSIONS
*/
using ParenExpression = or_t<
		And<
			Token<PARENOPEN>,
			Expression,
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
struct UnaryExpression {
	Or <
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
			UnaryExpression // recursive here
		>,
		PostfixExpression
	> value;
};
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
			Opt<And<
				Token<ELSE>,
				OrExpression
			>>
		>>
	>;
struct AssignmentExpression {
	And<
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
	> value;
};
