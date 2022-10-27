#pragma once
#include "primitives.h"

// forward declare recursive rules
struct Typename;
struct Statement;
using Expression = struct AssignmentExpression;

struct Import {
	And<ImportKW, Word, From, Word, Newline> value;
};
struct Using {
	And<UsingKW, Word, Equal, Typename> value;
};
struct ArgumentsSignature {
	CommaStar<And<Typename, Word>> value;
};
struct ColonIndentCodeBlock {
	And<Colon, Newline, Indent<Star<Statement>>> value;
};
struct Function {
	And<Typename, Word, Parenopen, ArgumentsSignature, Parenclose, ColonIndentCodeBlock> value;
};
struct ParenArguments {
	And<Parenopen, CommaStar<Expression>, Parenclose> value;
};
struct BracketArguments {
	And<Bracketopen, CommaStar<Expression>, Bracketclose> value;
};
struct NSTypename {
	And<Ns, Typename> value;
};
struct TemplateTypename {
	And<Lt, CommaStar<Typename>, Gt, Opt<NSTypename>> value;
};
struct Typename {
	And<Word, Opt<Or<NSTypename, TemplateTypename>>> value;
};
struct TemplateTypenameDeclaration {
	And<Word, Lt, CommaPlus<Or<TemplateTypenameDeclaration, Word>>, Gt> value;
};
struct PPPQualifier {
	Or<Public, Private, Protected> value;
};
struct ExpressionStatement {
	And<Expression, Newline> value;
};
struct ElseStatement {
	And<IndentToken, Else, ColonIndentCodeBlock> value;
};
struct IfStatement {
	And<If, Expression, ColonIndentCodeBlock, Opt<ElseStatement>> value;
};
struct ForStatement {
	And<
		For,
		CommaPlus<Or<And<Typename, Word>, Word>>,
		In,
		Expression,
		Opt<And<If, Expression>>,
		Opt<And<While, Expression>>,
		ColonIndentCodeBlock
	> value;
};
struct IForStatement {
	And<
		Ifor,
		And<Typename, Word, Comma>,
		CommaPlus<Or<And<Typename, Word>, Word>>,
		In,
		Expression,
		Opt<And<If, Expression>>,
		Opt<And<While, Expression>>,
		ColonIndentCodeBlock
	> value;
};
struct WhileStatement {
	And<While, Expression, ColonIndentCodeBlock> value;
};
struct ReturnStatement {
	And<
		Return,
		CommaStar<Expression>,
		Opt<And<
			If,
			Expression,
			Opt<And<
				Else,
				Expression
			>>
		>>,
		Newline
	> value;
};
struct BreakStatement {
	And<Break, Opt<And<If, Expression>>, Newline> value;
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
	And<Extends, MultipleInheritance> value;
};
struct ClassMemberQualifiers {
	And<Opt<PPPQualifier>, Opt<Static>> value;
};
struct Method {
	And<ClassMemberQualifiers, Typename, Word, Parenopen, ArgumentsSignature, Parenclose, ColonIndentCodeBlock> value;
};
struct MemberVariable {
	And<ClassMemberQualifiers, Typename, Word, Newline> value;
};
struct Constructor {
	And<ClassMemberQualifiers, Word, Parenopen, ArgumentsSignature, Parenclose, ColonIndentCodeBlock> value;
};
struct ClassElement {
	Or<Using, Method, MemberVariable, Constructor> value;
};
struct Class {
	And<
		ClassKW,
		Or<TemplateTypenameDeclaration, Word>,
		Opt<ClassInheritance>,
		Colon,
		Newline,
		Indent<Star<And<
            IndentToken,
            ClassElement
		>>>
    > value;
};
struct File {
	And<Star<Import>, Star<Or<Class, Function>>, End> value;
};
/*
EXPRESSIONS
*/
struct ParenExpression {
	Or<
		And<
			Parenopen,
			Expression,
			Parenclose
		>,
		Typename,
		Number
	> value;
};
struct PostfixExpression {
	And<
		ParenExpression,
		Star<Or<
            And<
                Or<
                    Dot,
                    Arrow
                >,
                Word
            >,
            ParenArguments,
            BracketArguments,
            Plusplus,
            Minusminus
		>>
    > value;
};
struct UnaryExpression {
	Or <
		And<
			Or<	// has to be recursive because of the type cast operator taking the same shape as a ParenExpression
				// so instead of Star<Or> ... ____ we refer to UnaryExpression inside the Or
				Not,
				PlusKW,
				Dash,
				Plusplus,
				Minusminus,
				Tilde,
				Asterisk,
				Ampersand,
				And< // type cast operator
					Parenopen,
					Typename,
					Parenclose
				>
			>,
			UnaryExpression // recursive
		>,
		PostfixExpression
	> value;
};
struct MultiplicativeExpression {
	And<UnaryExpression, Star<And<Or<Asterisk, Slash, Percent>, UnaryExpression>>> value;
};
struct AdditiveExpression {
	And<MultiplicativeExpression, Star<And<Or<PlusKW, Dash>, MultiplicativeExpression>>> value;
};
struct CompareExpression {
	And<AdditiveExpression, Star<And<Or<Lt, Lte, Gt, Gte>, AdditiveExpression>>> value;
};
struct EqualityExpression {
	And<CompareExpression, Star<And<Equalequal, CompareExpression>>> value;
};
struct AndExpression {
	And<EqualityExpression, Star<And<AndKW, EqualityExpression>>> value;
};
struct OrExpression {
	And<AndExpression, Star<And<OrKW, AndExpression>>> value;
};
struct ConditionalExpression {
	And<
		OrExpression,
		Opt<And<
			If,
			OrExpression,
			Opt<And<
				Else,
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
				Equal,
				Plusequal,
				Minusequal,
				Timesequal,
				Divequal,
				Modequal,
				Andequal,
				Orequal,
				Xorequal
			>,
			ConditionalExpression
		>>
	> value;
};
