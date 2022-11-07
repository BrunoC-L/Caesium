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

using Import = value_t<And<Token<IMPORT>, Or<Word, String>, Token<NEWLINE>>>;
using Alias = value_t<And<Token<USING>, Word, Token<EQUAL>, Typename, Token<NEWLINE>>>;
using ArgumentsSignature = value_t<CommaStar<And<Typename, Word>>>;
using ColonIndentCodeBlock = value_t<And<Token<COLON>, Token<NEWLINE>, Indent<Star<Statement>>>>;
using Function = value_t<And<Typename, Word, Token<PARENOPEN>, ArgumentsSignature, Token<PARENCLOSE>, ColonIndentCodeBlock>>;
using ParenArguments = value_t<And<Token<PARENOPEN>, CommaStar<Expression>, Token<PARENCLOSE>>>;
using BracketArguments = value_t<And<Token<BRACKETOPEN>, CommaStar<Expression>, Token<BRACKETCLOSE>>>;
using NSTypename = value_t<And<Token<NS>, Typename>>;
using TemplateTypename = value_t<And<Token<LT>, CommaStar<Typename>, Token<GT>, Opt<NSTypename>>>;
struct Typename { And<Word, Opt<Or<NSTypename, TemplateTypename>>> value; };
struct TemplateTypenameDeclaration { And<Word, Token<LT>, CommaPlus<Or<TemplateTypenameDeclaration, Word>>, Token<GT>> value; };
using PPPQualifier = value_t<Or<Token<PUBLIC>, Token<PRIVATE>, Token<PROTECTED>>>;
using ExpressionStatement = value_t<And<Expression, Newline>>;
using ElseStatement = value_t<And<IndentToken, Token<ELSE>, ColonIndentCodeBlock>>;
using IfStatement = value_t<And<Token<IF>, Expression, ColonIndentCodeBlock, Opt<ElseStatement>>>;
using BreakStatement = value_t<And<Token<BREAK>, Opt<And<Token<IF>, Expression>>, Newline>>;
using MultipleInheritance = value_t<CommaPlus<Typename>>;
using ClassInheritance = value_t<And<Token<EXTENDS>, MultipleInheritance>>;
using ClassMemberQualifiers = value_t<And<Opt<PPPQualifier>, Opt<Token<STATIC>>>>;
using MemberVariable = value_t<And<ClassMemberQualifiers, Typename, Word, Newline>>;
using Constructor = value_t<And<ClassMemberQualifiers, Word, Token<PARENOPEN>, ArgumentsSignature, Token<PARENCLOSE>, ColonIndentCodeBlock>>;
using ClassElement = value_t<Or<Alias, Function, MemberVariable, Constructor>>;
using ForStatement = value_t<
	And<
		Token<FOR>,
		CommaPlus<Or<And<Typename, Word>, Word>>,
		Token<IN>,
		Expression,
		Opt<And<Token<IF>, Expression>>,
		Opt<And<Token<WHILE>, Expression>>,
		ColonIndentCodeBlock
	>>;
using IForStatement = value_t<
	And<
		Token<IFOR>,
		And<Or<And<Typename, Word>, Word>, Token<COMMA>>, // require a variable for the index
		CommaPlus<Or<And<Typename, Word>, Word>>, // and at least 1 variable iterating
		Token<IN>,
		Expression,
		Opt<And<Token<IF>, Expression>>,
		Opt<And<Token<WHILE>, Expression>>,
		ColonIndentCodeBlock
	>>;
using WhileStatement = value_t<And<Token<WHILE>, Expression, ColonIndentCodeBlock>>;
using ReturnStatement = value_t<
	And<
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
	>>;
struct Statement {
	And<
		IndentToken,
		Or<
			And<Expression, Newline>,
			IfStatement,
			ForStatement,
			IForStatement,
			WhileStatement,
			ReturnStatement,
			BreakStatement
		>
	> value;
};
using Class = value_t<
	And<
		Token<CLASS>,
		Or<TemplateTypenameDeclaration, Word>,
		Opt<ClassInheritance>,
		Token<COLON>,
		Newline,
		Indent<Star<And<
            IndentToken,
            ClassElement
		>>>
    >>;
using File = value_t<
	And<Star<Import>, Star<Or<Class, Function>>, Token<END>>>;
/*
EXPRESSIONS
*/
using ParenExpression = value_t<
	Or<
		And<
			Token<PARENOPEN>,
			Expression,
			Token<PARENCLOSE>
		>,
		Typename,
		Token<NUMBER>
	>>;
using PostfixExpression = value_t<
	And<
		ParenExpression,
		Star<Or<
            And<
                Or<
					Token<DOT>,
					Token<ARROW>
                >,
                Word
            >,
            ParenArguments,
            BracketArguments,
			Token<PLUSPLUS>,
			Token<MINUSMINUS>
		>>
    >>;
struct UnaryExpression {
	Or <
		And<
			Or<	// has to be recursive because of the type cast operator taking the same shape as a ParenExpression
				// so instead of Star<Or> ... ____ we refer to UnaryExpression inside the Or
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
			UnaryExpression // recursive
		>,
		PostfixExpression
	> value;
};
using MultiplicativeExpression = value_t<And<UnaryExpression, Star<And<Or<Token<ASTERISK>, Token<SLASH>, Token<PERCENT>>, UnaryExpression>>>>;
using AdditiveExpression = value_t<And<MultiplicativeExpression, Star<And<Or<Token<PLUS>, Token<DASH>>, MultiplicativeExpression>>>>;
using CompareExpression = value_t<And<AdditiveExpression, Star<And<Or<Token<LT>, Token<LTE>, Token<GT>, Token<GTE>>, AdditiveExpression>>>>;
using EqualityExpression = value_t<And<CompareExpression, Star<And<Token<EQUALEQUAL>, CompareExpression>>>>;
using AndExpression = value_t<And<EqualityExpression, Star<And<Token<AND>, EqualityExpression>>>>;
using OrExpression = value_t<And<AndExpression, Star<And<Token<OR>, AndExpression>>>>;
using ConditionalExpression = value_t<
	And<
		OrExpression,
		Opt<And<
			Token<IF>,
			OrExpression,
			Opt<And<
				Token<ELSE>,
				OrExpression
			>>
		>>
	>>;
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
