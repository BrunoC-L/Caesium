#pragma once
#include "primitives.hpp"

// forward declare recursive rules
struct Typename; // typenames contain typenames
struct Statement; // statements contain statements
struct ConditionalExpression; // expressions contain expressions
struct FunctionArgument; // expressions use function arguments and function arguments use expressions

// alias the most used tokens
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
using ColonIndentCodeBlock = And<Token<COLON>, Newline, Indent<Star<Or<Token<NEWLINE>, Statement>>>>;
using Function = And<Typename, Word, Token<PARENOPEN>, FunctionParameters, Token<PARENCLOSE>, ColonIndentCodeBlock>;
using ParenArguments = And<Token<PARENOPEN>, CommaStar<FunctionArgument>, Token<PARENCLOSE>>;
using BraceArguments = And<Token<BRACEOPEN>, CommaStar<FunctionArgument>, Token<BRACECLOSE>>;
using BracketArguments = And<Token<BRACKETOPEN>, CommaStar<FunctionArgument>, Token<BRACKETCLOSE>>;
using TemplateArguments = And<Token<LT>, CommaStar<Expression>, Token<GT>>;

using NamespaceTypenameExtension = And<Token<NS>, Alloc<Typename>>;
using TemplateTypenameExtension = And<Token<LT>, CommaStar<Alloc<Typename>>, Token<GT>, Opt<NamespaceTypenameExtension>>;
using UnionTypenameExtension = And<Token<BITOR>, Alloc<Typename>>;

struct Typename : public And<Or<Token<AUTO>, Word>, Opt<Or<NamespaceTypenameExtension, TemplateTypenameExtension, UnionTypenameExtension>>> {};

using MemberVariable = And<Typename, Word, Newline>;
//using Constructor = And<Word, Token<PARENOPEN>, FunctionParameters, Token<PARENCLOSE>, ColonIndentCodeBlock>;
using ClassElement = Or<Alias/*, Function*/, MemberVariable/*, Constructor*/>;

using ParenExpression = Or<
	Word,
	Token<FLOATING_POINT_NUMBER>,
	Token<INTEGER_NUMBER>,
	Token<STRING>,
	And<Token<PARENOPEN>, CommaStar<Expression>, Token<PARENCLOSE>>,
	ParenArguments,
	BracketArguments,
	BraceArguments
>;
using Postfix = Or<
	And<
		Token<DOT>,
		Word
	>,
	ParenArguments,
	BracketArguments,
	BraceArguments,
	TemplateArguments
	/*,
	Token<PLUSPLUS>,
	Token<MINUSMINUS>*/
>;
using PostfixExpression = And<
	ParenExpression,
	Star<Postfix>
>;

using unary_operators = Or<Token<DASH>, Token<TILDE> /*, Token<NOT>, Token<ASTERISK>, Token<AMPERSAND>*/>;
using UnaryExpression = Alloc<And<Star<unary_operators>, PostfixExpression>>;
using MultiplicativeExpression = And<UnaryExpression, Star<And<Or<Token<ASTERISK>, Token<SLASH>, Token<PERCENT>>, UnaryExpression>>>;
using AdditiveExpression = And<MultiplicativeExpression, Star<And<Or<Token<PLUS>, Token<DASH>>, MultiplicativeExpression>>>;
using CompareOperator = Or<Token<LTQ>, Token<LTEQ>, Token<GTQ>, Token<GTEQ>>;
using CompareExpression = And<AdditiveExpression, Star<And<CompareOperator, AdditiveExpression>>>;
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

struct ElseStatement; // we need to explicitly allow `else if <>:` otherwise using `else {ifstatement}` would require indentation
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

using Interface = And<
	Token<INTERFACE>,
	Word,
	Token<COLON>,
	Newline,
	Indent<Star<And<
        IndentToken,
		Or<
			Alias,
			MemberVariable
		>
	>>>
>;

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

using Template = And<Token<TEMPLATE>, Word, Token<LT>, CommaStar<And<Word, Opt<And<Token<EQUAL>, Expression>>>>, Token<GT>, Token<COLON>, Token<NEWLINE>, TemplateBody>;

using File = And<
	Star<Import>,
	Star<
		Or< Token<NEWLINE>,
			Type,
			Function,
			Interface,
			Template,
			/*Template<Type>,
			Template<Function>,
			Template<BlockDeclaration>,*/
			Alias
		>
	>,
	Token<END>
>;
