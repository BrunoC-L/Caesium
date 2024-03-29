#pragma once
#include <fstream>
#include <string>
#include <sstream>
#include <map>
#include <set>
#include "../utility/expected.hpp"
#include "../utility/enumerate.hpp"

#include "node_structs.hpp"

struct variable_info {
	NodeStructs::ValueCategory value_category;
	NodeStructs::MetaType type;
};
using variables_t = std::map<std::string, std::vector<variable_info>>;
using transpile_header_cpp_t = expected<std::pair<std::string, std::string>>;

struct type_information{
	NodeStructs::MetaType type;
	std::string representation;
};

struct non_type_information{
	NodeStructs::ExpressionType type;
	std::string representation;
	NodeStructs::ValueCategory value_category;
};

using expression_information = std::variant<type_information, non_type_information>;
using transpile_t = expected<std::string>;
using transpile_t2 = expected<expression_information>;

struct Named {
	template <typename T> using map_to_vec = std::map<std::string, std::vector<T const*>>;

	map_to_vec<NodeStructs::Function> functions;
	map_to_vec<NodeStructs::Function> functions_using_auto;
	map_to_vec<NodeStructs::Type> types;
	map_to_vec<NodeStructs::Interface> interfaces;
	map_to_vec<NodeStructs::Block> blocks;
	map_to_vec<NodeStructs::Template> templates;
	map_to_vec<NodeStructs::NameSpace> namespaces;
	std::map<std::string, NodeStructs::Typename> type_aliases_typenames;
};

struct transpilation_state {
	variables_t variables;
	Named named;
	//std::vector<std::pair<std::string, std::string>> transpile_in_reverse_order;
	unsigned current_variable_unique_id = 1;

	transpilation_state(const transpilation_state& other) = delete;
	transpilation_state(transpilation_state&& other) = delete;
	transpilation_state(
		variables_t&& variables,
		Named&& named
	) : variables(std::move(variables)), named(std::move(named)) {}

	// shouldnt those be pointers...?
	std::set<NodeStructs::Function> traversed_functions;
	std::set<NodeStructs::Type> traversed_types;
	std::set<NodeStructs::Interface> traversed_interfaces;

	std::set<NodeStructs::Function> functions_to_transpile;
	std::set<NodeStructs::Type> types_to_transpile;
	std::set<NodeStructs::Interface> interfaces_to_transpile;

	std::map<NodeStructs::Typename, std::vector<NodeStructs::MetaType>> interface_symbol_to_members;
};

struct transpilation_state_with_indent {
	transpilation_state& state;
	size_t indent = 0;

	transpilation_state_with_indent indented() {
		return { state, indent + 1 };
	}

	transpilation_state_with_indent unindented() {
		return { state, 0 };
	}
};

template <size_t token>
static constexpr std::string _symbol_as_text() {
	if constexpr (token == TOKENS::ASTERISK) return "*";
	if constexpr (token == TOKENS::SLASH) return "/";
	if constexpr (token == TOKENS::PERCENT) return "%";
	if constexpr (token == TOKENS::EQUAL) return "=";
	if constexpr (token == TOKENS::DASH) return "-";
	if constexpr (token == TOKENS::PLUS) return "+";
	if constexpr (token == TOKENS::NOT) return "!";
	if constexpr (token == TOKENS::EQUALEQUAL) return "==";
	if constexpr (token == TOKENS::NEQUAL) return "!=";
	if constexpr (token == TOKENS::LTQ) return "<";
	if constexpr (token == TOKENS::GTQ) return ">";
	if constexpr (token == TOKENS::LTEQ) return "<=";
	if constexpr (token == TOKENS::GTEQ) return ">=";
}

template <size_t token>
std::string symbol_as_text(Token<token>) {
	return _symbol_as_text<token>();
}

template <size_t... tokens>
std::string symbol_variant_as_text(const std::variant<Token<tokens>...>& token) {
	return std::visit(
		[&](const auto& tk) { return symbol_as_text(tk); },
		token
	);
}

struct builtins {
	NodeStructs::Type builtin_void = { "Void", std::nullopt };
	NodeStructs::Type builtin_bool = { "Bool", std::nullopt };
	NodeStructs::Type builtin_int = { "Int", std::nullopt };
	NodeStructs::Type builtin_double = { "Floating", std::nullopt };
	NodeStructs::Type builtin_string = { "String", std::nullopt };

	NodeStructs::Template builtin_variant = { "Variant", std::nullopt, { NodeStructs::VariadicTemplateParameter{ "Args" } }, "BUILTIN" };
	NodeStructs::Template builtin_tuple = { "Tuple", std::nullopt, { NodeStructs::VariadicTemplateParameter{ "Args" } }, "BUILTIN" };
	NodeStructs::Template builtin_vector = { "Vector", std::nullopt, { NodeStructs::TemplateParameter{ "T" } }, "BUILTIN" };
	NodeStructs::Template builtin_set = { "Set", std::nullopt, { NodeStructs::TemplateParameter{ "T" } }, "BUILTIN" };
	NodeStructs::Template builtin_map = { "Map", std::nullopt, { NodeStructs::TemplateParameter{ "K" }, NodeStructs::TemplateParameter{ "V" } }, "BUILTIN" };

	NodeStructs::Template builtin_push =
		{ "push", std::nullopt, { NodeStructs::TemplateParameter{ "Cnt" }, NodeStructs::TemplateParameter{ "T" } }, "BUILTIN" }; // vec
	NodeStructs::Template builtin_insert =
		{ "insert", std::nullopt, { NodeStructs::TemplateParameter{ "Cnt" }, NodeStructs::TemplateParameter{ "T" } }, "BUILTIN" }; // map or set

	NodeStructs::Template builtin_print = { "print", std::nullopt, { NodeStructs::VariadicTemplateParameter{ "Args" } }, "BUILTIN" };
	NodeStructs::Template builtin_println = { "println", std::nullopt, { NodeStructs::VariadicTemplateParameter{ "Args" } }, "BUILTIN" };

	NodeStructs::Typename filesystem = { NodeStructs::BaseTypename{ "filesystem" } };
	NodeStructs::Type builtin_file = { "file", filesystem, {}, {} };
	NodeStructs::Type builtin_directory = { "directory", filesystem, {}, {} };

	NodeStructs::Function entries_dir = {
		"entries",
		filesystem,
		NodeStructs::Typename{ NodeStructs::TemplatedTypename {
			.type = NodeStructs::Typename{ NodeStructs::BaseTypename{ "Vector" } },
			.templated_with = std::vector{
				NodeStructs::Typename{ NodeStructs::UnionTypename{
					std::vector<NodeStructs::Typename>{
						NodeStructs::Typename{ NodeStructs::NamespacedTypename{ filesystem, "file" } },
						NodeStructs::Typename{ NodeStructs::NamespacedTypename{ filesystem, "directory" } }
					}
				} }
			}
		} },
		{
			NodeStructs::FunctionParameter{
				NodeStructs::Typename{ NodeStructs::Typename{ NodeStructs::NamespacedTypename{ filesystem, "directory" } } },
				NodeStructs::ParameterCategory{ NodeStructs::Reference{} },
				std::string{ "dir" }
			}
		},
		{}
	};

	NodeStructs::Function entries_str = {
		"entries",
		filesystem,
		NodeStructs::Typename{ NodeStructs::TemplatedTypename {
			.type = NodeStructs::Typename{ NodeStructs::BaseTypename{ "Vector" } },
			.templated_with = std::vector{
				NodeStructs::Typename{ NodeStructs::UnionTypename{
					std::vector<NodeStructs::Typename>{
						NodeStructs::Typename{ NodeStructs::NamespacedTypename{ filesystem, "file" } },
						NodeStructs::Typename{ NodeStructs::NamespacedTypename{ filesystem, "directory" } }
					}
				} }
			}
		} },
		{
			NodeStructs::FunctionParameter{
				NodeStructs::Typename{ NodeStructs::BaseTypename{ "String" } },
				NodeStructs::ParameterCategory{ NodeStructs::Reference{} },
				std::string{ "dir" }
			}
		},
		{}
	};
	NodeStructs::NameSpace filesystem_ns = {
		.name = "filesystem",
		.name_space = std::nullopt,
		.types = { builtin_file, builtin_directory },
		.functions = { entries_dir, entries_str }
	};
};

static std::string indent(size_t n) {
	std::string res;
	res.reserve(n);
	for (size_t i = 0; i < n; ++i)
		res += '\t';
	return res;
};

static NodeStructs::ValueCategory argument_category_to_value_category(const NodeStructs::ArgumentCategory cat) {
	return std::visit(overload(
		[](const NodeStructs::Reference&) -> NodeStructs::ValueCategory {
			return NodeStructs::Reference{};
		},
		[](const NodeStructs::MutableReference&) -> NodeStructs::ValueCategory {
			return NodeStructs::MutableReference{};
		},
		[](const NodeStructs::Copy&) -> NodeStructs::ValueCategory {
			return NodeStructs::Value{};
		},
		[](const NodeStructs::Move&) -> NodeStructs::ValueCategory {
			return NodeStructs::Value{};
		}
	), cat);
}

std::optional<error> validate_templates(const std::vector<const NodeStructs::Template*>& templates);

static NodeStructs::ValueCategory argument_category_optional_to_value_category(const std::optional<NodeStructs::ArgumentCategory> cat) {
	if (cat.has_value())
		return argument_category_to_value_category(cat.value());
	else
		return NodeStructs::Value{};
}

transpile_header_cpp_t transpile(const std::vector<NodeStructs::File>& project);

transpile_header_cpp_t transpile_main(
	transpilation_state_with_indent state,
	const NodeStructs::Function& fn
);

transpile_header_cpp_t transpile(
	transpilation_state_with_indent state,
	const NodeStructs::Function& fn
);

transpile_header_cpp_t transpile(
	transpilation_state_with_indent state,
	const NodeStructs::Type& type
);

transpile_header_cpp_t transpile(
	transpilation_state_with_indent state,
	const NodeStructs::Interface& interface
);

transpile_t transpile(
	transpilation_state_with_indent state,
	const std::vector<NodeStructs::FunctionParameter>& parameters
);

transpile_t transpile(
	transpilation_state_with_indent state,
	const std::vector<NodeStructs::Statement>& statements
);

std::vector<NodeStructs::MetaType> decompose_type(
	transpilation_state_with_indent state,
	const NodeStructs::MetaType& type
);

std::optional<error> add_decomposed_for_iterator_variables(
	transpilation_state_with_indent state,
	const std::vector<std::variant<NodeStructs::VariableDeclaration, std::string>>& iterators,
	const NodeStructs::MetaType& it_type
);

std::optional<error> add_for_iterator_variable(
	transpilation_state_with_indent state,
	const std::vector<std::variant<NodeStructs::VariableDeclaration, std::string>>& iterators,
	const NodeStructs::MetaType& it_type
);

void remove_for_iterator_variables(
	transpilation_state_with_indent state,
	const NodeStructs::ForStatement& statement
);

void remove_added_variables(
	transpilation_state_with_indent state,
	const NodeStructs::Statement& statement
);

transpile_t transpile_arg(
	transpilation_state_with_indent state,
	const NodeStructs::FunctionArgument& arg
);

transpile_t transpile_args(
	transpilation_state_with_indent state,
	const std::vector<NodeStructs::FunctionArgument>& args
);

transpile_t transpile_expressions(
	transpilation_state_with_indent state,
	const std::vector<NodeStructs::Expression>& args
);

transpile_t transpile_typenames(
	transpilation_state_with_indent state,
	const std::vector<NodeStructs::Typename>& args
);

NodeStructs::MetaType iterator_type(
	transpilation_state_with_indent state,
	const NodeStructs::MetaType& type
);

std::string template_name(
	std::string original_name,
	const std::vector<std::string>& args
);

std::string template_name(
	std::string original_name,
	const std::vector<NodeStructs::Expression>& arguments
);

bool is_assignable_to(
	transpilation_state_with_indent state,
	const NodeStructs::MetaType& parameter,
	const NodeStructs::MetaType& argument
);

transpile_t expr_to_printable(
	transpilation_state_with_indent state,
	const NodeStructs::Expression& expr
);

bool uses_auto(const NodeStructs::Function& fn);
bool uses_auto(const NodeStructs::FunctionParameter& param);
bool uses_auto(const NodeStructs::Statement& param);
bool uses_auto(const NodeStructs::Typename& t);

//#include "../type_visitor/transpile_type_visitor.hpp"
#include "../type_visitor/traverse_type_visitor.hpp"
#include "../type_visitor/type_of_function_like_call_with_args_visitor.hpp"
#include "../type_visitor/type_of_postfix_member_visitor.hpp"
#include "../type_visitor/transpile_member_call_visitor.hpp"
#include "../type_visitor/typename_of_type_visitor.hpp"
#include "../type_visitor/type_of_resolution_operator.hpp"

#include "../expression_visitor/expression_for_template_visitor.hpp"
#include "../expression_visitor/transpile_expression_visitor.hpp"

#include "../statement_visitor/transpile_statement_visitor.hpp"

#include "../typename_visitor/transpile_typename_visitor.hpp"
#include "../typename_visitor/type_of_typename_visitor.hpp"
#include "../typename_visitor/type_template_of_typename_visitor.hpp"
#include "../typename_visitor/typename_for_template_visitor.hpp"


expected<NodeStructs::Function> realise_function_using_auto(
	transpilation_state_with_indent state,
	const NodeStructs::Function& fn_using_auto,
	const std::vector<NodeStructs::FunctionArgument>& args
);

NodeStructs::Typename typename_of_primitive(const NodeStructs::PrimitiveType& primitive_t);

struct Arrangement {
	std::reference_wrapper<const NodeStructs::Template> tmpl;
	std::vector<size_t> arg_placements;
	/*std::weak_ordering operator<=>(const Arrangement& other) {
		return cmp(arg_placements, other.arg_placements);
	}*/
};

expected<Arrangement> find_best_template(
	const std::vector<NodeStructs::Template const*>& templates,
	const std::vector<NodeStructs::Expression>& args
);

expected<Arrangement> find_best_template(
	const std::vector<NodeStructs::Template const*>& templates,
	const std::vector<NodeStructs::Typename>& args
);
