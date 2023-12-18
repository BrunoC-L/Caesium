#pragma once
#include <vector>
#include <string>
#include <variant>
#include <optional>

#include "primitives.h"
#include "alloc.h"

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
		std::strong_ordering operator<=>(const Type&) const;
	};

	struct NamespacedTypename {
		Box<Typename> name_space;
		Box<Typename> name_in_name_space;

		using Type = NamespacedTypename;
		std::strong_ordering operator<=>(const Type&) const;
	};

	struct BaseTypename {
		std::string type;

		using Type = BaseTypename;
		std::strong_ordering operator<=>(const Type& other) const {
			return type <=> other.type;
		}
	};

	struct UnionTypename {
		std::vector<Typename> ors;

		std::strong_ordering operator<=>(const UnionTypename& other) const;
	};

	struct Typename {
		std::variant<TemplatedTypename, NamespacedTypename, BaseTypename, UnionTypename> value;
		std::strong_ordering operator<=>(const Typename& other) const {
			auto index_cmp = value.index() <=> other.value.index();
			return index_cmp != 0 ?
				index_cmp :
				std::visit(
					[&](const auto& a) -> std::strong_ordering {
						return std::strong_ordering(a <=> std::get<std::remove_cvref_t<decltype(a)>>(other.value));
					},
					value
				);
		}
	};

	struct Alias {
		Typename aliasFrom;
		Typename aliasTo;
		std::strong_ordering operator<=>(const Alias&) const {
			throw;
		}
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
		std::strong_ordering operator<=>(const Expression&) const {
			throw;
		}
	};

	struct Reference {
		std::strong_ordering operator<=>(const Reference&) const {
			throw;
		}
	};
	struct MutableReference {
		std::strong_ordering operator<=>(const MutableReference&) const {
			throw;
		}
	};
	struct Copy {
		std::strong_ordering operator<=>(const Copy&) const {
			throw;
		}
	};
	struct Move {
		std::strong_ordering operator<=>(const Move&) const {
			throw;
		}
	};
	struct Value {
		std::strong_ordering operator<=>(const Value&) const {
			throw;
		}
	};
	struct Key {
		std::strong_ordering operator<=>(const Key&) const {
			throw;
		}
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
		std::strong_ordering operator<=>(const BracketArguments&) const {
			throw;
		}
	};

	struct BraceArguments {
		std::vector<FunctionArgument> args;
		std::strong_ordering operator<=>(const BraceArguments&) const {
			throw;
		}
	};

	struct ParenArguments {
		std::vector<FunctionArgument> args;
		std::strong_ordering operator<=>(const ParenArguments&) const {
			throw;
		}
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
		std::strong_ordering operator<=>(const PostfixExpression&) const {
			throw;
		}
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
		std::strong_ordering operator<=>(const UnaryExpression&) const {
			throw;
		}
	};

	struct MultiplicativeExpression {
		Expression expr;
		using op_types = std::variant<
			Token<ASTERISK>,
			Token<SLASH>,
			Token<PERCENT>
		>;
		std::vector<std::pair<op_types, Expression>> muls;
		std::strong_ordering operator<=>(const MultiplicativeExpression&) const {
			throw;
		}
	};

	struct AdditiveExpression {
		Expression expr;
		using op_types = std::variant<
			Token<PLUS>,
			Token<DASH>
		>;
		std::vector<std::pair<op_types, Expression>> adds;
		std::strong_ordering operator<=>(const AdditiveExpression&) const {
			throw;
		}
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
		std::strong_ordering operator<=>(const CompareExpression&) const {
			throw;
		}
	};

	struct EqualityExpression {
		Expression expr;
		using op_types = std::variant<
			Token<EQUALEQUAL>,
			Token<NEQUAL>
		>;
		std::vector<std::pair<op_types, Expression>> equals;
		std::strong_ordering operator<=>(const EqualityExpression&) const {
			throw;
		}
	};

	struct AndExpression {
		Expression expr;
		std::vector<Expression> ands;
		std::strong_ordering operator<=>(const AndExpression&) const {
			throw;
		}
	};

	struct OrExpression {
		Expression expr;
		std::vector<Expression> ors;
		std::strong_ordering operator<=>(const OrExpression&) const {
			throw;
		}
	};

	struct ConditionalExpression {
		Expression expr;
		std::optional<std::pair<Expression, Expression>> ifElseExprs;
		std::strong_ordering operator<=>(const ConditionalExpression&) const {
			throw;
		}
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
		std::strong_ordering operator<=>(const AssignmentExpression&) const {
			throw;
		}
	};

	struct VariableDeclaration {
		Typename type;
		std::string name;
		std::strong_ordering operator<=>(const VariableDeclaration&) const {
			throw;
		}
	};

	struct VariableDeclarationStatement {
		Typename type;
		std::string name;
		Expression expr;
		std::strong_ordering operator<=>(const VariableDeclarationStatement&) const {
			throw;
		}
	};

	struct ForStatement {
		Expression collection;
		std::vector<std::variant<VariableDeclaration, std::string>> iterators;
		std::vector<Statement> statements;
		std::strong_ordering operator<=>(const ForStatement&) const {
			throw;
		}
	};

	struct IForStatement {
		std::string index;
		Expression collection;
		std::vector<std::variant<VariableDeclaration, std::string>> iterators;
		std::vector<Statement> statements;
		std::strong_ordering operator<=>(const IForStatement&) const {
			throw;
		}
	};

	struct IfStatement {
		Expression ifExpr;
		std::vector<Statement> ifStatements;
		std::optional<std::variant<Box<IfStatement>, std::vector<Statement>>> elseExprStatements;
		std::strong_ordering operator<=>(const IfStatement&) const {
			throw;
		}
	};

	struct WhileStatement {
		Expression whileExpr;
		std::vector<Statement> statements;
		std::strong_ordering operator<=>(const WhileStatement&) const {
			throw;
		}
	};

	struct BreakStatement {
		std::optional<Expression> ifExpr;
		std::strong_ordering operator<=>(const BreakStatement&) const {
			throw;
		}
	};

	struct ReturnStatement {
		std::vector<Expression> returnExpr;
		std::optional<Expression> ifExpr;
		std::strong_ordering operator<=>(const ReturnStatement&) const {
			throw;
		}
	};

	struct BlockStatement {
		Typename parametrized_block;
		std::strong_ordering operator<=>(const BlockStatement&) const {
			throw;
		}
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
		std::strong_ordering operator<=>(const Statement&) const {
			throw;
		}
	};

	struct Constructor {
		std::vector<std::tuple<Typename, ValueCategory, std::string>> parameters;
		std::vector<Statement> statements;
		std::strong_ordering operator<=>(const Constructor&) const {
			throw;
		}
	};

	struct MemberVariable {
		std::string name;
		Typename type;
		std::strong_ordering operator<=>(const MemberVariable&) const {
			throw;
		}
	};

	struct Import {
		std::string imported;
		std::strong_ordering operator<=>(const Import&) const {
			throw;
		}
	};

	template <typename T>
	struct Template;

	struct TemplateArguments {
		std::vector<std::string> arguments;
		std::strong_ordering operator<=>(const TemplateArguments&) const {
			throw;
		}
	};

	template <typename T>
	struct Template {
		TemplateArguments arguments;
		T templated;

		std::strong_ordering operator<=>(const Template&) const {
			throw;
		}
	};

	struct Function {
		std::string name;
		Typename returnType;
		std::vector<std::tuple<Typename, ValueCategory, std::string>> parameters;
		std::vector<Statement> statements;
		std::strong_ordering operator<=>(const Function&) const {
			throw;
		}
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
		std::strong_ordering operator<=>(const Type&) const {
			throw;
		}
	};

	struct TypeAggregate {
		std::vector<TypeVariant> arguments;

		using Type = TypeAggregate;
		std::strong_ordering operator<=>(const Type&) const {
			throw;
		}
	};

	struct TypeType {
		Box<TypeVariant> represented_type;

		using Type = TypeType;
		std::strong_ordering operator<=>(const Type&) const {
			throw;
		}
	};

	struct TypeUnion {
		std::vector<TypeVariant> arguments;

		using Type = TypeUnion;
		std::strong_ordering operator<=>(const Type&) const {
			throw;
		}
	};

	struct TypeVariant {
		std::variant<const Type*, TypeTemplateInstance, TypeAggregate, TypeType, TypeUnion> value;
		std::strong_ordering operator<=>(const TypeVariant& other) const {
			auto index_cmp = value.index() <=> other.value.index();
			return index_cmp != 0 ?
				index_cmp :
				std::visit(
					[&](const auto& a) {
						return a <=> std::get<std::remove_cvref_t<decltype(a)>>(other.value);
					},
					value
				);
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

		std::strong_ordering operator<=>(const Type&) const {
			throw;
		}
	};

	struct Block {
		std::string name;
		std::vector<Statement> statements;
		std::strong_ordering operator<=>(const Block&) const {
			throw;
		}
	};

	struct File {
		std::string filename;
		std::vector<Import> imports;
		std::vector<Type> types;
		std::vector<Template<Type>> type_templates;
		std::vector<Function> functions;
		std::vector<Template<Function>> function_templates;
		std::vector<Block> blocks;
		std::strong_ordering operator<=>(const File&) const {
			throw;
		}
	};
}
