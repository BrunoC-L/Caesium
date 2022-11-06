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

//using Import = value_t<And<Token<IMPORT>, Or<Word, String>, Token<NEWLINE>>>;

struct Import {
	And<Token<IMPORT>, Or<Word, String>, Token<NEWLINE>> value;
};
struct Alias {
	And<Token<USING>, Word, Token<EQUAL>, Typename, Token<NEWLINE>> value;
};
struct ArgumentsSignature {
	CommaStar<And<Typename, Word>> value;
};
struct ColonIndentCodeBlock {
	And<Token<COLON>, Token<NEWLINE>, Indent<Star<Statement>>> value;
};
struct Function {
	And<Typename, Word, Token<PARENOPEN>, ArgumentsSignature, Token<PARENCLOSE>, ColonIndentCodeBlock> value;
};
struct ParenArguments {
	And<Token<PARENOPEN>, CommaStar<Expression>, Token<PARENCLOSE>> value;
};
struct BracketArguments {
	And<Token<BRACKETOPEN>, CommaStar<Expression>, Token<BRACKETCLOSE>> value;
};
struct NSTypename {
	And<Token<NS>, Typename> value;
};
struct TemplateTypename {
	And<Token<LT>, CommaStar<Typename>, Token<GT>, Opt<NSTypename>> value;
};
struct Typename {
	And<Word, Opt<Or<NSTypename, TemplateTypename>>> value;
};
struct TemplateTypenameDeclaration {
	And<Word, Token<LT>, CommaPlus<Or<TemplateTypenameDeclaration, Word>>, Token<GT>> value;
};
struct PPPQualifier {
	Or<Token<PUBLIC>, Token<PRIVATE>, Token<PROTECTED>> value;
};
struct ExpressionStatement {
	And<Expression, Newline> value;
};
struct ElseStatement {
	And<IndentToken, Token<ELSE>, ColonIndentCodeBlock> value;
};
struct IfStatement {
	And<Token<IF>, Expression, ColonIndentCodeBlock, Opt<ElseStatement>> value;
};
struct ForStatement {
	And<
		Token<FOR>,
		CommaPlus<Or<And<Typename, Word>, Word>>,
		Token<IN>,
		Expression,
		Opt<And<Token<IF>, Expression>>,
		Opt<And<Token<WHILE>, Expression>>,
		ColonIndentCodeBlock
	> value;
};
struct IForStatement {
	And<
		Token<IFOR>,
		And<Or<And<Typename, Word>, Word>, Token<COMMA>>, // require a variable for the index
		CommaPlus<Or<And<Typename, Word>, Word>>, // and at least 1 variable iterating
		Token<IN>,
		Expression,
		Opt<And<Token<IF>, Expression>>,
		Opt<And<Token<WHILE>, Expression>>,
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
		Opt<And<
			Token<IF>,
			Expression,
			Opt<And<
				Token<ELSE>,
				Expression
			>>
		>>,
		Newline
	> value;
};
struct BreakStatement {
	And<Token<BREAK>, Opt<And<Token<IF>, Expression>>, Newline> value;
};
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
struct MultipleInheritance {
	CommaPlus<Typename> value;
};
struct ClassInheritance {
	And<Token<EXTENDS>, MultipleInheritance> value;
};
struct ClassMemberQualifiers {
	And<Opt<PPPQualifier>, Opt<Token<STATIC>>> value;
};
struct Method {
	And<ClassMemberQualifiers, Typename, Word, Token<PARENOPEN>, ArgumentsSignature, Token<PARENCLOSE>, ColonIndentCodeBlock> value;
};
struct MemberVariable {
	And<ClassMemberQualifiers, Typename, Word, Newline> value;
};
struct Constructor {
	And<ClassMemberQualifiers, Word, Token<PARENOPEN>, ArgumentsSignature, Token<PARENCLOSE>, ColonIndentCodeBlock> value;
};
struct ClassElement {
	Or<Alias, Method, MemberVariable, Constructor> value;
};
struct Class {
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
    > value;
};
struct File {
	And<Star<Import>, Star<Or<Class, Function>>, Token<END>> value;
};
/*
EXPRESSIONS
*/
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
struct PostfixExpression {
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
    > value;
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
struct MultiplicativeExpression {
	And<UnaryExpression, Star<And<Or<Token<ASTERISK>, Token<SLASH>, Token<PERCENT>>, UnaryExpression>>> value;
};
struct AdditiveExpression {
	And<MultiplicativeExpression, Star<And<Or<Token<PLUS>, Token<DASH>>, MultiplicativeExpression>>> value;
};
struct CompareExpression {
	And<AdditiveExpression, Star<And<Or<Token<LT>, Token<LTE>, Token<GT>, Token<GTE>>, AdditiveExpression>>> value;
};
struct EqualityExpression {
	And<CompareExpression, Star<And<Token<EQUALEQUAL>, CompareExpression>>> value;
};
struct AndExpression {
	And<EqualityExpression, Star<And<Token<AND>, EqualityExpression>>> value;
};
struct OrExpression {
	And<AndExpression, Star<And<Token<OR>, AndExpression>>> value;
};
struct ConditionalExpression {
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
	> value;
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
