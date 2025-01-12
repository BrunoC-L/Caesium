#pragma once
#include "primitives.hpp"

namespace grammar {
	// forward declare recursive rules
	struct Typename; // typenames contain typenames
	template <typename context>
	struct Statement; // statements contain statements
	struct ConditionalExpression; // expressions contain expressions
	struct FunctionArgument; // expressions use function arguments and function arguments use expressions

	// alias the most used tokens
	using Word = Token<WORD>;
	using String = Token<STRING>;
	using Newline = Token<NEWLINE>;

	using Enum = And<Commit<Token<ENUM>>, Word, Token<COLON>, Newline, Star<Indent<And<IndentToken, Word, Newline>>>>;

	using Expression = ConditionalExpression;
	using Import = And<Commit<Token<IMPORT>>, Or<Word, String>, Newline>;
	using Alias = And<Commit<Token<USING>>, Word, Token<EQUAL>, Typename, Newline>;
	using ParameterCategory = Or<Token<VAL>, And<Token<REF>, Token<NOT>>, Token<REF>>;
	using ArgumentCategory = Or<Token<MOVE>, And<Token<REF>, Token<NOT>>, Token<REF>>;
	using FunctionParameter = And<Commit<Typename>, Word>;
	using FunctionParameters = CommaStar<FunctionParameter>;
	struct function_context {};
	using ColonIndentCodeBlock = And<Token<COLON>, Newline, Indent<Star<Or<Token<NEWLINE>, Expect<Statement<function_context>>>>>>;
	using Function = And<Typename, Word, Token<PARENOPEN>, FunctionParameters, Token<PARENCLOSE>, ColonIndentCodeBlock>;
	using ParenArguments = And<Commit<Token<PARENOPEN>>, CommaStar<FunctionArgument>, Token<PARENCLOSE>>;
	using BraceArguments = And<Commit<Token<BRACEOPEN>>, CommaStar<FunctionArgument>, Token<BRACECLOSE>>;
	using BracketArguments = And<Commit<Token<BRACKETOPEN>>, CommaStar<FunctionArgument>, Token<BRACKETCLOSE>>;

	using TypenameOrExpression = Or<Typename, Expression>;

	using VariadicExpansionTypename = And<Word, Token<DOTS>>;
	using NamespaceTypenameExtension = And<Token<NS>, Word>;
	using TemplateTypenameExtension = And<Token<LT>, CommaStar<TypenameOrExpression>, Token<GT>>;
	using UnionTypenameExtension = And<Token<BITOR>, Alloc<Typename>>;
	using NonAutoTypename = And<
		Or<VariadicExpansionTypename, Word>,
		Star<Or<
			NamespaceTypenameExtension,
			TemplateTypenameExtension,
			UnionTypenameExtension,
		Token<QUESTION>
		>>
	>;

	struct Typename : And<
		Or<Token<AUTO>, NonAutoTypename>,
		Opt<ParameterCategory>,
		Opt<Token<QUESTION>>
	> {};

	using MemberVariable = And<Typename, Word, Newline>;
	using TypeElement = Or<Alias, MemberVariable>;

	using Construct = And<Typename, BraceArguments>;

	using ParenExpression = Or<
		Construct,
		Word,
		Token<FLOATING_POINT_NUMBER>,
		Token<INTEGER_NUMBER>,
		Token<STRING>,
		ParenArguments,
		BracketArguments,
		BraceArguments
	>;
	using Postfix = Or<
		And<
			Token<DOT>,
			Word,
			ParenArguments
		>,
		And<
			Token<DOT>,
			Word
		>,
		ParenArguments,
		BracketArguments,
		TemplateTypenameExtension,
		And<Token<NS>, Word>
		//BraceArguments,
		/*,
		Token<PLUSPLUS>,
		Token<MINUSMINUS>*/
	>;
	using PostfixExpression = And<
		ParenExpression,
		Star<Postfix>
	>;

	using unary_operators = Or<Token<DASH>, Token<NOT>/*, Token<TILDE>, Token<ASTERISK>, Token<AMPERSAND>*/>;
	using UnaryExpression = Alloc<And<Star<unary_operators>, PostfixExpression>>;
	using MultiplicativeExpression = And<UnaryExpression, Star<And<Or<Token<ASTERISK>, Token<SLASH>, Token<PERCENT>>, UnaryExpression>>>;
	using AdditiveExpression = And<MultiplicativeExpression, Star<And<Or<Token<PLUS>, Token<DASH>>, MultiplicativeExpression>>>;
	using CompareOperator = And<Or<Token<LT>, Token<LTE>, Token<GT>, Token<GTE>>, Token<QUESTION>>;
	using CompareExpression = And<AdditiveExpression, Star<And<CompareOperator, AdditiveExpression>>>;
	using EqualityExpression = And<CompareExpression, Star<And<Or<Token<EQUALEQUAL>, Token<NEQUAL>>, CompareExpression>>>;
	using AndExpression = And<EqualityExpression, Star<And<Token<AND>, EqualityExpression>>>;
	using OrExpression = And<AndExpression, Star<And<Token<OR>, AndExpression>>>;
	struct ConditionalExpression : And<
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
	struct ElseStatement : And<IndentToken, Token<ELSE>, Or<Alloc<IfStatement>, ColonIndentCodeBlock>> {};

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
	using MatchStatement = And<
		Token<MATCH>,
		CommaPlus<Expression>,
		Token<COLON>,
		Newline,
		Indent<Plus<And<
			IndentToken,
			CommaPlus<VariableDeclaration>,
			ColonIndentCodeBlock
		>>>
	>;
	using SwitchStatement = And<
		Token<SWITCH>,
		Expression,
		Token<COLON>,
		Newline,
		Indent<Star<And<
			IndentToken,
			Expression,
			ColonIndentCodeBlock
		>>>
	>;

	using Assignment = And<Expression, Token<EQUAL>, Expression>;
	using StatementOpts = Alloc<Or<
		VariableDeclarationStatement,
		ExpressionStatement,
		IfStatement,
		ForStatement,
		IForStatement,
		WhileStatement,
		BreakStatement,
		ReturnStatement,
		BlockStatement,
		MatchStatement,
		SwitchStatement,
		Assignment
	>>;
	struct type_context {};
	struct top_level_context {};
	template <> struct Statement<function_context> : And<IndentToken, Opt<Token<POUND>>, StatementOpts> {};
	template <> struct Statement<type_context> : And<IndentToken, Token<POUND>, StatementOpts> {};
	template <> struct Statement<top_level_context> : And<IndentToken, Token<POUND>, StatementOpts> {};

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
		Commit<Token<TYPE>>,
		Word,
		Token<COLON>,
		Newline,
		Indent<Star<And<
			IndentToken,
			TypeElement
		>>>
	>;

	using Template = And<
		Commit<Token<TEMPLATE>>,
		Word,
		Token<LT>,
		CommaStar<Or<
			And<
				Word,
				Token<DOTS>
			>,
			And<
				Word,
				Opt<And<
					Token<EQUAL>,
					TypenameOrExpression
				>>
			>
		>>,
		Token<GT>,
		Token<COLON>,
		Token<NEWLINE>,
		TemplateBody>;

	struct NameSpace;

	using Named = Or<
		NameSpace,
		Type,
		Function,
		Interface,
		Template,
		Alias,
		Enum
	>;

	struct NameSpace : And<
		Word,
		Token<COLON>,
		Token<NEWLINE>,
		Indent<Star<Or<Token<NEWLINE>, And<IndentToken, Named>>>>
	> {};

	using Exists = And<
		Token<EXISTS>,
		Token<COLON>,
		Token<NEWLINE>,
		Indent<Star<Or<Token<NEWLINE>, And<IndentToken, Named>>>>
	>;

	using File = And<
		Star<Import>,
		Star<Or<Token<NEWLINE>, Named, Exists>>,
		Token<END>
	>;

	template <typename T>
	constexpr std::string name_of_rule() {
	#define CASE(type, value)  if constexpr (std::is_same_v<T, type>) return value; else
	CASE(Type, "Type")
	CASE(Template, "Template")
	CASE(Enum, "Enum")
	CASE(Import, "Import")
	CASE(Alias, "Alias")
	CASE(ParenArguments, "ParenArguments")
	CASE(BraceArguments, "BraceArguments")
	CASE(BracketArguments, "BracketArguments")
	CASE(FunctionParameter, "FunctionParameter")
	CASE(Statement<function_context>, "Statement")
	if constexpr (std::is_same_v<T, And<Commit<IndentToken>, Alloc<Or<VariableDeclarationStatement, ExpressionStatement, IfStatement, ForStatement, IForStatement, WhileStatement, BreakStatement, ReturnStatement, BlockStatement, MatchStatement, SwitchStatement, Assignment>>>>) return "Statement"; else
#undef CASE
	static_assert(!(sizeof(T*)), "missing name for T");
	}
}
