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

	// tag types
	struct function_context  {}; constexpr inline function_context  copy(function_context)  { return {}; }
	struct type_context      {}; constexpr inline type_context      copy(type_context)      { return {}; }
	struct top_level_context {}; constexpr inline top_level_context copy(top_level_context) { return {}; }

	using Enum = And<Commit<Token<ENUM>>, Word, Token<COLON>, Newline, Star<Indent<And<IndentToken, Word, Newline>>>>;

	using Expression = ConditionalExpression;
	using Import = And<Commit<Token<IMPORT>>, Or<Word, String>, Newline>;
	using Alias = And<Commit<Token<USING>>, Word, Token<EQUAL>, Typename, Newline>;
	using ParameterCategory = Or<Token<VAL>, And<Token<REF>, Token<NOT>>, Token<REF>>;
	using ArgumentCategory = Or<Token<MOVE>, And<Token<REF>, Token<NOT>>, Token<REF>>;
	using FunctionParameter = And<Commit<Typename>, Word>;
	using FunctionParameters = CommaStar<FunctionParameter>;
	template <typename context> using ColonIndentCodeBlock = And<Token<COLON>, Newline, Indent<Star<Or<Newline, Expect<Statement<context>>>>>>;
	using Function = And<Typename, Word, Token<PARENOPEN>, FunctionParameters, Token<PARENCLOSE>, ColonIndentCodeBlock<function_context>>;
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

	template <typename context> struct ExpressionStatement : And<Expression, Newline> {};
	template <typename context> struct BlockDeclaration : And<Token<BLOCK>, ColonIndentCodeBlock<context>> {};
	template <typename context> struct BlockStatement : And<Token<BLOCK>, Typename> {};
	template <typename context> struct VariableDeclaration : And<Typename, Word> {};
	template <typename context> struct VariableDeclarationStatement : And<Typename, Word, Token<EQUAL>, Expression, Newline> {};

	template <typename context> struct ElseStatement; // we need to explicitly allow `else if <>:` otherwise using `else {ifstatement}` would require indentation
	template <typename context> struct IfStatement : And<Token<IF>, Expression, ColonIndentCodeBlock<context>, Opt<Alloc<ElseStatement<context>>>> {};
	
	template <> struct ElseStatement<function_context> : And<IndentToken, Token<ELSE>, Or<Alloc<IfStatement<function_context>>, ColonIndentCodeBlock<function_context>>> {};
	template <> struct ElseStatement<type_context> : And<IndentToken, Token<POUND>, Token<ELSE>, Or<Alloc<IfStatement<type_context>>, ColonIndentCodeBlock<type_context>>> {};
	template <> struct ElseStatement<top_level_context> : And<IndentToken, Token<POUND>, Token<ELSE>, Or<Alloc<IfStatement<top_level_context>>, ColonIndentCodeBlock<top_level_context>>> {};
	
	template <typename context> struct BreakStatement : And<Token<BREAK>, Opt<And<Token<IF>, Expression>>, Newline> {};

	template <typename context> struct ForStatement : And<
		Token<FOR>,
		CommaPlus<Or<VariableDeclaration<context>, Word>>,
		Token<IN>,
		Expression,
		Opt<And<Token<IF>, Expression>>,
		Opt<And<Token<WHILE>, Expression>>,
		ColonIndentCodeBlock<context>
	> {};

	template <typename context> struct IForStatement : And<
		Token<IFOR>,
		Word, // require a variable for the index
		Token<COMMA>,
		CommaPlus<Or<VariableDeclaration<context>, Word>>, // and at least 1 variable iterating
		Token<IN>,
		Expression,
		Opt<And<Token<IF>, Expression>>,
		Opt<And<Token<WHILE>, Expression>>,
		ColonIndentCodeBlock<context>
	> {};

	template <typename context> struct WhileStatement : And<Token<WHILE>, Expression, ColonIndentCodeBlock<context>> {};

	template <typename context> struct ReturnStatement : And<
		Token<RETURN>,
		CommaStar<FunctionArgument>,
		Opt<And<
			Token<IF>,
			Expression
		>>,
		Newline
	> {};

	template <typename context> struct MatchStatement : And<
		Token<MATCH>,
		CommaPlus<Expression>,
		Token<COLON>,
		Newline,
		Indent<Plus<And<
			IndentToken,
			CommaPlus<VariableDeclaration<context>>,
			ColonIndentCodeBlock<context>
		>>>
	> {};

	template <typename context> struct SwitchStatement : And<
		Token<SWITCH>,
		Expression,
		Token<COLON>,
		Newline,
		Indent<Star<And<
			IndentToken,
			Expression,
			ColonIndentCodeBlock<context>
		>>>
	> {};

	template <typename context> struct Assignment : And<Expression, Token<EQUAL>, Expression> {};

	template <typename context>
	using CompileTimeStatement = And<Token<POUND>, Or<
		VariableDeclarationStatement<context>,
		IfStatement<context>,
		ForStatement<context>,
		IForStatement<context>,
		WhileStatement<context>,
		BreakStatement<context>,
		BlockStatement<context>,
		MatchStatement<context>,
		SwitchStatement<context>,
		Assignment<context>
	>>;

	using RunTimeStatement = Or<
		ExpressionStatement<function_context>,
		VariableDeclarationStatement<function_context>,
		IfStatement<function_context>,
		ForStatement<function_context>,
		IForStatement<function_context>,
		WhileStatement<function_context>,
		BreakStatement<function_context>,
		ReturnStatement<function_context>,
		BlockStatement<function_context>,
		MatchStatement<function_context>,
		SwitchStatement<function_context>,
		Assignment<function_context>
	>;

	template <>
	struct Statement<function_context> : And<IndentToken, Or<CompileTimeStatement<function_context>, Or<RunTimeStatement>>> {};

	struct Type : And<
		Commit<Token<TYPE>>,
		Word,
		Token<COLON>,
		Newline,
		Indent<Star<Statement<type_context>>>
	> {};

	template <>
	struct Statement<type_context> : And<IndentToken, Or<CompileTimeStatement<type_context>, Or<Alias, MemberVariable>>> {};

	template <>
	struct Statement<top_level_context> : And<IndentToken, Or<CompileTimeStatement<type_context>, Or<Type>>> {};

	using Interface = And<
		Token<INTERFACE>,
		Word,
		Token<COLON>,
		Newline,
		Indent<Star<Statement<type_context>>>
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
		Newline,
		TemplateBody
	>;

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
		Newline,
		Indent<Star<Or<Newline, And<IndentToken, Named>>>>
	> {};

	using Exists = And<
		Token<EXISTS>,
		Token<COLON>,
		Newline,
		Indent<Star<Or<Newline, And<IndentToken, Named>>>>
	>;

	using File = And<
		Star<Import>,
		Star<Or<Newline, Named, Exists>>,
		Token<END>
	>;

	using _Type = And<Commit<Token<TYPE>>, Word, Token<COLON>, Newline, Indent<Star<Statement<type_context>>>>;

	template <typename T>
	constexpr std::string name_of_rule() {
	#define CASE(type, value) if constexpr (std::is_same_v<T, type>) return value; else
		CASE(_Type, "Type")
		CASE(Template, "Template")
		CASE(Enum, "Enum")
		CASE(Import, "Import")
		CASE(Alias, "Alias")
		CASE(ParenArguments, "ParenArguments")
		CASE(BraceArguments, "BraceArguments")
		CASE(BracketArguments, "BracketArguments")
		CASE(FunctionParameter, "FunctionParameter")
		CASE(Statement<function_context>, "Statement<function_context>")
		CASE(Statement<type_context>, "Statement<type_context>")
		CASE(Statement<top_level_context>, "Statement<top_level_context>")
	#undef CASE
		static_assert(!(sizeof(T*)), "missing `name_of_rule()` for T");
	}
}
