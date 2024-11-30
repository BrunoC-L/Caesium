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

struct rule_info {
	std::string file_name;
	std::string content;
	cursor_info beg;
	cursor_info end;
};

rule_info rule_info_stub();
rule_info rule_info_stub_no_throw();

template <typename... Ts> using Variant = caesium_lib::variant::type<Ts...>;
template <typename T> using Optional = caesium_lib::optional::type<T>;

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
		rule_info rule_info = rule_info_stub();
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
		rule_info rule_info = rule_info_stub();
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
		using vt = Variant<
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
		NonCopyableBox<vt> statement;
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
