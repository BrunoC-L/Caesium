#pragma once
#include "primitives.h"

struct Typename;
struct NSTypename;
struct TemplateTypename;
struct ArgumentsSignature;
struct ColonIndentCodeBlock;
struct Class;
struct Statement;

struct AssignmentExpression;
struct ConditionalExpression;
struct OrExpression;
struct AndExpression;
struct EqualityExpression;
struct CompareExpression;
struct AdditiveExpression;
struct MultiplicativeExpression;
struct UnaryExpression;
struct PostfixExpression;
struct PostfixExpression;
struct ParenExpression;

using Expression = AssignmentExpression;
struct Import {
	And<Token<IMPORT>, Token<WORD>, Token<FROM>, Token<WORD>, Token<NEWLINE>> value;
};
struct Using {
	And<Token<USING>, Token<WORD>, Token<EQUAL>, Typename> value;
};
struct Function {
	And<Typename, Token<WORD>, Token<PARENOPEN>, ArgumentsSignature, Token<PARENCLOSE>, ColonIndentCodeBlock> value;
};
struct File {
	And<Star<Import>, Star<Or<Class, Function>>, Token<END>> value;
};
struct ArgumentsSignature {
	CommaStar<And<Typename, Token<WORD>>> value;
};

struct InnerArguments {
	CommaStar<Expression> value;
};

template <class Beg, class End>
struct ContainedArguments {
	And<Beg, InnerArguments, End> value;
};
using ParenArguments   = ContainedArguments<Token<PARENOPEN  >, Token<PARENCLOSE  >>;
using BracketArguments = ContainedArguments<Token<BRACKETOPEN>, Token<BRACKETCLOSE>>;
using BraceArguments   = ContainedArguments<Token<BRACEOPEN  >, Token<BRACECLOSE  >>;


struct Typename {
	And<Token<WORD>, OPT<Or<NSTypename, TemplateTypename>>> value;
};

struct NSTypename {
	And<Token<NS>, Typename> value;
};

struct TypenameList {
	CommaStar<Typename> value;
};

struct TemplateTypenameDeclaration {
	And<Token<WORD>, Token<LT>, CommaPlus<Or<TemplateTypenameDeclaration, Token<WORD>>>, Token<GT>> value;
};

struct TemplateTypename {
	And<Token<LT>, TypenameList, Token<GT>, OPT<NSTypename>> value;
};
struct PPPQualifier {
	Or<Token<PUBLIC>, Token<PRIVATE>, Token<PROTECTED>> value;
};

struct CodeBlock {
	Star<Statement> value;
};

struct ColonIndentCodeBlock {
	And<Token<COLON>, Token<NEWLINE>, Indent<CodeBlock>> value;
};

struct ExpressionStatement {
	And<Expression, Token<NEWLINE>> value;
};

struct ElseStatement {
	And<IndentToken, Token<ELSE>, ColonIndentCodeBlock> value;
};

struct IfStatement {
	And<Token<IF>, Expression, ColonIndentCodeBlock, OPT<ElseStatement>> value;
};

struct ForStatement {
	And<
		Token<FOR>,
		CommaPlus<Or<And<Typename, Token<WORD>>,
		Token<WORD>>>,
		Token<IN>,
		Expression,
		OPT<And<Token<IF>, Expression>>,
		OPT<And<Token<WHILE>, Expression>>,
		ColonIndentCodeBlock
	> value;
};

struct IForStatement {
	And<
		Token<IFOR>,
		And<Typename, Token<WORD>, Token<COMMA>>,
		CommaPlus<Or<And<Typename, Token<WORD>>,
		Token<WORD>>>,
		Token<IN>,
		Expression,
		OPT<And<Token<IF>, Expression>>,
		OPT<And<Token<WHILE>, Expression>>,
		ColonIndentCodeBlock
	> value;
};

struct WhileStatement {
	And<Token<WHILE>, Expression, ColonIndentCodeBlock> value;
};
struct ReturnStatement {
	And<
		Token<RETURN>,
		CommaStar<Expression>,
		OPT<And<
			Token<IF>,
			Expression,
			OPT<And<
				Token<ELSE>,
				Expression
			>>
		>>,
		Token<NEWLINE>
	> value;
};

struct BreakStatement {
	And<Token<BREAK>, OPT<And<Token<IF>, Expression>>, Token<NEWLINE>> value;
};

struct Statement {
	And<
		IndentToken,
		Or<
			Expression,
			IfStatement,
			ForStatement,
			IForStatement,
			WhileStatement,
			ReturnStatement,
			BreakStatement
		>
	> value;
};
struct MultipleInheritance {
	CommaPlus<Typename> value;
};
struct ClassInheritance {
	And<Token<EXTENDS>, MultipleInheritance> value;
};

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

struct ConditionalExpression {
	And<
		OrExpression,
		OPT<And<
			Token<IF>,
			OrExpression,
			OPT<And<
				Token<ELSE>,
				OrExpression
			>>
		>>
	> value;
};

struct OrExpression {
	And<AndExpression, Star<And<Token<OR>, AndExpression>>> value;
};

struct AndExpression {
	And<EqualityExpression, Star<And<Token<AND>, EqualityExpression>>> value;
};

struct EqualityExpression {
	And<CompareExpression, Star<And<Token<EQUALEQUAL>, CompareExpression>>> value;
};

struct CompareExpression {
	And<AdditiveExpression, Star<And<Or<Token<LT>, Token<LTE>, Token<GT>, Token<GTE>>, AdditiveExpression>>> value;
};

struct AdditiveExpression {
	And<MultiplicativeExpression, Star<And<Or<Token<PLUS>, Token<DASH>>, MultiplicativeExpression>>> value;
};

struct MultiplicativeExpression {
	And<UnaryExpression, Star<And<Or<Token<ASTERISK>, Token<SLASH>, Token<PERCENT>>, UnaryExpression>>> value;
};

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

struct PostfixExpression {
	And<
		ParenExpression,
		Star<Or<
			And<
				Or<
					Token<DOT>,
					Token<ARROW>
				>,
				Token<WORD>
			>,
			ParenArguments,
			BracketArguments,
			Token<PLUSPLUS>,
			Token<MINUSMINUS>
		>>
	> value;
};

struct ParenExpression {
	Or<
		And<
			Token<PARENOPEN>,
			Expression,
			Token<PARENCLOSE>
		>,
		Typename,
		Token<NUMBER>
	> value;
};
struct ClassMemberQualifiers {
	And<OPT<PPPQualifier>, OPT<Token<STATIC>>> value;
};
struct Method {
	And<ClassMemberQualifiers, Typename, Token<WORD>, Token<PARENOPEN>, ArgumentsSignature, Token<PARENCLOSE>, ColonIndentCodeBlock> value;
};
struct MemberVariable{
	And<ClassMemberQualifiers, Typename, Token<WORD>, Token<NEWLINE>> value;
};
struct Constructor {
	And<ClassMemberQualifiers, Token<WORD>, Token<PARENOPEN>, ArgumentsSignature, Token<PARENCLOSE>, ColonIndentCodeBlock> value;
};
struct ClassElement {
	Or<Using, Method, MemberVariable, Constructor> value;
};
struct Class {
	And<
		Token<CLASS>,
		Or<TemplateTypenameDeclaration, Token<WORD>>,
		OPT<ClassInheritance>,
		Token<COLON>,
		Token<NEWLINE>,
		Indent<Star<And<
			IndentToken,
			ClassElement
		>>>
	> value;
};