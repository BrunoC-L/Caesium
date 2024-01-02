#pragma once
#include "primitives.hpp"

// forward declare recursive rules
struct Typename;
struct Statement;
struct ElseStatement;
struct ConditionalExpression;
struct FunctionArgument;

// to save some space since these are the most used token
using Word = Token<WORD>;
using String = Token<STRING>;
using Newline = Token<NEWLINE>;

using Expression = ConditionalExpression;
using Import = And<Star<Token<NEWLINE>>, Token<IMPORT>, Or<Word, String>, Newline>;
using Alias = And<Token<USING>, Word, Token<EQUAL>, Typename, Newline>;
using ParameterCategory = Or</*Token<KEY>, */Token<VAL>, And<Token<REF>, Token<NOT>>, Token<REF>>;
using ArgumentCategory  = Or<Token<COPY>,   Token<MOVE>, And<Token<REF>, Token<NOT>>, Token<REF>>;
using FunctionParameter = And<Typename, ParameterCategory, Word>;
using FunctionParameters = CommaStar<FunctionParameter>;
using ColonIndentCodeBlock = And<Token<COLON>, Newline, Indent<Star<Statement>>>;
using Function = And<Typename, Word, Token<PARENOPEN>, FunctionParameters, Token<PARENCLOSE>, ColonIndentCodeBlock>;
using ParenArguments = And<Token<PARENOPEN>, CommaStar<FunctionArgument>, Token<PARENCLOSE>>;
using BraceArguments = And<Token<BRACEOPEN>, CommaStar<FunctionArgument>, Token<BRACECLOSE>>;
using BracketArguments = And<Token<BRACKETOPEN>, CommaStar<FunctionArgument>, Token<BRACKETCLOSE>>;

using NamespaceTypenameExtension = And<Token<NS>, Alloc<Typename>>;
using TemplateTypenameExtension = And<Token<LT>, CommaStar<Alloc<Typename>>, Token<GT>, Opt<NamespaceTypenameExtension>>;
using UnionTypenameExtension = And<Token<BITOR>, Alloc<Typename>>;

struct Typename : public And<Word, Opt<Or<NamespaceTypenameExtension, TemplateTypenameExtension, UnionTypenameExtension>>> {};

using TemplateDeclaration = And<Token<TEMPLATE>, Token<LT>, CommaPlus<Word>, Token<GT>>;

using MemberVariable = And<Typename, Word, Newline>;
using Constructor = And<Word, Token<PARENOPEN>, FunctionParameters, Token<PARENCLOSE>, ColonIndentCodeBlock>;
using ClassElement = Or<Alias, Function, MemberVariable, Constructor>;

using ParenExpression = Or<
	Word,
	Token<NUMBER>,
	Token<STRING>,
	And<Token<PARENOPEN>, CommaStar<Expression>, Token<PARENCLOSE>>,
	ParenArguments,
	BracketArguments,
	BraceArguments
>;
using PostfixExpression = And<
	ParenExpression,
	Star<Or<
        And<
			Token<DOT>,
            Word
        >,
        ParenArguments,
        BracketArguments,
        BraceArguments,
		Token<PLUSPLUS>,
		Token<MINUSMINUS>
	>>
>;

using unary_operators = Or<Token<DASH>, Token<TILDE> /*, Token<NOT>, Token<ASTERISK>, Token<AMPERSAND>*/>;
using UnaryExpression = Alloc<And<Star<unary_operators>, PostfixExpression>>;
using MultiplicativeExpression = And<UnaryExpression, Star<And<Or<Token<ASTERISK>, Token<SLASH>, Token<PERCENT>>, UnaryExpression>>>;
using AdditiveExpression = And<MultiplicativeExpression, Star<And<Or<Token<PLUS>, Token<DASH>>, MultiplicativeExpression>>>;
using CompareExpression = And<AdditiveExpression, Star<And<Or<Token<LT>, Token<LTE>, Token<GT>, Token<GTE>>, AdditiveExpression>>>;
using EqualityExpression = And<CompareExpression, Star<And<Or<Token<EQUALEQUAL>, Token<NEQUAL>>, CompareExpression>>>;
using AndExpression = And<EqualityExpression, Star<And<Token<AND>, EqualityExpression>>>;
using OrExpression = And<AndExpression, Star<And<Token<OR>, AndExpression>>>;
struct ConditionalExpression : public And<
	OrExpression,
	Opt<And<
		Token<IF>,
		OrExpression,
		Token<ELSE>,
		OrExpression
	>>
> {};

struct FunctionArgument : And<Opt<ArgumentCategory>, Expression> {};

using ExpressionStatement = And<Expression, Newline>;
using BlockDeclaration = And<Token<BLOCK>, ColonIndentCodeBlock>;
using BlockStatement = And<Token<BLOCK>, Typename>;
using VariableDeclaration = And<Typename, Word>;
using VariableDeclarationStatement = And<Typename, Word, Token<EQUAL>, Expression, Newline>;
using IfStatement = And<Token<IF>, Expression, ColonIndentCodeBlock, Opt<Alloc<ElseStatement>>>;

struct ElseStatement : public And<IndentToken, Token<ELSE>, Or<Alloc<IfStatement>, ColonIndentCodeBlock>> {};

using BreakStatement = And<Token<BREAK>, Opt<And<Token<IF>, Expression>>, Newline>;
using ForStatement = And<
	Token<FOR>,
	CommaPlus<Or<VariableDeclaration, Word>>,
	Token<IN>,
	Expression,
	Opt<And<Token<IF>, Expression>>,
	Opt<And<Token<WHILE>, Expression>>,
	ColonIndentCodeBlock
>;
using IForStatement = And<
	Token<IFOR>,
	Word, // require a variable for the index
	Token<COMMA>,
	CommaPlus<Or<VariableDeclaration, Word>>, // and at least 1 variable iterating
	Token<IN>,
	Expression,
	Opt<And<Token<IF>, Expression>>,
	Opt<And<Token<WHILE>, Expression>>,
	ColonIndentCodeBlock
>;
using WhileStatement = And<Token<WHILE>, Expression, ColonIndentCodeBlock>;
using ReturnStatement = And<
	Token<RETURN>,
	CommaStar<FunctionArgument>,
	Opt<And<
		Token<IF>,
		Expression
	>>,
	Newline
>;
struct Statement : public And<
	IndentToken,
	Alloc<Or<
		VariableDeclarationStatement,
		ExpressionStatement,
		IfStatement,
		ForStatement,
		IForStatement,
		WhileStatement,
		BreakStatement,
		ReturnStatement,
		BlockStatement
	>>
>{};

using Type = And<
	Token<TYPE>,
	Word,
	Token<COLON>,
	Newline,
	Indent<Star<And<
        IndentToken,
        ClassElement
	>>>
>;


template <typename T>
using Template = And<
	TemplateDeclaration,
	Token<NEWLINE>,
	T
>;

using File = And<
	Star<Import>,
	Star<
		Or< Token<NEWLINE>,
			Type,
			Function,
			Template<Type>,
			Template<Function>,
			Template<BlockDeclaration>,
			Alias
		>
	>,
	Token<END>
>;
