#pragma once
#include <vector>
#include <string>
#include <variant>
#include <optional>
#include <compare>

#include "primitives.hpp"
#include "../utility/box.hpp"
#include "../utility/overload.hpp"

struct NodeStructs {
	struct TemplatedTypename;
	struct NamespacedTypename;
	struct BaseTypename;
	struct UnionTypename;
	struct Typename;

	struct TemplatedTypename {
		Box<Typename> type;
		std::vector<Typename> templated_with;
		std::weak_ordering operator<=>(const TemplatedTypename&) const = default;
	};

	struct NamespacedTypename {
		Box<Typename> name_space;
		std::string name_in_name_space;
		std::weak_ordering operator<=>(const NamespacedTypename&) const = default;
	};

	struct BaseTypename {
		std::string type;
		std::weak_ordering operator<=>(const BaseTypename& other) const = default;
	};

	struct UnionTypename {
		std::vector<Typename> ors;
		std::weak_ordering operator<=>(const UnionTypename& other) const = default;
	};

	struct Typename {
		std::variant<TemplatedTypename, NamespacedTypename, BaseTypename, UnionTypename> value;
		std::weak_ordering operator<=>(const Typename& other) const; // = default; causes internal compiler error 
	};

	struct Alias {
		std::string aliasFrom;
		Typename aliasTo;
		std::weak_ordering operator<=>(const Alias&) const = default;
	};

	struct Statement;

	struct ConditionalExpression;
	struct OrExpression;
	struct AndExpression;
	struct EqualityExpression;
	struct CompareExpression;
	struct AdditiveExpression;
	struct MultiplicativeExpression;
	struct UnaryExpression;
	struct CallExpression;
	struct NamespaceExpression;
	struct TemplateExpression;
	struct ConstructExpression;
	struct BracketAccessExpression;
	struct PropertyAccessAndCallExpression;
	struct PropertyAccessExpression;
	struct ParenArguments;
	struct BraceArguments;

	struct Expression {
		using vt = std::variant <
			ConditionalExpression,
			OrExpression,
			AndExpression,
			EqualityExpression,
			CompareExpression,
			AdditiveExpression,
			MultiplicativeExpression,
			UnaryExpression,
			CallExpression,
			NamespaceExpression,
			TemplateExpression,
			ConstructExpression,
			BracketAccessExpression,
			PropertyAccessAndCallExpression,
			PropertyAccessExpression,
			ParenArguments,
			BraceArguments,
			std::string, // variable name like abc
			Token<FLOATING_POINT_NUMBER>,
			Token<INTEGER_NUMBER>,
			Token<STRING> // string token like "abc"
		> ;
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
	/*struct Key {
		std::weak_ordering operator<=>(const Key&) const = default;
	};*/
	/*
	ref -> ref
	ref! -> ref!
	copy or move -> value
	move->value
	key->key
	*/
	using ArgumentCategory = std::variant<Reference, MutableReference, Copy, Move/*, Key*/>;
	using ParameterCategory = std::variant<Reference, MutableReference, Value/*, Key*/>;
	using ValueCategory = std::variant<Reference, MutableReference, Value>;
	struct FunctionArgument {
		std::optional<ArgumentCategory> category;
		Expression expr;
		std::weak_ordering operator<=>(const FunctionArgument&) const = default;
	};

	struct BracketArguments {
		std::vector<FunctionArgument> args;
		std::weak_ordering operator<=>(const BracketArguments&) const = default;
	};

	struct BraceArguments {
		std::vector<FunctionArgument> args;
		std::weak_ordering operator<=>(const BraceArguments&) const = default;
	};

	struct TemplateArguments {
		std::vector<Expression> args;
		std::weak_ordering operator<=>(const TemplateArguments&) const = default;
	};

	struct ParenArguments {
		std::vector<FunctionArgument> args;
		std::weak_ordering operator<=>(const ParenArguments&) const = default;
	};

	struct CallExpression {
		Expression operand;
		ParenArguments arguments;
		std::weak_ordering operator<=>(const CallExpression&) const = default;
	};

	struct NamespaceExpression {
		Expression name_space;
		std::string name_in_name_space;
		std::weak_ordering operator<=>(const NamespaceExpression&) const = default;
	};

	struct TemplateExpression {
		Expression operand;
		TemplateArguments arguments;
		std::weak_ordering operator<=>(const TemplateExpression&) const = default;
	};

	struct ConstructExpression {
		Typename operand;
		BraceArguments arguments;
		std::weak_ordering operator<=>(const ConstructExpression&) const = default;
	};

	struct BracketAccessExpression {
		Expression operand;
		BracketArguments arguments;
		std::weak_ordering operator<=>(const BracketAccessExpression&) const = default;
	};

	struct PropertyAccessAndCallExpression {
		Expression operand;
		std::string property_name;
		ParenArguments arguments;
		std::weak_ordering operator<=>(const PropertyAccessAndCallExpression&) const = default;
	};

	struct PropertyAccessExpression {
		Expression operand;
		std::string property_name;
		std::weak_ordering operator<=>(const PropertyAccessExpression&) const = default;
	};

	struct UnaryExpression {
		using op_types = std::variant<
			Token<DASH>,
			Token<NOT>
		>;
		std::vector<op_types> unary_operators;
		Expression expr;
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
			Token<LTQ>,
			Token<LTEQ>,
			Token<GTQ>,
			Token<GTEQ>
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

	struct MatchCase {
		std::vector<std::pair<Typename, std::string>> variable_declarations;
		std::vector<Statement> statements;
		std::weak_ordering operator<=>(const MatchCase&) const = default;
	};

	struct MatchStatement {
		std::vector<Expression> expressions;
		std::vector<MatchCase> cases;
		std::weak_ordering operator<=>(const MatchStatement&) const = default;
	};

	struct BreakStatement {
		std::optional<Expression> ifExpr;
		std::weak_ordering operator<=>(const BreakStatement&) const;
	};

	struct ReturnStatement {
		std::vector<FunctionArgument> returnExpr;
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
			BlockStatement,
			MatchStatement
		> statement;
		std::weak_ordering operator<=>(const Statement&) const;
	};

	struct MemberVariable {
		Typename type;
		std::string name;
		std::weak_ordering operator<=>(const MemberVariable&) const = default;
	};

	struct Import {
		std::string imported;
		std::weak_ordering operator<=>(const Import&) const = default;
	};

	struct FunctionParameter {
		Typename typename_;
		ParameterCategory category;
		std::string name;
		std::weak_ordering operator<=>(const FunctionParameter&) const = default;
	};

	struct Function {
		std::string name;
		std::optional<Typename> name_space;
		Typename returnType;
		std::vector<FunctionParameter> parameters;
		std::vector<Statement> statements;
		std::weak_ordering operator<=>(const Function&) const;
	};

	struct Type {
		std::string name;
		std::optional<Typename> name_space;
		std::vector<Alias> aliases;
		std::vector<MemberVariable> memberVariables;
		std::weak_ordering operator<=>(const Type&) const;
	};

	struct Interface {
		std::string name;
		std::optional<Typename> name_space;
		std::vector<Alias> aliases;
		std::vector<MemberVariable> memberVariables;
		std::weak_ordering operator<=>(const Interface&) const;
	};

	struct FunctionType {
		std::reference_wrapper<const Function> function;
		std::weak_ordering operator<=>(const FunctionType&) const;
	};

	struct InterfaceType {
		std::reference_wrapper<const Interface> interface;
		std::weak_ordering operator<=>(const InterfaceType&) const;
	};

	struct NameSpace;
	struct NamespaceType {
		std::reference_wrapper<const NameSpace> name_space;
		std::weak_ordering operator<=>(const NamespaceType&) const;
	};

	struct Template {
		std::string name;
		std::optional<Typename> name_space;
		std::vector<std::pair<std::string, std::optional<NodeStructs::Expression>>> parameters;
		std::string templated;
		std::weak_ordering operator<=>(const Template&) const;
	};

	struct TemplateType {
		std::string name;
		std::vector<const Template*> options;
		std::weak_ordering operator<=>(const TemplateType&) const = default;
	};

	struct MetaType;

	/*struct AggregateType {
		std::vector<std::pair<ArgumentCategory, MetaType>> arguments;
		std::weak_ordering operator<=>(const AggregateType&) const = default;
	};*/

	struct UnionType {
		std::vector<MetaType> arguments;
		std::weak_ordering operator<=>(const UnionType&) const = default;
	};

	struct Vector {
		std::weak_ordering operator<=>(const Vector&) const = default;
	};

	struct VectorType {
		Box<MetaType> value_type;
		std::weak_ordering operator<=>(const VectorType&) const = default;
	};

	struct Set {
		std::weak_ordering operator<=>(const Set&) const = default;
	};

	struct SetType {
		Box<MetaType> value_type;
		std::weak_ordering operator<=>(const SetType&) const = default;
	};

	struct Map {
		std::weak_ordering operator<=>(const Map&) const = default;
	};

	struct MapType {
		Box<MetaType> key_type;
		Box<MetaType> value_type;
		std::weak_ordering operator<=>(const MapType&) const = default;
	};

	/*struct BuiltInType {
		struct push_t {
			NodeStructs::VectorType container;
			std::weak_ordering operator<=>(const push_t&) const;
		};
		std::variant<push_t> builtin;
		std::weak_ordering operator<=>(const BuiltInType&) const;
	};*/

	// these types also hold their value for compile-time stuff
	struct PrimitiveType {
		std::variant<
			std::string,
			double,
			int,
			bool
		> value;
		std::weak_ordering operator<=>(const PrimitiveType&) const;
	};

	struct MetaType {
		std::variant<
			PrimitiveType, // ex. type(1)
			std::reference_wrapper<const Type>, // ex. type Dog -> type(Dog)

			FunctionType, // ex. Bool has_bone(...) -> type(has_bone)
			InterfaceType, // ex. interface Animal -> type(Animal)
			NamespaceType, // ex. namespace std -> type(std)
			UnionType, // ex. type A, type B -> type(A | B)
			TemplateType, // ex. template X -> type(X)

			Vector, // type(Vector)
			VectorType, // type(Vector<Int>), note that type(Vector<Int>{}) yields a NonPrimitiveType{VectorType}
			Set, // type(Set)
			SetType, // type(Set<Int>), note that type(Set<Int>{}) yields a NonPrimitiveType{SetType}
			Map, // type(Map)
			MapType // type(Map<Int, Int>), note that type(Map<Int,Int>{}) yields a NonPrimitiveType{MapType}
		> type;
		std::weak_ordering operator<=>(const MetaType& other) const;
	};

	struct ExpressionType {
		MetaType type;
		std::weak_ordering operator<=>(const ExpressionType&) const = default;
	};
	using ValueType = std::variant<PrimitiveType, ExpressionType>;
	using UniversalType = std::variant<PrimitiveType, ExpressionType, MetaType>;

	struct Block {
		std::string name;
		std::vector<Statement> statements;
		std::weak_ordering operator<=>(const Block&) const = default;
	};

	struct NameSpace {
		std::string name;
		std::optional<Typename> name_space;
		std::vector<Type> types;
		std::vector<Function> functions;
		std::vector<Interface> interfaces;
		std::vector<Template> templates;
		std::vector<Block> blocks;
		std::vector<Alias> aliases;
		std::vector<NameSpace> namespaces;
		std::weak_ordering operator<=>(const NameSpace&) const;
	};

	struct File {
		std::string filename;
		std::vector<Import> imports;
		std::vector<Type> types;
		std::vector<Function> functions;
		std::vector<Interface> interfaces;
		std::vector<Template> templates;
		std::vector<Block> blocks;
		std::vector<Alias> aliases;
		std::vector<NameSpace> namespaces;
		std::weak_ordering operator<=>(const File&) const = default;
	};
};

template <typename T>
static std::weak_ordering cmp(const T& a, const T& b) {
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
	else if constexpr (is_specialization<T, std::reference_wrapper>::value) {
		return cmp(a.get(), b.get());
	}
	else if constexpr (std::is_same_v<T, std::string>)
		return a <=> b;
	else if constexpr (std::is_same_v<T, double>) {
		std::partial_ordering partial = a <=> b;
		if (partial == partial.greater)
			return std::weak_ordering::greater;
		else if (partial == partial.less)
			return std::weak_ordering::less;
		else if (partial == partial.equivalent)
			return std::weak_ordering::equivalent;
		else
			throw;
	}
	else if constexpr (std::is_same_v<T, int>)
		return a <=> b;
	else
		return a <=> b;
}

inline std::weak_ordering NodeStructs::Expression::operator<=>(const NodeStructs::Expression& other) const {
	return cmp(expression.get(), other.expression.get());
}

inline std::weak_ordering NodeStructs::IfStatement::operator<=>(const NodeStructs::IfStatement& other) const {
	if (auto c = cmp(ifStatements, other.ifStatements); c != 0)
		return c;
	if (auto c = cmp(ifExpr, other.ifExpr); c != 0)
		return c;
	return cmp(elseExprStatements, other.elseExprStatements);
}

inline std::weak_ordering NodeStructs::Statement::operator<=>(const NodeStructs::Statement& other) const {
	return cmp(statement, other.statement);
}

inline std::weak_ordering NodeStructs::BreakStatement::operator<=>(const NodeStructs::BreakStatement& other) const {
	return cmp(ifExpr, other.ifExpr);
}

inline std::weak_ordering NodeStructs::ReturnStatement::operator<=>(const NodeStructs::ReturnStatement& other) const {
	if (auto c = cmp(ifExpr, other.ifExpr); c != 0)
		return c;
	return cmp(returnExpr, other.returnExpr);
}

inline std::weak_ordering NodeStructs::UnaryExpression::operator<=>(const NodeStructs::UnaryExpression& other) const {
	if (auto c = cmp(expr, other.expr); c != 0)
		return c;
	return cmp(unary_operators, other.unary_operators);
}

inline std::weak_ordering NodeStructs::Typename::operator<=>(const NodeStructs::Typename& other) const {
	return cmp(value, other.value);
}

inline std::weak_ordering NodeStructs::MetaType::operator<=>(const NodeStructs::MetaType& other) const {
	return cmp(type, other.type);
}

//inline std::weak_ordering NodeStructs::TypeType::operator<=>(const TypeType& other) const {
//	return cmp(type.get(), other.type.get());
//}

inline std::weak_ordering NodeStructs::FunctionType::operator<=>(const FunctionType& other) const {
	return cmp(function.get(), other.function.get());
}

inline std::weak_ordering NodeStructs::InterfaceType::operator<=>(const InterfaceType& other) const {
	return cmp(interface.get(), other.interface.get());
}

inline std::weak_ordering NodeStructs::NamespaceType::operator<=>(const NamespaceType& other) const {
	return cmp(name_space.get(), other.name_space.get());
}

inline std::weak_ordering NodeStructs::NameSpace::operator<=>(const NameSpace& other) const {
	if (auto c = cmp(name, other.name); c != 0)
		return c;
	if (auto c = cmp(name_space, other.name_space); c != 0)
		return c;
	return std::weak_ordering::equivalent;
}

inline std::weak_ordering NodeStructs::Type::operator<=>(const Type& other) const {
	if (auto c = cmp(name, other.name); c != 0)
		return c;
	if (auto c = cmp(name_space, other.name_space); c != 0)
		return c;
	if (auto c = cmp(aliases, other.aliases); c != 0)
		return c;
	if (auto c = cmp(memberVariables, other.memberVariables); c != 0)
		return c;
	return std::weak_ordering::equivalent;
}

inline std::weak_ordering NodeStructs::Function::operator<=>(const Function& other) const {
	if (auto c = cmp(name, other.name); c != 0)
		return c;
	if (auto c = cmp(name_space, other.name_space); c != 0)
		return c;
	if (auto c = cmp(parameters, other.parameters); c != 0)
		return c;
	if (auto c = cmp(returnType, other.returnType); c != 0)
		return c;
	if (auto c = cmp(statements, other.statements); c != 0)
		return c;
	return std::weak_ordering::equivalent;
}

inline std::weak_ordering NodeStructs::Interface::operator<=>(const Interface& other) const {
	if (auto c = cmp(name, other.name); c != 0)
		return c;
	if (auto c = cmp(name_space, other.name_space); c != 0)
		return c;
	if (auto c = cmp(aliases, other.aliases); c != 0)
		return c;
	if (auto c = cmp(memberVariables, other.memberVariables); c != 0)
		return c;
	return std::weak_ordering::equivalent;
}

inline std::weak_ordering NodeStructs::Template::operator<=>(const Template& other) const {
	if (auto c = cmp(name, other.name); c != 0)
		return c;
	if (auto c = cmp(name_space, other.name_space); c != 0)
		return c;
	if (auto c = cmp(parameters, other.parameters); c != 0)
		return c;
	if (auto c = cmp(templated, other.templated); c != 0)
		return c;
	return std::weak_ordering::equivalent;
}

inline std::weak_ordering NodeStructs::PrimitiveType::operator<=>(const PrimitiveType& other) const {
	return cmp(*this, other);
}