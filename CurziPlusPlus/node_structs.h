#pragma once
#include <vector>
#include <string>
#include <variant>
#include <optional>

#define NODE_CAST(T, E) std::dynamic_pointer_cast<T>(E)

template<class... Ts> struct overload : Ts... { using Ts::operator()...; };
template<class... Ts> overload(Ts...)->overload<Ts...>; // to help IDE

namespace NodeStructs {
	struct Typename;

	struct TemplateTypeExtension {
		std::vector<Typename> templateTypes;
	};

	struct NSTypeExtension {
		std::string NSTypename;
	};

	using TypenameExtension = std::variant<TemplateTypeExtension, NSTypeExtension>;

	struct Typename {
		std::string type;
		std::vector<TypenameExtension> extensions;
	};

	struct Alias {
		Typename aliasFrom;
		Typename aliasTo;
	};

	struct Statement;

	struct AssignmentExpression;
	using Expression = AssignmentExpression;

	struct ParenArguments {
		std::vector<Expression> args;
	};

	struct BracketArguments {
		std::vector<Expression> args;
	};

	struct ParenExpression {
		std::variant<std::unique_ptr<Expression>, Typename, Token<NUMBER>> expr;
	};

	struct PostfixExpression {
		ParenExpression expr;
		using op_types = std::variant<
			std::string, // property
			ParenArguments, // call
			BracketArguments, // access
			Token<PLUSPLUS>,
			Token<MINUSMINUS>
		>;
		std::vector<op_types> postfixes;
	};

	struct UnaryExpression {
		using op_types = std::variant<
			Token<NOT>,
			Token<PLUS>,
			Token<DASH>,
			Token<PLUSPLUS>,
			Token<MINUSMINUS>,
			Token<TILDE>,
			Token<ASTERISK>,
			Token<AMPERSAND>,
			Typename // type cast operator
		>;
		using op_and_unaryexpr = std::pair<op_types, std::unique_ptr<UnaryExpression>>;
		std::variant<op_and_unaryexpr, PostfixExpression> expr;
	};

	struct MultiplicativeExpression {
		UnaryExpression expr;
		using op_types = std::variant<
			Token<ASTERISK>,
			Token<SLASH>,
			Token<PERCENT>
		>;
		std::vector<std::pair<op_types, UnaryExpression>> muls;
	};

	struct AdditiveExpression {
		MultiplicativeExpression expr;
		using op_types = std::variant<
			Token<PLUS>,
			Token<DASH>
		>;
		std::vector<std::pair<op_types, MultiplicativeExpression>> adds;
	};

	struct CompareExpression {
		AdditiveExpression expr;
		using op_types = std::variant<
			Token<LT>,
			Token<LTE>,
			Token<GT>,
			Token<GTE>
		>;
		std::vector<std::pair<op_types, AdditiveExpression>> comparisons;
	};

	struct EqualityExpression {
		CompareExpression expr;
		std::vector<CompareExpression> equals;
	};

	struct AndExpression {
		EqualityExpression expr;
		std::vector<EqualityExpression> ands;
	};

	struct OrExpression {
		AndExpression expr;
		std::vector<AndExpression> ors;
	};

	struct ConditionalExpression {
		OrExpression expr;
		std::optional<OrExpression> ifExpr;
		std::optional<OrExpression> elseExpr;
	};

	struct AssignmentExpression {
		ConditionalExpression expr;
		using op_types = std::variant<
			Token<EQUAL>,
			Token<PLUSEQUAL>,
			Token<MINUSEQUAL>,
			Token<TIMESEQUAL>,
			Token<DIVEQUAL>,
			Token<MODEQUAL>,
			Token<ANDEQUAL>,
			Token<OREQUAL>,
			Token<XOREQUAL>
		>;
		std::vector<std::pair<op_types, ConditionalExpression>> assignments;
	};

	struct VariableDeclarationStatement {
		Typename type;
		std::string name;
	};

	struct ForStatement {
		NodeStructs::Expression collection;
		std::vector<std::variant<VariableDeclarationStatement, std::string>> iterators;
	};

	struct IForStatement {

	};

	struct IfStatement {
		Expression ifExpr;
		std::vector<Statement> ifStatements;
		std::variant<std::unique_ptr<IfStatement>, std::vector<Statement>> elseExprStatements;
	};

	struct WhileStatement {

	};

	struct BreakStatement {

	};

	struct ReturnStatement {

	};

	struct Statement {
		std::variant<
			Expression,
			VariableDeclarationStatement,
			IfStatement,
			ForStatement,
			IForStatement,
			WhileStatement,
			BreakStatement,
			ReturnStatement
		> statement;
	};

	struct Constructor {
		std::optional<TemplateDeclaration> templated;
		std::vector<std::pair<Typename, std::string>> parameters;
		std::vector<Statement> statements;
	};

	struct MemberVariable {
		std::string name;
		Typename type;
	};

	struct Import {
		std::string imported;
	};

	struct TemplateDeclaration {
		std::string name;
		std::vector<TemplateDeclaration> parameters;
	};

	struct Function {
		std::string name;
		std::optional<TemplateDeclaration> templated;
		NodeStructs::Typename returnType;
		std::vector<std::pair<Typename, std::string>> parameters;
		std::vector<Statement> statements;
	};

	struct Class {
		std::string name;
		std::optional<TemplateDeclaration> templated;
		std::vector<Typename> inheritances;
		std::vector<Alias> aliases;
		std::vector<Constructor> constructors;
		std::vector<Function> methods;
		std::vector<MemberVariable> memberVariables;

		template <typename T> std::vector<T>& get() {
			if constexpr (std::is_same_v<T, Alias>)
				return aliases;
			else if constexpr (std::is_same_v<T, Constructor>)
				return constructors;
			else if constexpr (std::is_same_v<T, Function>)
				return methods;
			else if constexpr (std::is_same_v<T, MemberVariable>)
				return memberVariables;
			else
				static_assert(!sizeof(T*), "T is not supported");
		}
	};

	struct File {
		std::vector<Import> imports;
		std::vector<Class> classes;
		std::vector<Function> functions;
	};
}
