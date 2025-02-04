#pragma once
#include <vector>
#include <string>
#include "lib.hpp"
#include <optional>
#include <compare>
#include <map>
#include <iostream>

#include "../utility/box.hpp"
#include "../utility/overload.hpp"

#include "primitives.hpp"

struct cursor_info {
	unsigned line;
	unsigned column;
};

constexpr inline cursor_info copy(const cursor_info& e) { return e; }

struct rule_info {
	std::string file_name;
	std::string content;
	cursor_info beg;
	cursor_info end;
};

constexpr inline rule_info copy(const rule_info& e) { return e; }

template <typename T>
struct empty {};

template <typename T>
constexpr inline empty<T> copy(const empty<T>&) { return {}; }

template <typename T>
rule_info rule_info_stub() {
	empty<T> t{}; // for debug purposes
	throw;
}

rule_info rule_info_stub_no_throw();
rule_info rule_info_language_element(std::string s);

template <typename... Ts> using Variant = caesium_lib::variant::type<Ts...>;
template <typename T> using Optional = caesium_lib::optional::type<T>;

namespace grammar {
	struct type_context;
	struct function_context;
	struct top_level_context;
}

using type_context = grammar::type_context;
using function_context = grammar::function_context;
using top_level_context = grammar::top_level_context;

struct Namespace;

struct NodeStructs {
	struct TemplatedTypename;
	struct NamespacedTypename;
	struct BaseTypename;
	struct VariadicExpansionTypename;
	struct OptionalTypename;
	struct UnionTypename;
	struct Typename;
	struct WordTypenameOrExpression;

	template <typename context>
	struct Statement;

	template <typename context>
	struct CompileTimeStatement;

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

	struct Builtin;
	struct PrimitiveType;
	struct Type;
	struct FunctionType;
	struct InterfaceType;
	struct NamespaceType;
	struct UnionType;
	struct TemplateType;
	struct EnumType;
	struct EnumValueType;
	struct OptionalType;
	struct AggregateType;
	struct Vector;
	struct VectorType;
	struct Set;
	struct SetType;
	struct Map;
	struct MapType;
	struct TypeList;
	struct TypeListType;
	struct TypeToken;
	struct CompileTimeType;
	struct MetaType;

	struct Reference {};
	struct MutableReference {};
	struct Move {};
	struct Value {};
	using ArgumentCategory = Variant<Reference, MutableReference, Move>;
	using ParameterCategory = Variant<Reference, MutableReference, Value>;
	using ValueCategory = Variant<Reference, MutableReference, Value>;

	struct Typename {
		using vt = Variant<TemplatedTypename, NamespacedTypename, BaseTypename, OptionalTypename, UnionTypename, VariadicExpansionTypename>;
		NonCopyableBox<vt> value;
		Optional<ParameterCategory> category;
		rule_info rule_info = rule_info_stub<Typename>();
	};

	struct TemplatedTypename {
		Typename type;
		std::vector<WordTypenameOrExpression> templated_with;
	};

	struct NamespacedTypename {
		Typename name_space;
		std::string name_in_name_space;
	};

	struct BaseTypename {
		std::string type;
	};

	struct VariadicExpansionTypename {
		Typename type;
	};

	struct UnionTypename {
		std::vector<Typename> ors;
	};

	struct OptionalTypename {
		Typename type;
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
		rule_info rule_info = rule_info_stub<Expression>();
	};

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
		std::vector<WordTypenameOrExpression> args;
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

	template <typename context>
	struct VariableDeclarationStatement {
		Typename type;
		std::string name;
		Expression expr;
	};

	template <typename context>
	struct ForStatement {
		Expression collection;
		std::vector<Variant<VariableDeclaration, std::string>> iterators;
		std::vector<Statement<context>> statements;
	};

	template <typename context>
	struct IForStatement {
		std::string index_iterator;
		ForStatement<context> for_statement;
	};

	template <typename context>
	struct IfStatement {
		Expression ifExpr;
		std::vector<Statement<context>> ifStatements;
		std::optional<Variant<NonCopyableBox<IfStatement<context>>, std::vector<Statement<context>>>> elseExprStatements;
	};

	template <typename context>
	struct WhileStatement {
		Expression whileExpr;
		std::vector<Statement<context>> statements;
	};

	template <typename context>
	struct MatchCase {
		std::vector<std::pair<Typename, std::string>> variable_declarations;
		std::vector<Statement<context>> statements;
	};

	template <typename context>
	struct MatchStatement {
		std::vector<Expression> expressions;
		std::vector<MatchCase<context>> cases;
	};

	template <typename context>
	struct BreakStatement {
		std::optional<Expression> ifExpr;
	};

	template <typename context>
	struct ReturnStatement {
		std::vector<FunctionArgument> returnExpr;
		std::optional<Expression> ifExpr;
	};

	template <typename context>
	struct SwitchCase {
		Expression expr;
		std::vector<Statement<context>> statements;
	};

	template <typename context>
	struct SwitchStatement {
		Expression expr;
		std::vector<SwitchCase<context>> cases;
	};

	template <typename context>
	struct Assignment {
		Expression left;
		Expression right;
	};

	template <typename context>
	struct BlockStatement {
		Typename parametrized_block;
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
		std::vector<Statement<function_context>> statements;
	};

	struct Type {
		std::string name;
		std::optional<Typename> name_space;
		std::vector<Alias> aliases;
		std::vector<MemberVariable> member_variables;
		std::vector<CompileTimeStatement<type_context>> compile_time_statements;
		rule_info rule_info = rule_info_stub<Type>();
	};

	struct Interface {
		std::string name;
		std::optional<Typename> name_space;
		std::vector<Alias> aliases;
		std::vector<MemberVariable> member_variables;
		rule_info rule_info = rule_info_stub<Interface>();
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
		WordTypenameOrExpression value;
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
		rule_info rule_info = rule_info_stub<Template>();
	};

	struct MetaType {
		using vt = Variant <
			PrimitiveType, // ex. type(1)
			Type, // ex. type Dog -> type(Dog{})

			FunctionType, // ex. Bool has_bone(...) -> type(has_bone)
			InterfaceType, // ex. interface Animal -> type(Animal)
			NamespaceType, // ex. namespace std -> type(std)
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
		> ;
		NonCopyableBox<vt> type;
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

	struct Vector {
	};

	struct VectorType {
		MetaType value_type;
	};

	struct Set {
	};

	struct SetType {
		MetaType value_type;
	};

	struct Map {
	};

	struct MapType {
		MetaType key_type;
		MetaType value_type;
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
		template <typename T>
		struct NonValued {};
		template <typename T>
		struct Valued {
			T value;
		};
		using vt = Variant<
			NonValued<std::string>,
			NonValued<double>,
			NonValued<int>,
			NonValued<bool>,
			NonValued<void_t>,
			NonValued<char>,
			NonValued<empty_optional_t>,
			Valued<std::string>,
			Valued<double>,
			Valued<int>,
			Valued<bool>,
			Valued<void_t>,
			Valued<char>,
			Valued<empty_optional_t>
		>;
		vt value;
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
		MetaType value_type;
	};

	struct CompileTimeType {
		MetaType type;
	};

	struct Block {
		std::string name;
		std::vector<Statement<function_context>> statements;
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

		rule_info rule_info = rule_info_stub<NameSpace>();
	};

	struct Exists {
		NameSpace global_exists;
	};

	struct File {
		std::vector<Import> imports;
		std::vector<Exists> exists;
		NameSpace content;
	};

	template <typename context>
	using RunTimeStatement = Variant<
		Expression,
		VariableDeclarationStatement<context>,
		IfStatement<context>,
		ForStatement<context>,
		IForStatement<context>,
		WhileStatement<context>,
		BreakStatement<context>,
		ReturnStatement<context>,
		BlockStatement<context>,
		MatchStatement<context>,
		SwitchStatement<context>,
		Assignment<context>
	>;

	template <typename context>
	struct CompileTimeStatement : Variant<
		VariableDeclarationStatement<context>,
		IfStatement<context>,
		ForStatement<context>,
		IForStatement<context>,
		WhileStatement<context>,
		BreakStatement<context>,
		ReturnStatement<context>,
		BlockStatement<context>,
		MatchStatement<context>,
		SwitchStatement<context>,
		Assignment<context>
	> {};

	template <>
	struct Statement<function_context> {
		using context = function_context;
		using contextual_options = Variant<RunTimeStatement<function_context>>;
		NonCopyableBox<Variant<CompileTimeStatement<context>, contextual_options>> statement;
		bool is_compile_time;
	};

	template <>
	struct Statement<type_context> {
		using context = type_context;
		using contextual_options = Variant<Alias, MemberVariable>;
		NonCopyableBox<Variant<CompileTimeStatement<context>, contextual_options>> statement;
	};

	template <>
	struct Statement<top_level_context> {
		using context = type_context;
		using contextual_options = Variant<Type>;
		NonCopyableBox<Variant<CompileTimeStatement<context>, contextual_options>> statement;
	};
};
