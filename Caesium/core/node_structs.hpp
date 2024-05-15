#pragma once
#include <vector>
#include <string>
#include "lib.hpp"
#include <optional>
#include <compare>
#include <map>

#include "primitives.hpp"
#include "../utility/box.hpp"
#include "../utility/overload.hpp"

template <typename... Ts> using Variant = caesium_lib::variant::type<Ts...>;

struct Namespace;

struct NodeStructs {
	struct TemplatedTypename;
	struct NamespacedTypename;
	struct BaseTypename;
	struct UnionTypename;
	struct Typename;

	struct TemplatedTypename {
		NonCopyableBox<Typename> type;
		std::vector<Typename> templated_with;
		std::weak_ordering operator<=>(const TemplatedTypename&) const;
	};

	struct NamespacedTypename {
		NonCopyableBox<Typename> name_space;
		std::string name_in_name_space;
		std::weak_ordering operator<=>(const NamespacedTypename&) const;
	};

	struct BaseTypename {
		std::string type;
		std::weak_ordering operator<=>(const BaseTypename& other) const;
	};

	struct UnionTypename {
		std::vector<Typename> ors;
		std::weak_ordering operator<=>(const UnionTypename& other) const;
	};

	struct Typename {
		Variant<TemplatedTypename, NamespacedTypename, BaseTypename, UnionTypename> value;
		std::weak_ordering operator<=>(const Typename& other) const;
	};

	struct Alias {
		std::string aliasFrom;
		Typename aliasTo;
		std::optional<Typename> name_space;
		std::weak_ordering operator<=>(const Alias&) const;
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
		using vt = Variant<
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
		NonCopyableBox<vt> expression;
		std::weak_ordering operator<=>(const Expression& other) const;
	};

	struct Reference {
		std::weak_ordering operator<=>(const Reference&) const;
	};
	struct MutableReference {
		std::weak_ordering operator<=>(const MutableReference&) const;
	};
	struct Move {
		std::weak_ordering operator<=>(const Move&) const;
	};
	struct Value {
		std::weak_ordering operator<=>(const Value&) const;
	};
	using ArgumentCategory = Variant<Reference, MutableReference, Move>;
	using ParameterCategory = Variant<Reference, MutableReference, Value>;
	using ValueCategory = Variant<Reference, MutableReference, Value>;
	struct FunctionArgument {
		std::optional<ArgumentCategory> category;
		Expression expr;
		std::weak_ordering operator<=>(const FunctionArgument&) const;
	};

	struct BracketArguments {
		std::vector<FunctionArgument> args;
		std::weak_ordering operator<=>(const BracketArguments&) const;
	};

	struct BraceArguments {
		std::vector<FunctionArgument> args;
		std::weak_ordering operator<=>(const BraceArguments&) const;
	};

	struct TemplateArguments {
		std::vector<Expression> args;
		std::weak_ordering operator<=>(const TemplateArguments&) const;
	};

	struct ParenArguments {
		std::vector<FunctionArgument> args;
		std::weak_ordering operator<=>(const ParenArguments&) const;
	};

	struct CallExpression {
		Expression operand;
		ParenArguments arguments;
		std::weak_ordering operator<=>(const CallExpression&) const;
	};

	struct NamespaceExpression {
		Expression name_space;
		std::string name_in_name_space;
		std::weak_ordering operator<=>(const NamespaceExpression&) const;
	};

	struct TemplateExpression {
		Expression operand;
		TemplateArguments arguments;
		std::weak_ordering operator<=>(const TemplateExpression&) const;
	};

	struct ConstructExpression {
		Typename operand;
		BraceArguments arguments;
		std::weak_ordering operator<=>(const ConstructExpression&) const;
	};

	struct BracketAccessExpression {
		Expression operand;
		BracketArguments arguments;
		std::weak_ordering operator<=>(const BracketAccessExpression&) const;
	};

	struct PropertyAccessAndCallExpression {
		Expression operand;
		std::string property_name;
		ParenArguments arguments;
		std::weak_ordering operator<=>(const PropertyAccessAndCallExpression&) const;
	};

	struct PropertyAccessExpression {
		Expression operand;
		std::string property_name;
		std::weak_ordering operator<=>(const PropertyAccessExpression&) const;
	};

	struct UnaryExpression {
		using op_types = Variant<
			Token<DASH>,
			Token<NOT>
		>;
		std::vector<op_types> unary_operators;
		Expression expr;
		std::weak_ordering operator<=>(const UnaryExpression&) const;
	};

	struct MultiplicativeExpression {
		Expression expr;
		using op_types = Variant<
			Token<ASTERISK>,
			Token<SLASH>,
			Token<PERCENT>
		>;
		std::vector<std::pair<op_types, Expression>> muls;
		std::weak_ordering operator<=>(const MultiplicativeExpression&) const;
	};

	struct AdditiveExpression {
		Expression expr;
		using op_types = Variant<
			Token<PLUS>,
			Token<DASH>
		>;
		std::vector<std::pair<op_types, Expression>> adds;
		std::weak_ordering operator<=>(const AdditiveExpression&) const;
	};

	struct CompareExpression {
		Expression expr;
		using op_types = Variant<
			Token<LTQ>,
			Token<LTEQ>,
			Token<GTQ>,
			Token<GTEQ>
		>;
		std::vector<std::pair<op_types, Expression>> comparisons;
		std::weak_ordering operator<=>(const CompareExpression&) const;
	};

	struct EqualityExpression {
		Expression expr;
		using op_types = Variant<
			Token<EQUALEQUAL>,
			Token<NEQUAL>
		>;
		std::vector<std::pair<op_types, Expression>> equals;
		std::weak_ordering operator<=>(const EqualityExpression&) const;
	};

	struct AndExpression {
		Expression expr;
		std::vector<Expression> ands;
		std::weak_ordering operator<=>(const AndExpression&) const;
	};

	struct OrExpression {
		Expression expr;
		std::vector<Expression> ors;
		std::weak_ordering operator<=>(const OrExpression&) const;
	};

	struct ConditionalExpression {
		Expression expr;
		std::optional<std::pair<Expression, Expression>> ifElseExprs;
		std::weak_ordering operator<=>(const ConditionalExpression&) const;
	};

	struct VariableDeclaration {
		Typename type;
		std::string name;
		std::weak_ordering operator<=>(const VariableDeclaration&) const;
	};

	struct VariableDeclarationStatement {
		Typename type;
		std::string name;
		Expression expr;
		std::weak_ordering operator<=>(const VariableDeclarationStatement&) const;
	};

	struct ForStatement {
		Expression collection;
		std::vector<Variant<VariableDeclaration, std::string>> iterators;
		std::vector<Statement> statements;
		std::weak_ordering operator<=>(const ForStatement&) const;
	};

	struct IForStatement {
		std::string index;
		Expression collection;
		std::vector<Variant<VariableDeclaration, std::string>> iterators;
		std::vector<Statement> statements;
		std::weak_ordering operator<=>(const IForStatement&) const;
	};

	struct IfStatement {
		Expression ifExpr;
		std::vector<Statement> ifStatements;
		std::optional<Variant<NonCopyableBox<IfStatement>, std::vector<Statement>>> elseExprStatements;
		std::weak_ordering operator<=>(const IfStatement& other) const;
	};

	struct WhileStatement {
		Expression whileExpr;
		std::vector<Statement> statements;
		std::weak_ordering operator<=>(const WhileStatement&) const;
	};

	struct MatchCase {
		std::vector<std::pair<Typename, std::string>> variable_declarations;
		std::vector<Statement> statements;
		std::weak_ordering operator<=>(const MatchCase&) const;
	};

	struct MatchStatement {
		std::vector<Expression> expressions;
		std::vector<MatchCase> cases;
		std::weak_ordering operator<=>(const MatchStatement&) const;
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

	struct SwitchStatement {
		std::weak_ordering operator<=>(const SwitchStatement&) const;
	};

	struct EqualStatement {
		Expression left;
		Expression right;
		std::weak_ordering operator<=>(const EqualStatement&) const;
	};

	struct BlockStatement {
		Typename parametrized_block;
		std::weak_ordering operator<=>(const BlockStatement&) const;
	};

	struct Statement {
		Variant<
			Expression,
			VariableDeclarationStatement,
			IfStatement,
			ForStatement,
			IForStatement,
			WhileStatement,
			BreakStatement,
			ReturnStatement,
			BlockStatement,
			MatchStatement,
			SwitchStatement,
			EqualStatement
		> statement;
		std::weak_ordering operator<=>(const Statement&) const;
	};

	struct MemberVariable {
		Typename type;
		std::string name;
		std::weak_ordering operator<=>(const MemberVariable&) const;
	};

	struct Import {
		std::string imported;
		std::weak_ordering operator<=>(const Import&) const;
	};

	struct FunctionParameter {
		Typename typename_;
		ParameterCategory category;
		std::string name;
		std::weak_ordering operator<=>(const FunctionParameter&) const;
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
		std::vector<MemberVariable> member_variables;
		std::weak_ordering operator<=>(const Type&) const;
	};

	struct Interface {
		std::string name;
		std::optional<Typename> name_space;
		std::vector<Alias> aliases;
		std::vector<MemberVariable> member_variables;
		std::weak_ordering operator<=>(const Interface&) const;
	};

	struct InterfaceType {
		std::reference_wrapper<const Interface> interface;
		std::weak_ordering operator<=>(const InterfaceType&) const;
	};

	struct NamespaceType {
		std::reference_wrapper<const Namespace> name_space;
		std::weak_ordering operator<=>(const NamespaceType&) const;
	};

	struct FunctionType {
		std::string name;
		std::reference_wrapper<const Namespace> name_space;
		std::weak_ordering operator<=>(const FunctionType&) const;
	};

	struct TemplateParameter {
		std::string name;
		std::weak_ordering operator<=>(const TemplateParameter&) const;
	};

	struct TemplateParameterWithDefaultValue {
		std::string name;
		NodeStructs::Expression value;
		std::weak_ordering operator<=>(const TemplateParameterWithDefaultValue&) const;
	};

	struct VariadicTemplateParameter {
		std::string name;
		std::weak_ordering operator<=>(const VariadicTemplateParameter&) const;
	};

	struct Template {
		std::string name;
		std::optional<Typename> name_space;
		std::vector<Variant<TemplateParameter, TemplateParameterWithDefaultValue, VariadicTemplateParameter>> parameters;
		std::string templated;
		std::weak_ordering operator<=>(const Template&) const;
	};

	struct Builtin {
		std::string name;
		std::weak_ordering operator<=>(const Builtin&) const;
	};

	struct TemplateType {
		std::string name;
		std::reference_wrapper<const Namespace> name_space;
		std::weak_ordering operator<=>(const TemplateType&) const;
	};

	struct MetaType;

	struct AggregateType {
		std::vector<std::pair<ArgumentCategory, MetaType>> arguments;
		std::weak_ordering operator<=>(const AggregateType&) const;
	};

	struct UnionType {
		std::vector<MetaType> arguments;
		std::weak_ordering operator<=>(const UnionType&) const;
	};

	struct Vector {
		std::weak_ordering operator<=>(const Vector&) const;
	};

	struct VectorType {
		NonCopyableBox<MetaType> value_type;
		std::weak_ordering operator<=>(const VectorType&) const;
	};

	struct Set {
		std::weak_ordering operator<=>(const Set&) const;
	};

	struct SetType {
		NonCopyableBox<MetaType> value_type;
		std::weak_ordering operator<=>(const SetType&) const;
	};

	struct Map {
		std::weak_ordering operator<=>(const Map&) const;
	};

	struct MapType {
		NonCopyableBox<MetaType> key_type;
		NonCopyableBox<MetaType> value_type;
		std::weak_ordering operator<=>(const MapType&) const;
	};

	struct void_t {
		std::weak_ordering operator<=>(const void_t&) const;
	};

	// these types also hold their value for compile-time stuff
	struct PrimitiveType {
		Variant<
			std::string,
			double,
			int,
			bool,
			void_t,
			char
		> value;
		std::weak_ordering operator<=>(const PrimitiveType&) const;
	};

	struct Enum {
		std::string name;
		std::vector<std::string> values;
		std::optional<Typename> name_space;

		std::weak_ordering operator<=>(const Enum&) const;
	};

	struct EnumType {
		std::reference_wrapper<const Enum> enum_;
		std::weak_ordering operator<=>(const EnumType&) const;
	};

	struct EnumValueType {
		std::reference_wrapper<const Enum> enum_;
		std::string value;
		std::weak_ordering operator<=>(const EnumValueType&) const;
	};

	struct MetaType {
		using vt = Variant<
			PrimitiveType, // ex. type(1)
			Type, // ex. type Dog -> type(Dog{})

			FunctionType, // ex. Bool has_bone(...) -> type(has_bone)
			InterfaceType, // ex. interface Animal -> type(Animal)
			NamespaceType, // ex. namespace std -> type(std)
			UnionType, // ex. type A, type B -> type(A | B)
			TemplateType, // ex. template X -> type(X)
			Builtin,
			EnumType,
			EnumValueType,
			AggregateType,

			Vector, // type(Vector)
			VectorType, // type(Vector<Int>)
			Set, // type(Set)
			SetType, // type(Set<Int>)
			Map, // type(Map)
			MapType // type(Map<Int, Int>)
		>;
		vt type;
		std::weak_ordering operator<=>(const MetaType& other) const;
	};

	struct ExpressionType {
		MetaType type;
		std::weak_ordering operator<=>(const ExpressionType&) const;
	};

	//using ValueType = Variant<PrimitiveType, ExpressionType>;
	//using UniversalType = Variant<PrimitiveType, ExpressionType, MetaType>;

	struct Block {
		std::string name;
		std::vector<Statement> statements;
		std::weak_ordering operator<=>(const Block&) const;
	};

	struct NameSpace {
		std::string name;
		std::optional<Typename> name_space;

		std::vector<Function> functions;
		std::vector<Function> functions_using_auto;

		std::vector<Type> types;
		std::vector<Interface> interfaces;

		std::vector<Template> templates;

		std::vector<Block> blocks;
		std::vector<Alias> aliases;
		std::vector<Enum> enums;

		std::vector<NameSpace> namespaces;

		std::weak_ordering operator<=>(const NameSpace&) const;
	};

	struct File {
		std::vector<Import> imports;
		NameSpace content;
		std::weak_ordering operator<=>(const File&) const;
	};
};

template <typename T>
static std::weak_ordering cmp(const T& a, const T& b) {
	if constexpr (is_specialization<T, std::vector>::value) {
		if (auto size_cmp = cmp(a.size(), b.size()); size_cmp != 0)
			return size_cmp;

		for (size_t i = 0; i < a.size(); ++i)
			if (auto v = cmp(a.at(i), b.at(i)); v != std::weak_ordering::equivalent)
				return v;

		return std::weak_ordering::equivalent;
	}
	else if constexpr (is_specialization<T, std::pair>::value) {
		if (auto first = cmp(a.first, b.first); first != 0)
			return first;
		return cmp(a.second, b.second);
	}
	else if constexpr (is_specialization<T, std::optional>::value) {
		return a.has_value() && b.has_value() ? cmp(a.value(), b.value()) : cmp(a.has_value(), b.has_value());
	}
	else if constexpr (is_specialization<T, caesium_lib::variant::type>::value) {
		return cmp(a._value, b._value);
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
	else if constexpr (is_specialization<T, caesium_lib::variant::type>::value) {
		return cmp(a._value, b._value);
	}
	else if constexpr (is_specialization<T, Box>::value) {
		return cmp(a.get(), b.get());
	}
	else if constexpr (is_specialization<T, NonCopyableBox>::value) {
		return cmp(a.get(), b.get());
	}
	else if constexpr (is_specialization<T, std::reference_wrapper>::value) {
		if constexpr (std::is_same_v<T, std::reference_wrapper<const Namespace>>) {
			return &a.get() <=> &b.get();
		}
		else {
			return cmp(a.get(), b.get());
		}
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
	else
		return a <=> b;
}

template <typename T>
auto cmp0(const T& x1, const T& x2) {
	return std::weak_ordering::equivalent;
}

template <typename T>
auto cmp1(const T& x1, const T& x2) {
	const auto& [m1] = x1;
	const auto& [n1] = x2;
	if (auto c = cmp(m1, n1); c != std::weak_ordering::equivalent)
		return c;
	return std::weak_ordering::equivalent;
}

template <typename T>
auto cmp2(const T& x1, const T& x2) {
	const auto& [m1, m2] = x1;
	const auto& [n1, n2] = x2;
	if (auto c = cmp(m1, n1); c != std::weak_ordering::equivalent)
		return c;
	if (auto c = cmp(m2, n2); c != std::weak_ordering::equivalent)
		return c;
	return std::weak_ordering::equivalent;
}

template <typename T>
auto cmp3(const T& x1, const T& x2) {
	const auto& [m1, m2, m3] = x1;
	const auto& [n1, n2, n3] = x2;
	if (auto c = cmp(m1, n1); c != std::weak_ordering::equivalent)
		return c;
	if (auto c = cmp(m2, n2); c != std::weak_ordering::equivalent)
		return c;
	if (auto c = cmp(m3, n3); c != std::weak_ordering::equivalent)
		return c;
	return std::weak_ordering::equivalent;
}

template <typename T>
auto cmp4(const T& x1, const T& x2) {
	const auto& [m1, m2, m3, m4] = x1;
	const auto& [n1, n2, n3, n4] = x2;
	if (auto c = cmp(m1, n1); c != std::weak_ordering::equivalent)
		return c;
	if (auto c = cmp(m2, n2); c != std::weak_ordering::equivalent)
		return c;
	if (auto c = cmp(m3, n3); c != std::weak_ordering::equivalent)
		return c;
	if (auto c = cmp(m4, n4); c != std::weak_ordering::equivalent)
		return c;
	return std::weak_ordering::equivalent;
}

template <typename T>
auto cmp5(const T& x1, const T& x2) {
	const auto& [m1, m2, m3, m4, m5] = x1;
	const auto& [n1, n2, n3, n4, n5] = x2;
	if (auto c = cmp(m1, n1); c != std::weak_ordering::equivalent)
		return c;
	if (auto c = cmp(m2, n2); c != std::weak_ordering::equivalent)
		return c;
	if (auto c = cmp(m3, n3); c != std::weak_ordering::equivalent)
		return c;
	if (auto c = cmp(m4, n4); c != std::weak_ordering::equivalent)
		return c;
	if (auto c = cmp(m5, n5); c != std::weak_ordering::equivalent)
		return c;
	return std::weak_ordering::equivalent;
}

template <typename T>
auto cmp11(const T& x1, const T& x2) {
	const auto& [m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11] = x1;
	const auto& [n1, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11] = x2;
	if (auto c = cmp(m1, n1); c != std::weak_ordering::equivalent)
		return c;
	if (auto c = cmp(m2, n2); c != std::weak_ordering::equivalent)
		return c;
	if (auto c = cmp(m3, n3); c != std::weak_ordering::equivalent)
		return c;
	if (auto c = cmp(m4, n4); c != std::weak_ordering::equivalent)
		return c;
	if (auto c = cmp(m5, n5); c != std::weak_ordering::equivalent)
		return c;
	if (auto c = cmp(m6, n6); c != std::weak_ordering::equivalent)
		return c;
	if (auto c = cmp(m7, n7); c != std::weak_ordering::equivalent)
		return c;
	if (auto c = cmp(m8, n8); c != std::weak_ordering::equivalent)
		return c;
	if (auto c = cmp(m9, n9); c != std::weak_ordering::equivalent)
		return c;
	if (auto c = cmp(m10, n10); c != std::weak_ordering::equivalent)
		return c;
	if (auto c = cmp(m11, n11); c != std::weak_ordering::equivalent)
		return c;
	return std::weak_ordering::equivalent;
}

#define CMP0(T) inline std::weak_ordering NodeStructs::T::operator<=>(const T& other) const { return cmp0(*this, other); }
#define CMP1(T) inline std::weak_ordering NodeStructs::T::operator<=>(const T& other) const { return cmp1(*this, other); }
#define CMP2(T) inline std::weak_ordering NodeStructs::T::operator<=>(const T& other) const { return cmp2(*this, other); }
#define CMP3(T) inline std::weak_ordering NodeStructs::T::operator<=>(const T& other) const { return cmp3(*this, other); }
#define CMP4(T) inline std::weak_ordering NodeStructs::T::operator<=>(const T& other) const { return cmp4(*this, other); }
#define CMP5(T) inline std::weak_ordering NodeStructs::T::operator<=>(const T& other) const { return cmp5(*this, other); }
#define CMP11(T) inline std::weak_ordering NodeStructs::T::operator<=>(const T& other) const { return cmp11(*this, other); }

CMP2(TemplatedTypename)
CMP2(NamespacedTypename)
CMP1(BaseTypename)
CMP1(UnionTypename)
CMP1(Typename)
CMP0(Reference)
CMP0(MutableReference)
CMP0(Move)
CMP0(Value)
CMP1(Statement)
CMP1(Expression)
CMP2(VariableDeclaration)
CMP3(VariableDeclarationStatement)
CMP3(IfStatement)
CMP3(ForStatement)
CMP4(IForStatement)
CMP2(WhileStatement)
CMP2(MatchCase)
CMP2(MatchStatement)
CMP1(BreakStatement)
CMP2(ReturnStatement)
CMP0(SwitchStatement)
CMP2(EqualStatement)
CMP1(BlockStatement)
CMP2(FunctionArgument)
CMP2(ConditionalExpression)
CMP2(OrExpression)
CMP2(AndExpression)
CMP2(EqualityExpression)
CMP2(CompareExpression)
CMP2(AdditiveExpression)
CMP2(MultiplicativeExpression)
CMP2(UnaryExpression)
CMP2(CallExpression)
CMP2(NamespaceExpression)
CMP2(TemplateExpression)
CMP2(ConstructExpression)
CMP2(BracketAccessExpression)
CMP3(PropertyAccessAndCallExpression)
CMP2(PropertyAccessExpression)
CMP1(ParenArguments)
CMP1(BraceArguments)
CMP1(BracketArguments)
CMP1(TemplateArguments)
CMP1(ExpressionType)
CMP4(Type)
CMP3(Alias)
CMP2(MemberVariable)
CMP2(FunctionType)
CMP1(InterfaceType)
CMP1(NamespaceType)
CMP1(UnionType)
CMP2(TemplateType)
CMP1(Builtin)
CMP1(EnumType)
CMP2(EnumValueType)
CMP1(AggregateType)
CMP0(Vector)
CMP0(Set)
CMP0(Map)
CMP1(VectorType)
CMP1(SetType)
CMP2(MapType)
CMP4(Interface)
CMP2(Block)
CMP4(Template)
CMP3(Enum)
CMP1(TemplateParameter)
CMP2(TemplateParameterWithDefaultValue)
CMP1(VariadicTemplateParameter)
CMP3(FunctionParameter)
CMP5(Function)
CMP11(NameSpace)
CMP2(File)
CMP1(Import)
CMP0(void_t)

inline std::weak_ordering NodeStructs::MetaType::operator<=>(const NodeStructs::MetaType& other) const {
	if (std::holds_alternative<NodeStructs::EnumType>(type._value) &&
		std::holds_alternative<NodeStructs::EnumValueType>(other.type._value)) {
		const auto& a = std::get<NodeStructs::EnumType>(type._value);
		const auto& b = std::get<NodeStructs::EnumValueType>(other.type._value);
		return cmp(a.enum_.get(), b.enum_.get());
	}
	if (std::holds_alternative<NodeStructs::EnumValueType>(type._value) &&
		std::holds_alternative<NodeStructs::EnumType>(other.type._value)) {
		const auto& a = std::get<NodeStructs::EnumValueType>(type._value);
		const auto& b = std::get<NodeStructs::EnumType>(other.type._value);
		return cmp(a.enum_.get(), b.enum_.get());
	}
	return cmp(type._value, other.type._value);
}

inline std::weak_ordering NodeStructs::PrimitiveType::operator<=>(const NodeStructs::PrimitiveType& other) const {
	const auto& a = value._value;
	const auto& b = other.value._value;
	return cmp(a.index(), b.index());
}

inline std::string copy(const std::string& str) {
	return str;
}

template <typename T>
std::optional<T> copy(const std::optional<T>& x) {
	if (x.has_value())
		return std::optional<T>{ copy(x.value()) };
	else
		return std::optional<T>{ std::nullopt };
}

template <typename... Ts>
Variant<Ts...> copy(const Variant<Ts...>& x) {
	return std::visit([](const auto& u) { return Variant<Ts...>{ copy(u) }; }, x._value);
}

template <typename T>
std::vector<T> copy(const std::vector<T>& vec) {
	std::vector<T> res;
	res.reserve(vec.size());
	for (const auto& e : vec)
		res.push_back(copy(e));
	return res;
}

template <typename K, typename V>
std::map<K, V> copy(const std::map<K, V>& m) {
	std::map<K, V> res;
	for (const auto& [k, v] : m)
		res.emplace(copy(k), copy(v));
	return res;
}

template <typename T>
NonCopyableBox<T> copy(const NonCopyableBox<T>& box) {
	return NonCopyableBox<T>{ copy(box.get()) };
}

inline auto copy(const NodeStructs::PrimitiveType& p) {
	return NodeStructs::PrimitiveType{ p.value._value };
}

template <typename T>
std::reference_wrapper<T> copy(const std::reference_wrapper<T>& t) {
	return { t.get() };
}

template <typename T, typename U>
std::pair<T, U> copy(const std::pair<T, U>& x) {
	return copy2(x);
}

template <int token>
Token<token> copy(const Token<token>& tk) {
	return tk;
}

template <typename T>
T copy0(const T& x) {
	return {};
}

template <typename T>
T copy1(const T& x) {
	const auto& [a] = x;
	return {
		copy(a),
	};
}

template <typename T>
T copy2(const T& x) {
	const auto& [a, b] = x;
	return {
		copy(a),
		copy(b),
	};
}

template <typename T>
T copy3(const T& x) {
	const auto& [a, b, c] = x;
	return {
		copy(a),
		copy(b),
		copy(c),
	};
}

template <typename T>
T copy4(const T& x) {
	const auto& [a, b, c, d] = x;
	return {
		copy(a),
		copy(b),
		copy(c),
		copy(d),
	};
}

template <typename T>
T copy5(const T& x) {
	const auto& [a, b, c, d, e] = x;
	return {
		copy(a),
		copy(b),
		copy(c),
		copy(d),
		copy(e),
	};
}

template <typename T>
T copy6(const T& x) {
	const auto& [a, b, c, d, e, f] = x;
	return {
		copy(a),
		copy(b),
		copy(c),
		copy(d),
		copy(e),
		copy(f),
	};
}

template <typename T>
T copy11(const T& x) {
	const auto& [a, b, c, d, e, f, g, h, i, j, k] = x;
	return {
		copy(a),
		copy(b),
		copy(c),
		copy(d),
		copy(e),
		copy(f),
		copy(g),
		copy(h),
		copy(i),
		copy(j),
		copy(k),
	};
}

#define COPY0(T) inline NodeStructs::T copy(const NodeStructs::T& tn) { return copy0(tn); }
#define COPY1(T) inline NodeStructs::T copy(const NodeStructs::T& tn) { return copy1(tn); }
#define COPY2(T) inline NodeStructs::T copy(const NodeStructs::T& tn) { return copy2(tn); }
#define COPY3(T) inline NodeStructs::T copy(const NodeStructs::T& tn) { return copy3(tn); }
#define COPY4(T) inline NodeStructs::T copy(const NodeStructs::T& tn) { return copy4(tn); }
#define COPY5(T) inline NodeStructs::T copy(const NodeStructs::T& tn) { return copy5(tn); }
#define COPY11(T) inline NodeStructs::T copy(const NodeStructs::T& tn) { return copy11(tn); }

COPY2(TemplatedTypename)
COPY2(NamespacedTypename)
COPY1(BaseTypename)
COPY1(UnionTypename)
COPY1(Typename)
COPY0(Reference)
COPY0(MutableReference)
COPY0(Move)
COPY0(Value)
COPY1(Statement)
COPY1(Expression)
COPY2(VariableDeclaration)
COPY3(VariableDeclarationStatement)
COPY3(IfStatement)
COPY3(ForStatement)
COPY4(IForStatement)
COPY2(WhileStatement)
COPY2(MatchCase)
COPY2(MatchStatement)
COPY1(BreakStatement)
COPY2(ReturnStatement)
COPY0(SwitchStatement)
COPY2(EqualStatement)
COPY1(BlockStatement)
COPY2(FunctionArgument)
COPY2(ConditionalExpression)
COPY2(OrExpression)
COPY2(AndExpression)
COPY2(EqualityExpression)
COPY2(CompareExpression)
COPY2(AdditiveExpression)
COPY2(MultiplicativeExpression)
COPY2(UnaryExpression)
COPY2(CallExpression)
COPY2(NamespaceExpression)
COPY2(TemplateExpression)
COPY2(ConstructExpression)
COPY2(BracketAccessExpression)
COPY3(PropertyAccessAndCallExpression)
COPY2(PropertyAccessExpression)
COPY1(ParenArguments)
COPY1(BraceArguments)
COPY1(BracketArguments)
COPY1(TemplateArguments)
COPY1(MetaType)
COPY4(Type)
COPY3(Alias)
COPY2(MemberVariable)
COPY2(FunctionType)
COPY1(InterfaceType)
COPY1(NamespaceType)
COPY1(UnionType)
COPY2(TemplateType)
COPY1(Builtin)
COPY1(EnumType)
COPY2(EnumValueType)
COPY1(AggregateType)
COPY0(Vector)
COPY0(Set)
COPY0(Map)
COPY1(VectorType)
COPY1(SetType)
COPY2(MapType)
COPY4(Interface)
COPY2(Block)
COPY4(Template)
COPY3(Enum)
COPY1(TemplateParameter)
COPY2(TemplateParameterWithDefaultValue)
COPY1(VariadicTemplateParameter)
COPY3(FunctionParameter)
COPY5(Function)
COPY11(NameSpace)
COPY2(File)
COPY1(Import)
