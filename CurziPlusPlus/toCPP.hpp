#pragma once
#include <fstream>
#include <string>
#include <sstream>
#include <map>
#include <set>
#include <expected>
#include <ranges>

#include "no_copy.hpp"
#include "node_structs.hpp"
#include "on_exit.hpp"

struct user_error {
	std::string content;
	std::unexpected<user_error> unexpected() && {
		return std::unexpected<user_error>{ std::move(*this) };
	}
};
using variables_t = std::map<std::string, std::vector<std::pair<NodeStructs::ValueCategory, NodeStructs::TypeCategory>>>;
using transpile_header_cpp_t = std::expected<std::pair<std::string, std::string>, user_error>;
using transpile_t = std::expected<std::string, user_error>;
struct type_and_representation {
	NodeStructs::TypeCategory type;
	std::string representation;
};
using transpile_type_repr = std::expected<type_and_representation, user_error>;


struct Named {
	template <typename T> using map2ref = std::map<std::string, T const *>;
	map2ref<NodeStructs::Function> functions;
	map2ref<NodeStructs::Template<NodeStructs::Function>> function_templates;
	map2ref<NodeStructs::Type> types;
	map2ref<NodeStructs::Template<NodeStructs::Type>> type_templates;
	map2ref<NodeStructs::Block> blocks;
	map2ref<NodeStructs::Template<NodeStructs::Block>> block_templates;
};
struct transpilation_state {
	variables_t variables;
	Named named;
	std::vector<std::pair<std::string, std::string>> transpile_in_reverse_order;
	size_t indent = 0;

	transpilation_state(
		variables_t&& variables,
		Named&& named
	) : variables(std::move(variables)), named(std::move(named)) {}

	[[nodiscard]] auto indent_sentinel() {
		indent += 1;
		return OnExit([&]() { indent -= 1; });
	}

	[[nodiscard]] auto zero_indent_sentinel() {
		auto old = indent;
		indent = 0;
		return OnExit([this, _old = old]() { indent  = _old; });
	}

	std::set<NodeStructs::Function> traversed_functions;
	//std::set<NodeStructs::Template<NodeStructs::Function>> traversed_function_templates;
	std::set<NodeStructs::Type> traversed_types;
	//std::set<NodeStructs::Template<NodeStructs::Type>> traversed_type_templates;
	std::set<NodeStructs::Block> traversed_blocks;
	//std::set<NodeStructs::Template<NodeStructs::Block>> traversed_block_templates;
private:
	no_copy _;
};

template <size_t token>
static constexpr std::string _symbol_as_text() {
	if constexpr (token == TOKENS::ASTERISK) return "*";
	if constexpr (token == TOKENS::SLASH) return "/";
	if constexpr (token == TOKENS::PERCENT) return "%";
	if constexpr (token == TOKENS::EQUAL) return "=";
	if constexpr (token == TOKENS::LT) return "<";
	if constexpr (token == TOKENS::GT) return ">";
	if constexpr (token == TOKENS::DASH) return "-";
	if constexpr (token == TOKENS::PLUS) return "+";
	if constexpr (token == TOKENS::TILDE) return "~";
	if constexpr (token == TOKENS::EQUALEQUAL) return "==";
	if constexpr (token == TOKENS::NEQUAL) return "!=";
	if constexpr (token == TOKENS::PLUSEQUAL) return "+=";
	if constexpr (token == TOKENS::MINUSEQUAL) return "-=";
	if constexpr (token == TOKENS::TIMESEQUAL) return "*=";
	if constexpr (token == TOKENS::DIVEQUAL) return "/=";
	if constexpr (token == TOKENS::MODEQUAL) return "%=";
	if constexpr (token == TOKENS::ANDEQUAL) return "&=";
	if constexpr (token == TOKENS::OREQUAL) return "|=";
	if constexpr (token == TOKENS::XOREQUAL) return "^=";
	if constexpr (token == TOKENS::LTE) return "<=";
	if constexpr (token == TOKENS::GTE) return ">=";
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

struct cpp_std {
	NodeStructs::Type _uset = NodeStructs::Type{
		.name = std::string{ "Set" },
		.methods = std::vector<NodeStructs::Function>{},
		.memberVariables = std::vector<NodeStructs::MemberVariable>{},
	};

	NodeStructs::Template<NodeStructs::Type> unordered_set = {
		std::vector{ std::string{ "T" } },
		_uset,
	};

	NodeStructs::Type _vec = NodeStructs::Type{
		.name = std::string{ "Vector" },
		.methods = std::vector<NodeStructs::Function>{},
		.memberVariables = std::vector<NodeStructs::MemberVariable>{},
	};

	NodeStructs::Template<NodeStructs::Type> vector = {
		std::vector{std::string{"T"}},
		_vec,
	};

	NodeStructs::Template<NodeStructs::Type> unordered_map = []() {
		std::vector<NodeStructs::Function> methods = {
			NodeStructs::Function{
				"at",
				NodeStructs::BaseTypename{ "V" },
				std::vector<std::tuple<NodeStructs::Typename, NodeStructs::ValueCategory, std::string>>{},
				std::vector<NodeStructs::Statement>{},
			}
		};
		return NodeStructs::Template<NodeStructs::Type>{
			std::vector<std::string>{ "K", "V" },
			NodeStructs::Type{
				.name = std::string{ "Map" },
				.methods = std::move(methods),
			},
		};
	}();

	NodeStructs::Template<NodeStructs::Type> pair = {
		std::vector<std::string>{ "First", "Second" },
		NodeStructs::Type{
			.name = std::string{ "Pair" },
			.methods = std::vector<NodeStructs::Function>{},
			.memberVariables = std::vector<NodeStructs::MemberVariable>{},
		},
	};
	NodeStructs::Type _int = { "Int" };
	NodeStructs::Type _bool = { "Bool" };
	NodeStructs::Type string = { "String" };
	NodeStructs::Type _void = { "Void" };

	NodeStructs::Function println{
		.name = std::string{ "println" },
		.returnType = NodeStructs::Typename{ NodeStructs::BaseTypename{ "Void" } },
		.parameters = std::vector<std::tuple<NodeStructs::Typename, NodeStructs::ValueCategory, std::string>>{
			{ NodeStructs::Typename{ NodeStructs::BaseTypename{"String"} }, NodeStructs::ValueCategory{ NodeStructs::Value{} }, "t" }
		},
	};

	/*NodeStructs::Template<NodeStructs::Function> println = {
		std::vector<std::string>{ "T" },
		_println
	};*/
};

static constexpr auto default_includes = 
	"#include <utility>\n"

	"using Int = int;\n"
	"using Bool = bool;\n"
	"using Void = void;\n"
	"template <typename First, typename Second> using Pair = std::pair<First, Second>;\n"

	"#include <variant>\n"
	"template <typename... Ts> using Variant = std::variant<Ts...>;\n"

	"#include <vector>\n"
	"template <typename T> using Vector = std::vector<T>;\n"

	"#include <string>\n"
	"using String = std::string;\n"

	"#include <unordered_set>\n"
	"template <typename T> using Set = std::unordered_set<T>;\n"

	"#include <set>\n"
	"template <typename T> using TreeSet = std::set<T>;\n"

	"#include <unordered_map>\n"
	"template <typename K, typename V> using Map = std::unordered_map<K, V>;\n"

	"static constexpr bool True = true;\n"
	"static constexpr bool False = false;\n"

	"\n";

void insert_all_named_recursive_with_imports(const std::vector<NodeStructs::File>& project, Named& named, const std::string& filename);
std::expected<std::pair<std::string, std::string>, user_error> transpile(const std::vector<NodeStructs::File>& project);

transpile_header_cpp_t transpile_main(
	transpilation_state& state,
	const NodeStructs::Function& fn
);

transpile_header_cpp_t transpile(
	transpilation_state& state,
	const NodeStructs::Function& fn
);

transpile_header_cpp_t transpile(
	transpilation_state& state,
	const NodeStructs::Type& type
);

transpile_t transpile(
	transpilation_state& state,
	const std::vector<std::tuple<NodeStructs::Typename, NodeStructs::ValueCategory, std::string>>& parameters
);

transpile_t transpile(
	transpilation_state& state,
	const std::vector<NodeStructs::Statement>& statements
);

static std::string indent(size_t n) {
	std::string res;
	res.reserve(n);
	for (size_t i = 0; i < n; ++i)
		res += '\t';
	return res;
};

std::vector<NodeStructs::TypeCategory> decomposed_type(
	transpilation_state& state,
	const NodeStructs::TypeCategory& type
);

std::optional<user_error> add_decomposed_for_iterator_variables(
	transpilation_state& state,
	const std::vector<std::variant<NodeStructs::VariableDeclaration, std::string>>& iterators,
	const NodeStructs::TypeCategory& it_type
);

std::optional<user_error> add_for_iterator_variable(
	transpilation_state& state,
	const std::vector<std::variant<NodeStructs::VariableDeclaration, std::string>>& iterators,
	const NodeStructs::TypeCategory& it_type
);

void remove_for_iterator_variables(
	transpilation_state& state,
	const NodeStructs::ForStatement& statement
);

void remove_added_variables(
	transpilation_state& state,
	const NodeStructs::Statement& statement
);

transpile_t transpile_arg(
	transpilation_state& state,
	const NodeStructs::FunctionArgument& arg
);

transpile_t transpile_args(
	transpilation_state& state,
	const std::vector<NodeStructs::FunctionArgument>& args
);

NodeStructs::TypeCategory iterator_type(
	transpilation_state& state,
	const NodeStructs::TypeCategory& type
);
