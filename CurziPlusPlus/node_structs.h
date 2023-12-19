#pragma once
#include <vector>
#include <string>
#include <variant>
#include <optional>

#include "primitives.h"
#include "box.h"

template<class... Ts> struct overload : Ts... { using Ts::operator()...; };
template<class... Ts> overload(Ts...) -> overload<Ts...>; // to help IDE

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

template <typename T>
std::weak_ordering cmp(const T& a, const T& b) {
	if constexpr (is_specialization<T, std::vector>::value) {
		if (auto size_cmp = cmp(a.size(), b.size()); size_cmp != 0)
			return size_cmp;

		for (size_t i = 0; i < a.size(); ++i)
			if (auto v = cmp(a.at(i), b.at(i)); v != 0)
				return v;

		return std::weak_ordering::equivalent;
	}
	else if constexpr (is_specialization<T, std::optional>::value) {
		return a.has_value() && b.has_value() ? cmp(a.value(), b.value()) : cmp(a.has_value(), b.has_value());
	}
	else if constexpr (is_specialization<T, std::variant>::value) {
		auto index_cmp = cmp(a.index(), b.index());
		if (index_cmp != 0)
			return index_cmp;
		return std::visit(
			[&](const auto& _a) {
				return cmp(_a, std::get<std::remove_cvref_t<decltype(_a)>>(b));
			},
			a
		);
	}
	else
		return a <=> b;
}

namespace NodeStructs {
	struct TemplatedTypename;
	struct NamespacedTypename;
	struct BaseTypename;
	struct UnionTypename;
	struct Typename;

	struct TemplatedTypename {
		Box<Typename> type;
		std::vector<Typename> templated_with;

		using Type = TemplatedTypename;
		std::weak_ordering operator<=>(const Type&) const = default;
	};

	struct NamespacedTypename {
		Box<Typename> name_space;
		Box<Typename> name_in_name_space;

		using Type = NamespacedTypename;
		std::weak_ordering operator<=>(const Type&) const = default;
	};

	struct BaseTypename {
		std::string type;

		using Type = BaseTypename;
		std::weak_ordering operator<=>(const Type& other) const {
			return type <=> other.type;
		}
	};

	struct UnionTypename {
		std::vector<Typename> ors;

		std::weak_ordering operator<=>(const UnionTypename& other) const = default;
	};

	struct Typename {
		std::variant<TemplatedTypename, NamespacedTypename, BaseTypename, UnionTypename> value;
		std::weak_ordering operator<=>(const Typename& other) const {
			return cmp(value, other.value);
		}
	};

	struct Alias {
		Typename aliasFrom;
		Typename aliasTo;
		std::weak_ordering operator<=>(const Alias&) const = default;
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
		Box<vt> expression;
		std::weak_ordering operator<=>(const Expression& other) const;
	};

	struct Reference {
		std::weak_ordering operator<=>(const Reference&) const = default;
	};
	struct MutableReference {
		std::weak_ordering operator<=>(const MutableReference&) const = default;
	};
	struct Copy {
		std::weak_ordering operator<=>(const Copy&) const = default;
	};
	struct Move {
		std::weak_ordering operator<=>(const Move&) const = default;
	};
	struct Value {
		std::weak_ordering operator<=>(const Value&) const = default;
	};
	struct Key {
		std::weak_ordering operator<=>(const Key&) const = default;
	};
	/*
	ref->ref
	ref!->ref!
	copy->value
	move->val
	?->key
	*/
	using ArgumentPassingType = std::variant<Reference, MutableReference, Copy, Move>;
	using ValueCategory = std::variant<Reference, MutableReference, Value, Key>;
	using FunctionArgument = std::tuple<std::optional<ArgumentPassingType>, Expression>;

	struct BracketArguments {
		std::vector<FunctionArgument> args;
		std::weak_ordering operator<=>(const BracketArguments&) const = default;
	};

	struct BraceArguments {
		std::vector<FunctionArgument> args;
		std::weak_ordering operator<=>(const BraceArguments&) const = default;
	};

	struct ParenArguments {
		std::vector<FunctionArgument> args;
		std::weak_ordering operator<=>(const ParenArguments&) const = default;
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
		std::weak_ordering operator<=>(const PostfixExpression&) const = default;
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
		std::weak_ordering operator<=>(const UnaryExpression&) const;
	};

	struct MultiplicativeExpression {
		Expression expr;
		using op_types = std::variant<
			Token<ASTERISK>,
			Token<SLASH>,
			Token<PERCENT>
		>;
		std::vector<std::pair<op_types, Expression>> muls;
		std::weak_ordering operator<=>(const MultiplicativeExpression&) const = default;
	};

	struct AdditiveExpression {
		Expression expr;
		using op_types = std::variant<
			Token<PLUS>,
			Token<DASH>
		>;
		std::vector<std::pair<op_types, Expression>> adds;
		std::weak_ordering operator<=>(const AdditiveExpression&) const = default;
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
		std::weak_ordering operator<=>(const CompareExpression&) const = default;
	};

	struct EqualityExpression {
		Expression expr;
		using op_types = std::variant<
			Token<EQUALEQUAL>,
			Token<NEQUAL>
		>;
		std::vector<std::pair<op_types, Expression>> equals;
		std::weak_ordering operator<=>(const EqualityExpression&) const = default;
	};

	struct AndExpression {
		Expression expr;
		std::vector<Expression> ands;
		std::weak_ordering operator<=>(const AndExpression&) const = default;
	};

	struct OrExpression {
		Expression expr;
		std::vector<Expression> ors;
		std::weak_ordering operator<=>(const OrExpression&) const = default;
	};

	struct ConditionalExpression {
		Expression expr;
		std::optional<std::pair<Expression, Expression>> ifElseExprs;
		std::weak_ordering operator<=>(const ConditionalExpression&) const = default;
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
		std::weak_ordering operator<=>(const AssignmentExpression&) const = default;
	};

	struct VariableDeclaration {
		Typename type;
		std::string name;
		std::weak_ordering operator<=>(const VariableDeclaration&) const = default;
	};

	struct VariableDeclarationStatement {
		Typename type;
		std::string name;
		Expression expr;
		std::weak_ordering operator<=>(const VariableDeclarationStatement&) const = default;
	};

	struct ForStatement {
		Expression collection;
		std::vector<std::variant<VariableDeclaration, std::string>> iterators;
		std::vector<Statement> statements;
		std::weak_ordering operator<=>(const ForStatement&) const = default;
	};

	struct IForStatement {
		std::string index;
		Expression collection;
		std::vector<std::variant<VariableDeclaration, std::string>> iterators;
		std::vector<Statement> statements;
		std::weak_ordering operator<=>(const IForStatement&) const = default;
	};

	struct IfStatement {
		Expression ifExpr;
		std::vector<Statement> ifStatements;
		std::optional<std::variant<Box<IfStatement>, std::vector<Statement>>> elseExprStatements;
		std::weak_ordering operator<=>(const IfStatement& other) const;
	};

	struct WhileStatement {
		Expression whileExpr;
		std::vector<Statement> statements;
		std::weak_ordering operator<=>(const WhileStatement&) const = default;
	};

	struct BreakStatement {
		std::optional<Expression> ifExpr;
		std::weak_ordering operator<=>(const BreakStatement&) const;
	};

	struct ReturnStatement {
		std::vector<Expression> returnExpr;
		std::optional<Expression> ifExpr;
		std::weak_ordering operator<=>(const ReturnStatement&) const;
	};

	struct BlockStatement {
		Typename parametrized_block;
		std::weak_ordering operator<=>(const BlockStatement&) const = default;
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
		std::weak_ordering operator<=>(const Statement&) const;
	};

	struct Constructor {
		std::vector<std::tuple<Typename, ValueCategory, std::string>> parameters;
		std::vector<Statement> statements;
		std::weak_ordering operator<=>(const Constructor&) const = default;
	};

	struct MemberVariable {
		std::string name;
		Typename type;
		std::weak_ordering operator<=>(const MemberVariable&) const = default;
	};

	struct Import {
		std::string imported;
		std::weak_ordering operator<=>(const Import&) const = default;
	};

	template <typename T>
	struct Template;

	struct TemplateArguments {
		std::vector<std::string> arguments;
		std::weak_ordering operator<=>(const TemplateArguments&) const = default;
	};

	template <typename T>
	struct Template {
		TemplateArguments arguments;
		T templated;

		std::weak_ordering operator<=>(const Template&) const = default;
	};

	struct Function {
		std::string name;
		Typename returnType;
		std::vector<std::tuple<Typename, ValueCategory, std::string>> parameters;
		std::vector<Statement> statements;
		std::weak_ordering operator<=>(const Function&) const = default;
	};

	struct Type;
	struct TypeTemplateInstance;
	struct TypeAggregate;
	struct TypeType; // String is a type, the type of String is a TypeType
	struct TypeUnion;
	struct TypeVariant;

	struct TypeTemplateInstance {
		const Template<Type>* type_template;
		std::vector<TypeVariant> template_arguments;

		using Type = TypeTemplateInstance;
		std::weak_ordering operator<=>(const Type&) const = default;
	};

	struct TypeAggregate {
		std::vector<TypeVariant> arguments;

		using Type = TypeAggregate;
		std::weak_ordering operator<=>(const Type&) const = default;
	};

	struct TypeType {
		Box<TypeVariant> represented_type;

		using Type = TypeType;
		std::weak_ordering operator<=>(const Type&) const = default;
	};

	struct TypeUnion {
		std::vector<TypeVariant> arguments;

		using Type = TypeUnion;
		std::weak_ordering operator<=>(const Type&) const = default;
	};

	struct TypeVariant {
		std::variant<const Type*, TypeTemplateInstance, TypeAggregate, TypeType, TypeUnion> value;
		std::weak_ordering operator<=>(const TypeVariant& other) const {
			return cmp(value, other.value);
		}
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

		std::weak_ordering operator<=>(const Type&) const = default;
	};

	struct Block {
		std::string name;
		std::vector<Statement> statements;
		std::weak_ordering operator<=>(const Block&) const = default;
	};

	struct File {
		std::string filename;
		std::vector<Import> imports;
		std::vector<Type> types;
		std::vector<Template<Type>> type_templates;
		std::vector<Function> functions;
		std::vector<Template<Function>> function_templates;
		std::vector<Block> blocks;
		std::weak_ordering operator<=>(const File&) const = default;
	};
}
