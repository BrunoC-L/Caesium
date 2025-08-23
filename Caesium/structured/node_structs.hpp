#pragma once
#include <vector>
#include <string>
#include <optional>
#include <compare>
#include <map>
#include <iostream>

#include "lib.hpp"

#include "../utility/box.hpp"
#include "../utility/overload.hpp"

#include "../grammar/primitives.hpp"

struct cursor_info {
	unsigned line;
	unsigned column;
};

constexpr inline cursor_info copy(const cursor_info& e) { return e; }

struct caesium_source_location {
	std::string file_name;
	std::string content;
	cursor_info beg = { std::numeric_limits<unsigned>::max(), std::numeric_limits<unsigned>::max() };
	cursor_info end = { std::numeric_limits<unsigned>::max(), std::numeric_limits<unsigned>::max() };
};

constexpr inline caesium_source_location copy(const caesium_source_location& e) { return e; }

template <typename T>
struct empty {};

template <typename T>
constexpr inline empty<T> copy(const empty<T>&) { return {}; }

template <typename T>
caesium_source_location rule_info_stub() {
	empty<T> t{}; // for debug purposes
	(void)t; // silence unused warning, this variable only exists to show T when debugging
	NOT_IMPLEMENTED;
}

caesium_source_location rule_info_stub_no_throw();
caesium_source_location rule_info_language_element(std::string s);

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

namespace NodeStructs {
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

	struct Type;

	struct Reference {};
	struct MutableReference {};
	struct Move {};
	struct Value {};
	using ArgumentCategory = Variant<Reference, MutableReference, Move>;
	using ValueCategory = Variant<Reference, MutableReference, Value>;

	struct Typename {
		using vt = Variant<TemplatedTypename, NamespacedTypename, BaseTypename, OptionalTypename, UnionTypename, VariadicExpansionTypename>;
		NonCopyableBox<vt> value;
		Optional<ValueCategory> category;
		caesium_source_location info = rule_info_stub<Typename>();
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
		std::string name;
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
		caesium_source_location info = rule_info_stub<Expression>();
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
		caesium_source_location info = rule_info_stub<Function>();
	};

	struct Type {
		std::string name;
		std::optional<Typename> name_space;
		std::vector<Statement<type_context>> members;
		caesium_source_location info = rule_info_stub<Type>();
	};

	struct Interface {
		std::string name;
		std::optional<Typename> name_space;
		std::vector<Statement<type_context>> members;
		caesium_source_location info = rule_info_stub<Interface>();
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
		caesium_source_location info = rule_info_stub<Template>();
	};

	struct Enum {
		std::string name;
		std::vector<std::string> values;
		std::optional<Typename> name_space;
	};

	struct Block {
		std::string name;
		std::vector<Statement<function_context>> statements;
	};

	struct NameSpace {
		std::string name;

		std::optional<Typename> name_space;

		std::vector<Function> functions;

		std::vector<Type> types;
		std::vector<Interface> interfaces;

		std::vector<Template> templates;

		std::vector<Block> blocks;
		std::vector<Alias> aliases;
		std::vector<Enum> enums;

		std::vector<NameSpace> namespaces;

		caesium_source_location info = rule_info_stub<NameSpace>();
	};

	struct Exists {
		NameSpace global_exists;
	};

	struct File {
		std::vector<Import> imports;
		std::vector<Exists> exists;
		NameSpace content;
	};

	using RunTimeStatement = Variant<
		Expression,
		VariableDeclarationStatement<function_context>,
		IfStatement<function_context>,
		ForStatement<function_context>,
		IForStatement<function_context>,
		WhileStatement<function_context>,
		BreakStatement<function_context>,
		ReturnStatement<function_context>,
		BlockStatement<function_context>,
		MatchStatement<function_context>,
		SwitchStatement<function_context>,
		Assignment<function_context>
	>;

	template <typename context>
	struct CompileTimeStatement : Variant<
		VariableDeclarationStatement<context>,
		IfStatement<context>,
		ForStatement<context>,
		IForStatement<context>,
		WhileStatement<context>,
		BreakStatement<context>,
		BlockStatement<context>,
		MatchStatement<context>,
		SwitchStatement<context>,
		Assignment<context>
	> {};

	template <typename T> struct contextual_options_;

	template <>
	struct contextual_options_<function_context> { using type = Variant<RunTimeStatement>; };

	template <>
	struct contextual_options_<type_context> { using type = Variant<Alias, MemberVariable>; };

	template <>
	struct contextual_options_<top_level_context> { using type = Variant<Type, Function>; };

	template <typename T>
	using contextual_options = contextual_options_<T>::type;

	template <typename context_>
	struct Statement {
		NonCopyableBox<Variant<CompileTimeStatement<context_>, contextual_options<context_>>> statement;
#ifdef DEBUG
		caesium_lib::string::type debug_info_string;
#endif
	};
};
