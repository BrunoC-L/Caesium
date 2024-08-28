#pragma once
#include <vector>
#include <string>
#include "lib.hpp"
#include <optional>
#include <compare>
#include <map>

#include "parse.hpp"
#include "../utility/box.hpp"
#include "../utility/overload.hpp"

template <typename... Ts> using Variant = caesium_lib::variant::type<Ts...>;

struct Namespace;

struct NodeStructs {
	struct TemplatedTypename;
	struct NamespacedTypename;
	struct BaseTypename;
	struct VariadicExpansionTypename;
	struct TupleTypename;
	struct UnionTypename;
	struct Typename;

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

	struct MetaType;

	struct TemplatedTypename {
		NonCopyableBox<Typename> type;
		std::vector<Typename> templated_with;
	};

	struct NamespacedTypename {
		NonCopyableBox<Typename> name_space;
		std::string name_in_name_space;
	};

	struct BaseTypename {
		std::string type;
	};

	struct VariadicExpansionTypename {
		NonCopyableBox<Typename> type;
	};

	struct TupleTypename {
		std::vector<Typename> members;
	};

	struct UnionTypename {
		std::vector<Typename> ors;
	};

	/*struct TupleTypename {
	};*/

	struct OptionalTypename {
		NonCopyableBox<Typename> type;
	};

	struct Typename {
		Variant<TemplatedTypename, NamespacedTypename, BaseTypename, OptionalTypename, TupleTypename, UnionTypename, VariadicExpansionTypename> value;
	};

	struct Alias {
		std::string aliasFrom;
		Typename aliasTo;
		std::optional<Typename> name_space;
	};

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
	};

	struct Reference {
	};
	struct MutableReference {
	};
	struct Move {
	};
	struct Value {
	};
	using ArgumentCategory = Variant<Reference, MutableReference, Move>;
	using ParameterCategory = Variant<Reference, MutableReference, Value>;
	using ValueCategory = Variant<Reference, MutableReference, Value>;
	struct FunctionArgument {
		std::optional<ArgumentCategory> category;
		Expression expr;
	};

	struct BracketArguments {
		std::vector<FunctionArgument> args;
	};

	struct BraceArguments {
		std::vector<FunctionArgument> args;
	};

	struct TemplateArguments {
		std::vector<Expression> args;
	};

	struct ParenArguments {
		std::vector<FunctionArgument> args;
	};

	struct CallExpression {
		Expression operand;
		ParenArguments arguments;
	};

	struct NamespaceExpression {
		Expression name_space;
		std::string name_in_name_space;
	};

	struct TemplateExpression {
		Expression operand;
		TemplateArguments arguments;
	};

	struct ConstructExpression {
		Typename operand;
		BraceArguments arguments;
	};

	struct BracketAccessExpression {
		Expression operand;
		BracketArguments arguments;
	};

	struct PropertyAccessAndCallExpression {
		Expression operand;
		std::string property_name;
		ParenArguments arguments;
	};

	struct PropertyAccessExpression {
		Expression operand;
		std::string property_name;
	};

	struct UnaryExpression {
		using op_types = Variant<
			Token<DASH>,
			Token<NOT>
		>;
		std::vector<op_types> unary_operators;
		Expression expr;
	};

	struct MultiplicativeExpression {
		Expression expr;
		using op_types = Variant<
			Token<ASTERISK>,
			Token<SLASH>,
			Token<PERCENT>
		>;
		std::vector<std::pair<op_types, Expression>> muls;
	};

	struct AdditiveExpression {
		Expression expr;
		using op_types = Variant<
			Token<PLUS>,
			Token<DASH>
		>;
		std::vector<std::pair<op_types, Expression>> adds;
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
	};

	struct EqualityExpression {
		Expression expr;
		using op_types = Variant<
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
		Expression collection;
		std::vector<Variant<VariableDeclaration, std::string>> iterators;
		std::vector<Statement> statements;
	};

	struct IForStatement {
		std::string index;
		Expression collection;
		std::vector<Variant<VariableDeclaration, std::string>> iterators;
		std::vector<Statement> statements;
	};

	struct IfStatement {
		Expression ifExpr;
		std::vector<Statement> ifStatements;
		std::optional<Variant<NonCopyableBox<IfStatement>, std::vector<Statement>>> elseExprStatements;
	};

	struct WhileStatement {
		Expression whileExpr;
		std::vector<Statement> statements;
	};

	struct MatchCase {
		std::vector<std::pair<Typename, std::string>> variable_declarations;
		std::vector<Statement> statements;
	};

	struct MatchStatement {
		std::vector<Expression> expressions;
		std::vector<MatchCase> cases;
	};

	struct BreakStatement {
		std::optional<Expression> ifExpr;
	};

	struct ReturnStatement {
		std::vector<FunctionArgument> returnExpr;
		std::optional<Expression> ifExpr;
	};

	struct SwitchCase {
		Expression expr;
		std::vector<Statement> statements;
	};

	struct SwitchStatement {
		Expression expr;
		std::vector<SwitchCase> cases;
	};

	struct Assignment {
		Expression left;
		Expression right;
	};

	struct BlockStatement {
		Typename parametrized_block;
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
			Assignment
		> statement;
	};

	struct MemberVariable {
		Typename type;
		std::string name;
	};

	struct Import {
		std::string imported;
	};

	struct FunctionParameter {
		Typename typename_;
		ParameterCategory category;
		std::string name;
	};

	struct Function {
		std::string name;
		std::optional<Typename> name_space;
		Typename returnType;
		std::vector<FunctionParameter> parameters;
		std::vector<Statement> statements;
	};

	struct Type {
		std::string name;
		std::optional<Typename> name_space;
		std::vector<Alias> aliases;
		std::vector<MemberVariable> member_variables;
	};

	struct Interface {
		std::string name;
		std::optional<Typename> name_space;
		std::vector<Alias> aliases;
		std::vector<MemberVariable> member_variables;
	};

	struct InterfaceType {
		std::reference_wrapper<const Interface> interface;
	};

	struct NamespaceType {
		std::reference_wrapper<const Namespace> name_space;
	};

	struct FunctionType {
		std::string name;
		std::reference_wrapper<const Namespace> name_space;
	};

	struct TemplateParameter {
		std::string name;
	};

	struct TemplateParameterWithDefaultValue {
		std::string name;
		NodeStructs::Expression value;
	};

	struct VariadicTemplateParameter {
		std::string name;
	};

	struct Template {
		std::string name;
		std::optional<Typename> name_space;
		std::vector<Variant<TemplateParameter, TemplateParameterWithDefaultValue, VariadicTemplateParameter>> parameters;
		std::string templated;
		int indent;
	};

	struct Builtin {
		std::string name;
	};

	struct TemplateType {
		std::string name;
		std::reference_wrapper<const Namespace> name_space;
	};

	struct AggregateType {
		std::vector<FunctionArgument> arguments;
		std::vector<MetaType> arg_types;
	};

	struct UnionType {
		std::vector<MetaType> arguments;
	};

	struct TupleType {
		std::vector<MetaType> arguments;
	};

	struct Vector {
	};

	struct VectorType {
		NonCopyableBox<MetaType> value_type;
	};

	struct Set {
	};

	struct SetType {
		NonCopyableBox<MetaType> value_type;
	};

	struct Map {
	};

	struct MapType {
		NonCopyableBox<MetaType> key_type;
		NonCopyableBox<MetaType> value_type;
	};

	struct void_t {};
	struct empty_optional_t {};

	// these types also hold their value for compile-time stuff
	struct PrimitiveType {
		Variant<
			std::string,
			double,
			int,
			bool,
			void_t,
			char,
			empty_optional_t
		> value;
	};

	struct Enum {
		std::string name;
		std::vector<std::string> values;
		std::optional<Typename> name_space;

	};

	struct EnumType {
		std::reference_wrapper<const Enum> enum_;
	};

	struct EnumValueType {
		std::reference_wrapper<const Enum> enum_;
		std::string value;
	};

	struct OptionalType {
		NonCopyableBox<MetaType> value_type;
	};

	struct MetaType {
		using vt = Variant<
			PrimitiveType, // ex. type(1)
			Type, // ex. type Dog -> type(Dog{})

			FunctionType, // ex. Bool has_bone(...) -> type(has_bone)
			InterfaceType, // ex. interface Animal -> type(Animal)
			NamespaceType, // ex. namespace std -> type(std)
			TupleType, // A & B
			UnionType, // ex. type A, type B -> type(A | B)
			TemplateType, // ex. template X -> type(X)
			Builtin,
			EnumType, // ex. enum E -> type(E)
			EnumValueType, // ex enum E: A, B... -> type(E::A)
			OptionalType, // A?
			AggregateType, // type({A, B, C})

			Vector, // type(Vector)
			VectorType, // type(Vector<Int>)
			Set, // type(Set)
			SetType, // type(Set<Int>)
			Map, // type(Map)
			MapType // type(Map<Int, Int>)
		>;
		vt type;
	};

	struct Block {
		std::string name;
		std::vector<Statement> statements;
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

	};

	struct Exists {
		NameSpace global_exists;
	};

	struct File {
		std::vector<Import> imports;
		std::vector<Exists> exists;
		NameSpace content;
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

#define CMP0(T) inline std::weak_ordering operator<=>(const NodeStructs::T& left, const NodeStructs::T& right) { return cmp0(left, right); }
#define CMP1(T) inline std::weak_ordering operator<=>(const NodeStructs::T& left, const NodeStructs::T& right) { return cmp1(left, right); }
#define CMP2(T) inline std::weak_ordering operator<=>(const NodeStructs::T& left, const NodeStructs::T& right) { return cmp2(left, right); }
#define CMP3(T) inline std::weak_ordering operator<=>(const NodeStructs::T& left, const NodeStructs::T& right) { return cmp3(left, right); }
#define CMP4(T) inline std::weak_ordering operator<=>(const NodeStructs::T& left, const NodeStructs::T& right) { return cmp4(left, right); }
#define CMP5(T) inline std::weak_ordering operator<=>(const NodeStructs::T& left, const NodeStructs::T& right) { return cmp5(left, right); }
#define CMP6(T) inline std::weak_ordering operator<=>(const NodeStructs::T& left, const NodeStructs::T& right) { return cmp6(left, right); }
#define CMP11(T) inline std::weak_ordering operator<=>(const NodeStructs::T& left, const NodeStructs::T& right) { return cmp11(left, right); }

CMP2(TemplatedTypename)
CMP2(NamespacedTypename)
CMP1(BaseTypename)
CMP1(TupleTypename)
CMP1(UnionTypename)
CMP1(OptionalTypename)
CMP1(VariadicExpansionTypename)
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
CMP2(SwitchCase)
CMP2(SwitchStatement)
CMP2(Assignment)
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
CMP4(Type)
CMP3(Alias)
CMP2(MemberVariable)
CMP2(FunctionType)
CMP1(InterfaceType)
CMP1(NamespaceType)
CMP1(TupleType)
CMP1(UnionType)
CMP1(OptionalType)
CMP2(TemplateType)
CMP1(Builtin)
CMP1(EnumType)
CMP2(EnumValueType)
CMP2(AggregateType)
CMP0(Vector)
CMP0(Set)
CMP0(Map)
CMP1(VectorType)
CMP1(SetType)
CMP2(MapType)
CMP4(Interface)
CMP2(Block)
CMP5(Template)
CMP3(Enum)
CMP1(TemplateParameter)
CMP2(TemplateParameterWithDefaultValue)
CMP1(VariadicTemplateParameter)
CMP3(FunctionParameter)
CMP5(Function)
CMP11(NameSpace)
CMP1(Exists)
CMP3(File)
CMP1(Import)

inline std::weak_ordering operator<=>(const NodeStructs::MetaType& left, const NodeStructs::MetaType& right) {
	if (std::holds_alternative<NodeStructs::EnumType>(left.type._value) &&
		std::holds_alternative<NodeStructs::EnumValueType>(right.type._value)) {
		const auto& a = std::get<NodeStructs::EnumType>(left.type._value);
		const auto& b = std::get<NodeStructs::EnumValueType>(right.type._value);
		return cmp(a.enum_.get(), b.enum_.get());
	}
	if (std::holds_alternative<NodeStructs::EnumValueType>(left.type._value) &&
		std::holds_alternative<NodeStructs::EnumType>(right.type._value)) {
		const auto& a = std::get<NodeStructs::EnumValueType>(left.type._value);
		const auto& b = std::get<NodeStructs::EnumType>(right.type._value);
		return cmp(a.enum_.get(), b.enum_.get());
	}
	return cmp(left.type._value, right.type._value);
}

inline std::weak_ordering operator<=>(const NodeStructs::PrimitiveType& left, const NodeStructs::PrimitiveType& right) {
	const auto& a = left.value._value;
	const auto& b = right.value._value;
	return cmp(a.index(), b.index());
}

inline int copy(int i) {
	return i;
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
COPY1(TupleTypename)
COPY1(UnionTypename)
COPY1(OptionalTypename)
COPY1(VariadicExpansionTypename)
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
COPY2(SwitchCase)
COPY2(SwitchStatement)
COPY2(Assignment)
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
COPY1(TupleType)
COPY1(UnionType)
COPY1(OptionalType)
COPY2(TemplateType)
COPY1(Builtin)
COPY1(EnumType)
COPY2(EnumValueType)
COPY2(AggregateType)
COPY0(Vector)
COPY0(Set)
COPY0(Map)
COPY1(VectorType)
COPY1(SetType)
COPY2(MapType)
COPY4(Interface)
COPY2(Block)
COPY5(Template)
COPY3(Enum)
COPY1(TemplateParameter)
COPY2(TemplateParameterWithDefaultValue)
COPY1(VariadicTemplateParameter)
COPY3(FunctionParameter)
COPY5(Function)
COPY11(NameSpace)
COPY1(Exists)
COPY3(File)
COPY1(Import)
