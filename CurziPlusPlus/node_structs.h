#pragma once
#include <vector>
#include <string>
#include <variant>
#include <optional>

#include "primitives.h"
#include "alloc.h"

template<class... Ts> struct overload : Ts... { using Ts::operator()...; };
template<class... Ts> overload(Ts...)->overload<Ts...>; // to help IDE

#define overload_default_error [&](const auto& e) {\
static_assert(\
	!sizeof(std::remove_cvref_t<decltype(e)>*),\
	"Overload set is missing support for a type held in the variant."\
	);\
/* requires P2741R3 user-generated static_assert messages
static_assert(
	false,
	std::format("Overload set is missing support for a type held in the variant. see {}.", std::source_location::current())
);*/\
}

namespace NodeStructs {
	struct TemplatedTypename;
	struct NamespacedTypename;
	struct BaseTypename;

	using Typename = std::variant<TemplatedTypename, NamespacedTypename, BaseTypename>;

	struct TemplatedTypename {
		Allocated<Typename> type;
		std::vector<Typename> templated_with;

		bool operator==(const TemplatedTypename&) const;
		bool operator==(const Typename& other) const;
	};

	struct NamespacedTypename {
		Allocated<Typename> name_space;
		Allocated<Typename> name_in_name_space;

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
	struct ParenArguments;
	struct BraceArguments;

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
			ParenArguments,
			BraceArguments,
			std::string,
			Token<NUMBER>,
			Token<STRING>
		>;
		Allocated<vt> expression;
	};

	struct Reference {};
	struct MutableReference {};
	struct Copy {};
	struct Move {};
	struct Value {};
	struct Key {};
	/*
	ref->ref
	ref!->ref!
	copy->value
	move->val
	?->key
	*/
	using ArgumentPassingType = std::variant<Reference, MutableReference, Copy,  Move      >;
	using ValueCategory       = std::variant<Reference, MutableReference,    Value,       Key>;
	using FunctionArgument = std::tuple<std::optional<ArgumentPassingType>, Expression>;

	struct BracketArguments {
		std::vector<FunctionArgument> args;
	};

	struct BraceArguments {
		std::vector<FunctionArgument> args;
	};

	struct ParenArguments {
		std::vector<FunctionArgument> args;
	};

	struct PostfixExpression {
		Expression expr;
		using op_types = std::variant<
			std::string, // property
			ParenArguments, // call
			BracketArguments, // access
			BraceArguments, // construct
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
		std::optional<std::variant<Allocated<IfStatement>, std::vector<Statement>>> elseExprStatements;
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
		std::vector<std::tuple<Typename, ValueCategory, std::string>> parameters;
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
		std::vector<std::string> arguments;
	};

	template <typename T>
	struct Template {
		TemplateArguments arguments;
		T templated;
	};

	struct Function {
		std::string name;
		NodeStructs::Typename returnType;
		std::vector<std::tuple<Typename, ValueCategory, std::string>> parameters;
		std::vector<Statement> statements;
	};
	
	struct Type;
	struct TypeTemplateInstance;
	struct Aggregate;
	struct TypeType; // String is a type, the type of String is a TypeType

	using TypeVariant = std::variant<const Type*, TypeTemplateInstance, Aggregate, TypeType>;

	struct TypeTemplateInstance {
		const NodeStructs::Template<NodeStructs::Type>* type_template;
		std::vector<TypeVariant> template_arguments;
	};

	struct Aggregate {
		std::vector<TypeVariant> arguments;
	};

	struct TypeType {
		Allocated<TypeVariant> represented_type;
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
		std::vector<NodeStructs::Template<NodeStructs::Type>> type_templates;
		std::vector<Function> functions;
		std::vector<NodeStructs::Template<NodeStructs::Function>> function_templates;
		std::vector<Block> blocks;
	};
}
