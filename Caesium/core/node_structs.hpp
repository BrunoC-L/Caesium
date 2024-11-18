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
template <typename T> using Optional = caesium_lib::optional::type<T>;

struct Namespace;

struct NodeStructs {
	struct TemplatedTypename;
	struct NamespacedTypename;
	struct BaseTypename;
	struct VariadicExpansionTypename;
	struct TupleTypename;
	struct UnionTypename;
	struct Typename;
	struct WordTypenameOrExpression;

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

	struct Reference {};
	struct MutableReference {};
	struct Move {};
	struct Value {};
	using ArgumentCategory = Variant<Reference, MutableReference, Move>;
	using ParameterCategory = Variant<Reference, MutableReference, Value>;
	using ValueCategory = Variant<Reference, MutableReference, Value>;

	struct TemplatedTypename {
		NonCopyableBox<Typename> type;
		std::vector<WordTypenameOrExpression> templated_with;
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

	struct OptionalTypename {
		NonCopyableBox<Typename> type;
	};

	struct Typename {
		using Variant_ = Variant<TemplatedTypename, NamespacedTypename, BaseTypename, OptionalTypename, TupleTypename, UnionTypename, VariadicExpansionTypename>;
		Variant_ value;
		Optional<ParameterCategory> category;
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
		>;
		NonCopyableBox<vt> expression;
	};

	NodeStructs::Expression make_expression(NodeStructs::Expression::vt expr) {
		if (std::holds_alternative<std::string>(expr._value) && std::get<std::string>(expr._value) == "DB")
			throw;
		return { std::move(expr) };
	}

	struct WordTypenameOrExpression {
		Variant<std::string, Typename, Expression> value;
	};

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
		std::vector<WordTypenameOrExpression> args;
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
			Token<LT>,
			Token<LTE>,
			Token<GT>,
			Token<GTE>
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
		std::string index_iterator;
		ForStatement for_statement;
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
		using VT = Variant<
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
		>;
		VT statement;
		bool is_compile_time;
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
		NodeStructs::WordTypenameOrExpression value;
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

	struct TypeToken {
	};

	struct TypeList {
	};

	struct TypeListType {
		std::vector<MetaType> types;
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

	struct CompileTimeType {
		NonCopyableBox<MetaType> type;
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
			MapType, // type(Map<Int, Int>)
			TypeList, // type(type_list)
			TypeListType, // type(type_list<Int, String, Cat>)
			TypeToken, // type(type)

			CompileTimeType // type of a variable declared with #type var = ...
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
	else if constexpr (is_specialization<T, caesium_lib::optional::type>::value) {
		return cmp(a._value, b._value);
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

inline bool copy(bool b) {
	return b;
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
Optional<T> copy(const Optional<T>& x) {
	return Optional<T>{ copy(x._value) };
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

#define CMP0(T) inline std::weak_ordering operator<=>(const NodeStructs::T& left, const NodeStructs::T& right) { return cmp0(left, right); }
#define CMP1(T) inline std::weak_ordering operator<=>(const NodeStructs::T& left, const NodeStructs::T& right) { return cmp1(left, right); }
#define CMP2(T) inline std::weak_ordering operator<=>(const NodeStructs::T& left, const NodeStructs::T& right) { return cmp2(left, right); }
#define CMP3(T) inline std::weak_ordering operator<=>(const NodeStructs::T& left, const NodeStructs::T& right) { return cmp3(left, right); }
#define CMP4(T) inline std::weak_ordering operator<=>(const NodeStructs::T& left, const NodeStructs::T& right) { return cmp4(left, right); }
#define CMP5(T) inline std::weak_ordering operator<=>(const NodeStructs::T& left, const NodeStructs::T& right) { return cmp5(left, right); }
#define CMP6(T) inline std::weak_ordering operator<=>(const NodeStructs::T& left, const NodeStructs::T& right) { return cmp6(left, right); }
#define CMP11(T) inline std::weak_ordering operator<=>(const NodeStructs::T& left, const NodeStructs::T& right) { return cmp11(left, right); }

#define COPY0(T) inline NodeStructs::T copy(const NodeStructs::T& tn) { return copy0(tn); }
#define COPY1(T) inline NodeStructs::T copy(const NodeStructs::T& tn) { return copy1(tn); }
#define COPY2(T) inline NodeStructs::T copy(const NodeStructs::T& tn) { return copy2(tn); }
#define COPY3(T) inline NodeStructs::T copy(const NodeStructs::T& tn) { return copy3(tn); }
#define COPY4(T) inline NodeStructs::T copy(const NodeStructs::T& tn) { return copy4(tn); }
#define COPY5(T) inline NodeStructs::T copy(const NodeStructs::T& tn) { return copy5(tn); }
#define COPY11(T) inline NodeStructs::T copy(const NodeStructs::T& tn) { return copy11(tn); }

#define CMP_COPY_N(N, T) CMP##N(T) COPY##N(T)

COPY1(MetaType)

CMP_COPY_N(2, Typename)
CMP_COPY_N(1, OptionalTypename)
CMP_COPY_N(1, UnionTypename)
CMP_COPY_N(1, TupleTypename)
CMP_COPY_N(1, VariadicExpansionTypename)
CMP_COPY_N(1, BaseTypename)
CMP_COPY_N(2, NamespacedTypename)
CMP_COPY_N(2, TemplatedTypename)
CMP_COPY_N(1, WordTypenameOrExpression)
CMP_COPY_N(1, Expression)
CMP_COPY_N(0, Reference)
CMP_COPY_N(0, MutableReference)
CMP_COPY_N(0, Move)
CMP_COPY_N(0, Value)
CMP_COPY_N(2, Statement)
CMP_COPY_N(2, VariableDeclaration)
CMP_COPY_N(3, VariableDeclarationStatement)
CMP_COPY_N(3, IfStatement)
CMP_COPY_N(3, ForStatement)
CMP_COPY_N(2, IForStatement)
CMP_COPY_N(2, WhileStatement)
CMP_COPY_N(2, MatchCase)
CMP_COPY_N(2, MatchStatement)
CMP_COPY_N(1, BreakStatement)
CMP_COPY_N(2, ReturnStatement)
CMP_COPY_N(2, SwitchCase)
CMP_COPY_N(2, SwitchStatement)
CMP_COPY_N(2, Assignment)
CMP_COPY_N(1, BlockStatement)
CMP_COPY_N(2, FunctionArgument)
CMP_COPY_N(2, ConditionalExpression)
CMP_COPY_N(2, OrExpression)
CMP_COPY_N(2, AndExpression)
CMP_COPY_N(2, EqualityExpression)
CMP_COPY_N(2, CompareExpression)
CMP_COPY_N(2, AdditiveExpression)
CMP_COPY_N(2, MultiplicativeExpression)
CMP_COPY_N(2, UnaryExpression)
CMP_COPY_N(2, CallExpression)
CMP_COPY_N(2, NamespaceExpression)
CMP_COPY_N(2, TemplateExpression)
CMP_COPY_N(2, ConstructExpression)
CMP_COPY_N(2, BracketAccessExpression)
CMP_COPY_N(3, PropertyAccessAndCallExpression)
CMP_COPY_N(2, PropertyAccessExpression)
CMP_COPY_N(1, ParenArguments)
CMP_COPY_N(1, BraceArguments)
CMP_COPY_N(1, BracketArguments)
CMP_COPY_N(1, TemplateArguments)
CMP_COPY_N(4, Type)
CMP_COPY_N(3, Alias)
CMP_COPY_N(2, MemberVariable)
CMP_COPY_N(2, FunctionType)
CMP_COPY_N(1, InterfaceType)
CMP_COPY_N(1, NamespaceType)
CMP_COPY_N(1, TupleType)
CMP_COPY_N(1, UnionType)
CMP_COPY_N(1, OptionalType)
CMP_COPY_N(2, TemplateType)
CMP_COPY_N(1, Builtin)
CMP_COPY_N(1, EnumType)
CMP_COPY_N(2, EnumValueType)
CMP_COPY_N(2, AggregateType)
CMP_COPY_N(0, Vector)
CMP_COPY_N(0, Set)
CMP_COPY_N(0, Map)
CMP_COPY_N(1, VectorType)
CMP_COPY_N(1, SetType)
CMP_COPY_N(2, MapType)
CMP_COPY_N(0, TypeToken)
CMP_COPY_N(0, TypeList)
CMP_COPY_N(1, TypeListType)
CMP_COPY_N(1, CompileTimeType)
CMP_COPY_N(4, Interface)
CMP_COPY_N(2, Block)
CMP_COPY_N(5, Template)
CMP_COPY_N(3, Enum)
CMP_COPY_N(1, TemplateParameter)
CMP_COPY_N(2, TemplateParameterWithDefaultValue)
CMP_COPY_N(1, VariadicTemplateParameter)
CMP_COPY_N(2, FunctionParameter)
CMP_COPY_N(5, Function)
CMP_COPY_N(11, NameSpace)
CMP_COPY_N(1, Exists)
CMP_COPY_N(3, File)
CMP_COPY_N(1, Import)
