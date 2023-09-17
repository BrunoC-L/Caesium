#pragma once
#include <vector>
#include <string>
#include <variant>
#include <optional>

#include "primitives.h"

#define NODE_CAST(T, E) std::dynamic_pointer_cast<T>(E)

template<class... Ts> struct overload : Ts... { using Ts::operator()...; };
template<class... Ts> overload(Ts...)->overload<Ts...>; // to help IDE

namespace NodeStructs {
	struct TemplatedTypename;
	struct NamespacedTypename;
	struct BaseTypename;

	using Typename = std::variant<TemplatedTypename, NamespacedTypename, BaseTypename>;

	struct TemplatedTypename {
		std::unique_ptr<Typename> type;
		std::vector<Typename> templated_with;

		/*TemplatedTypename(const TemplatedTypename& other);
		TemplatedTypename(std::unique_ptr<Typename>&& type, std::vector<Typename>&& templated_with);*/
		bool operator==(const TemplatedTypename&) const;
		bool operator==(const Typename& other) const;
	};

	struct NamespacedTypename {
		std::unique_ptr<Typename> name_space;
		std::unique_ptr<Typename> name_in_name_space;

		/*NamespacedTypename(const NamespacedTypename& other);
		NamespacedTypename(std::unique_ptr<Typename>&& name_space, std::unique_ptr<Typename>&& name_in_name_space);*/
		bool operator==(const NamespacedTypename&) const;
		bool operator==(const Typename& other) const;
	};

	struct BaseTypename {
		std::string type;

		bool operator==(const BaseTypename&) const;
		bool operator==(const Typename& other) const;
	};

	struct Alias {
		Typename aliasFrom;
		Typename aliasTo;
	};

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
	struct ParenExpression;
	struct BraceExpression;

	struct Expression {
		using vt = std::variant<
			AssignmentExpression,
			ConditionalExpression,
			OrExpression,
			AndExpression,
			EqualityExpression,
			CompareExpression,
			AdditiveExpression,
			MultiplicativeExpression,
			UnaryExpression,
			PostfixExpression,
			ParenExpression,
			BraceExpression,
			std::string,
			Token<NUMBER>
		>;
		std::unique_ptr<vt> expression;

		/*Expression(const Expression& other);
		Expression(std::unique_ptr<vt>&& expression) : expression(std::move(expression)) {};*/
	};

	struct BracketArguments {
		std::vector<Expression> args;
	};

	struct BraceExpression {
		std::vector<Expression> args;
	};

	struct ParenExpression {
		std::vector<Expression> args;
	};

	struct PostfixExpression {
		Expression expr;
		using op_types = std::variant<
			std::string, // property
			ParenExpression, // call
			BracketArguments, // access
			BraceExpression, // construct
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
		using op_and_unary_expr = std::pair<op_types, Expression>;
		//std::variant<op_and_unary_expr, Expression> expr;
		Expression expr;
		op_types unary_operator;
	};

	struct MultiplicativeExpression {
		Expression expr;
		using op_types = std::variant<
			Token<ASTERISK>,
			Token<SLASH>,
			Token<PERCENT>
		>;
		std::vector<std::pair<op_types, Expression>> muls;
	};

	struct AdditiveExpression {
		Expression expr;
		using op_types = std::variant<
			Token<PLUS>,
			Token<DASH>
		>;
		std::vector<std::pair<op_types, Expression>> adds;
	};

	struct CompareExpression {
		Expression expr;
		using op_types = std::variant<
			Token<LT>,
			Token<LTE>,
			Token<GT>,
			Token<GTE>
		>;
		std::vector<std::pair<op_types, Expression>> comparisons;
	};

	struct EqualityExpression {
		Expression expr;
		using op_types = std::variant<
			Token<EQUALEQUAL>,
			Token<NEQUAL>
		>;
		std::vector<std::pair<op_types, Expression>> equals;
	};

	struct AndExpression {
		Expression expr;
		std::vector<Expression> ands;
	};

	struct OrExpression {
		Expression expr;
		std::vector<Expression> ors;
	};

	struct ConditionalExpression {
		Expression expr;
		std::optional<std::pair<Expression, Expression>> ifElseExprs;
	};

	struct AssignmentExpression {
		Expression expr;
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
		std::vector<std::pair<op_types, Expression>> assignments;
	};

	struct VariableDeclaration {
		Typename type;
		std::string name;
	};

	struct VariableDeclarationStatement {
		Typename type;
		std::string name;
		Expression expr;
	};

	struct ForStatement {
		NodeStructs::Expression collection;
		std::vector<std::variant<VariableDeclaration, std::string>> iterators;
		std::vector<NodeStructs::Statement> statements;
	};

	struct IForStatement {
		std::string index;
		NodeStructs::Expression collection;
		std::vector<std::variant<VariableDeclaration, std::string>> iterators;
		std::vector<NodeStructs::Statement> statements;
	};

	struct IfStatement {
		Expression ifExpr;
		std::vector<Statement> ifStatements;
		std::optional<std::variant<std::unique_ptr<IfStatement>, std::vector<Statement>>> elseExprStatements;
	};

	struct WhileStatement {
		Expression whileExpr;
		std::vector<NodeStructs::Statement> statements;
	};

	struct BreakStatement {
		std::optional<Expression> ifExpr;
	};

	struct ReturnStatement {
		std::vector<Expression> returnExpr;
		std::optional<Expression> ifExpr;
	};

	struct BlockStatement {
		Typename parametrized_block;
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
			ReturnStatement,
			BlockStatement
		> statement;
	};

	struct Constructor {
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

	template <typename T>
	struct Template;

	struct TemplateArguments {
		std::vector<std::variant<std::string, Template<std::string>>> arguments;
	};

	template <typename T>
	struct Template {
		TemplateArguments arguments;
		T templated;
	};

	struct Function {
		std::string name;
		NodeStructs::Typename returnType;
		std::vector<std::pair<Typename, std::string>> parameters;
		std::vector<Statement> statements;
	};
	
	struct Type;
	struct TypeTemplateInstance;

	using TypeOrTypeTemplateInstance = std::variant<const NodeStructs::Type*, NodeStructs::TypeTemplateInstance>;

	struct TypeTemplateInstance {
		NodeStructs::Template<const NodeStructs::Type*> type_template;
		std::vector<TypeOrTypeTemplateInstance> template_arguments;
	};

	struct Type {
		std::string name;
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

	struct Block {
		std::string name;
		std::vector<NodeStructs::Statement> statements;
	};

	struct File {
		std::string filename;
		std::vector<Import> imports;
		std::vector<Type> types;
		//std::vector<NodeStructs::Template<const NodeStructs::Type*>> type_templates;
		std::vector<Function> functions;
		std::vector<Block> blocks;
	};
}
