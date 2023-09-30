#pragma once
#include <fstream>
#include <string>
#include <sstream>
#include <map>

#include "node_structs.h"

template <size_t token>
std::string _symbol_as_text() {
	if constexpr (token == TOKENS::SEMICOLON) return ";";
	if constexpr (token == TOKENS::COLON || token == TOKENS::IN) return ":";
	if constexpr (token == TOKENS::NS) return "::";
	if constexpr (token == TOKENS::BACKSLASH) return "\\";
	if constexpr (token == TOKENS::DOT) return ".";
	if constexpr (token == TOKENS::COMMA) return ",";
	if constexpr (token == TOKENS::SPACE) return " ";
	if constexpr (token == TOKENS::TAB) return "\t";
	if constexpr (token == TOKENS::NEWLINE) return "\n";
	if constexpr (token == TOKENS::BRACEOPEN) return "{";
	if constexpr (token == TOKENS::BRACECLOSE) return "}";
	if constexpr (token == TOKENS::BRACKETOPEN) return "[";
	if constexpr (token == TOKENS::BRACKETCLOSE) return "]";
	if constexpr (token == TOKENS::PARENOPEN) return "(";
	if constexpr (token == TOKENS::PARENCLOSE) return ")";
	if constexpr (token == TOKENS::ASTERISK) return "*";
	if constexpr (token == TOKENS::SLASH) return "/";
	if constexpr (token == TOKENS::PERCENT) return "%";
	if constexpr (token == TOKENS::AMPERSAND) return "&";
	if constexpr (token == TOKENS::QUESTION) return "?";
	if constexpr (token == TOKENS::POUND) return "#";
	if constexpr (token == TOKENS::EQUAL) return "=";
	if constexpr (token == TOKENS::LT) return "<";
	if constexpr (token == TOKENS::GT) return ">";
	if constexpr (token == TOKENS::DASH) return "-";
	if constexpr (token == TOKENS::NOT) return "!";
	if constexpr (token == TOKENS::CARET) return "^";
	if constexpr (token == TOKENS::BITOR) return "|";
	if constexpr (token == TOKENS::PLUS) return "+";
	if constexpr (token == TOKENS::TILDE) return "~";
	if constexpr (token == TOKENS::PLUSPLUS) return "++";
	if constexpr (token == TOKENS::MINUSMINUS) return "--";
	if constexpr (token == TOKENS::EQUALEQUAL) return "==";
	if constexpr (token == TOKENS::NEQUAL) return "!=";
	if constexpr (token == TOKENS::PLUSEQUAL) return "+=";
	if constexpr (token == TOKENS::MINUSEQUAL) return "-=";
	if constexpr (token == TOKENS::TIMESEQUAL) return "*=";
	if constexpr (token == TOKENS::DIVEQUAL) return "/=";
	if constexpr (token == TOKENS::MODEQUAL) return "%=";
	if constexpr (token == TOKENS::ANDEQUAL) return "&=";
	if constexpr (token == TOKENS::OREQUAL) return "|=";
	if constexpr (token == TOKENS::LTE) return "<=";
	if constexpr (token == TOKENS::GTE) return ">=";
	if constexpr (token == TOKENS::XOREQUAL) return "^=";
	if constexpr (token == TOKENS::AND || token == TOKENS::ANDAND) return "&&";
	if constexpr (token == TOKENS::OR || token == TOKENS::OROR) return "||";
	if constexpr (token == TOKENS::RETURN) return "return";
	if constexpr (token == TOKENS::SWITCH) return "switch";
	if constexpr (token == TOKENS::FOR) return "for";
	if constexpr (token == TOKENS::WHILE) return "while";
	if constexpr (token == TOKENS::IF) return "if";
	if constexpr (token == TOKENS::ELSE) return "else";
	if constexpr (token == TOKENS::BREAK) return "break";
	if constexpr (token == TOKENS::CASE) return "case";
	if constexpr (token == TOKENS::DO) return "do";
	if constexpr (token == TOKENS::USING) return "using";
	if constexpr (token == TOKENS::STATIC) return "static";
	if constexpr (token == TOKENS::TYPE) return "struct";
	if constexpr (token == TOKENS::TEMPLATE) return "template";
	if constexpr (token == TOKENS::AUTO) return "auto";
	throw std::runtime_error("invalid token");
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

struct Named {
	template <typename T> using map2ptr = std::map<std::string, const T*>;
	map2ptr<NodeStructs::Function> functions;
	map2ptr<NodeStructs::Template<const NodeStructs::Function*>> function_templates;
	map2ptr<NodeStructs::Type> types;
	map2ptr<NodeStructs::Template<const NodeStructs::Type*>> type_templates;
	map2ptr<NodeStructs::Block> blocks;
	map2ptr<NodeStructs::Template<const NodeStructs::Block*>> block_templates;
};

struct cpp_std {
	const NodeStructs::Type _uset = NodeStructs::Type{
		.name = std::string{"Set"},
		//.name = std::string{"std::unordered_set"},
		//.aliases = std::vector<NodeStructs::Alias>{},
		//.constructors = std::vector<NodeStructs::Constructor>{},
		.methods = std::vector<NodeStructs::Function>{},
		.memberVariables = std::vector<NodeStructs::MemberVariable>{},
	};
	const NodeStructs::Template<const NodeStructs::Type*> unordered_set = {
		std::vector<std::variant<std::string, NodeStructs::Template<std::string>>>{std::string{"T"}},
		&_uset,
	};
	const NodeStructs::Type _vec = NodeStructs::Type{
		.name = std::string{"Vector"},
		//.name = std::string{"std::vector"},
		//.aliases = std::vector<NodeStructs::Alias>{},
		//.constructors = std::vector<NodeStructs::Constructor>{},
		.methods = std::vector<NodeStructs::Function>{},
		.memberVariables = std::vector<NodeStructs::MemberVariable>{},
	};
	const NodeStructs::Template<const NodeStructs::Type*> vector = {
		std::vector<std::variant<std::string, NodeStructs::Template<std::string>>>{std::string{"T"}},
		&_vec,
	};
	static NodeStructs::Function _map_at() {
		NodeStructs::Function test = NodeStructs::Function{
				"at",
				NodeStructs::BaseTypename{"V"},
				std::vector<std::pair<NodeStructs::Typename, std::string>>{/*{NodeStructs::BaseTypename{"V"}, ""}*/},
				std::vector<NodeStructs::Statement>{},
		};
		return test;
	};
	static NodeStructs::Template<const NodeStructs::Type*> create_unordered_map_type_template() {
		static NodeStructs::Type _map = NodeStructs::Type{
				.name = std::string{"Map"},
				//.name = std::string{"std::unordered_map"},
				//.aliases = std::vector<NodeStructs::Alias>{},
				//.constructors = std::vector<NodeStructs::Constructor>{},
				.methods = std::vector<NodeStructs::Function>{/* testf()*/ },
				.memberVariables = std::vector<NodeStructs::MemberVariable>{},
		};
		_map.methods.push_back(_map_at());
		return {
			std::vector<std::variant<std::string, NodeStructs::Template<std::string>>>{"K", "V"},
			&_map,
		};
	}
	const NodeStructs::Template<const NodeStructs::Type*> unordered_map = create_unordered_map_type_template();
	static NodeStructs::Template<const NodeStructs::Type*> create_pair_type_template() {
		static NodeStructs::Type _pair = NodeStructs::Type{
				.name = std::string{"Pair"},
				//.name = std::string{"std::unordered_map"},
				//.aliases = std::vector<NodeStructs::Alias>{},
				//.constructors = std::vector<NodeStructs::Constructor>{},
				.methods = std::vector<NodeStructs::Function>{/* testf()*/ },
				.memberVariables = std::vector<NodeStructs::MemberVariable>{},
		};
		return {
			std::vector<std::variant<std::string, NodeStructs::Template<std::string>>>{"First", "Second"},
			&_pair,
		};
	}
	const NodeStructs::Template<const NodeStructs::Type*> pair = create_pair_type_template();
	const NodeStructs::Type _int = {
		.name = "Int",
	};
	const NodeStructs::Type _bool = {
		.name = "Bool",
	};
	const NodeStructs::Type string = {
		.name = "String",
	};
};

static constexpr std::string_view default_includes = 
	//"#pragma once\n"
	//"#include <memory>\n"
	//"#include <fstream>\n"
	"#include <utility>\n"

	"using Int = int;\n"
	"using Bool = bool;\n"
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

	"\n";

void insert_all_named_recursive_with_imports(const std::vector<NodeStructs::File>& project, Named& named, const std::string& filename);
std::string transpile(const std::vector<NodeStructs::File>& project);

std::string transpile_main(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	Named& named,
	const NodeStructs::Function& fn
);

std::string transpile(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const NodeStructs::Function& fn
);

std::string transpile(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const NodeStructs::Type& type
);

std::string transpile(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const std::vector<std::pair<NodeStructs::Typename, std::string>>& parameters
);

std::string transpile(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const NodeStructs::Typename& type
);

std::string transpile(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const NodeStructs::BaseTypename& type
);

std::string transpile(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const NodeStructs::NamespacedTypename& type
);

std::string transpile(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const NodeStructs::TemplatedTypename& type
);

std::string transpile(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const std::vector<NodeStructs::Statement>& statements
);

std::string transpile_statement(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const NodeStructs::Statement& statement
);

std::vector<NodeStructs::TypeOrTypeTemplateInstance> decomposed_type(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const NodeStructs::TypeOrTypeTemplateInstance& type
);

void add_for_iterator_variables(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const NodeStructs::ForStatement& statement,
	const NodeStructs::TypeOrTypeTemplateInstance& it_type
);

void remove_for_iterator_variables(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const NodeStructs::ForStatement& statement
);

void remove_added_variables(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const NodeStructs::Statement& statement
);

std::string transpile_statement(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const NodeStructs::VariableDeclarationStatement& statement
);

std::string transpile_statement(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const NodeStructs::BlockStatement& statement
);

std::string transpile_statement(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const NodeStructs::IfStatement& statement
);

std::string transpile_statement(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const NodeStructs::ForStatement& statement
);

std::string transpile_statement(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const NodeStructs::IForStatement& statement
);

std::string transpile_statement(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const NodeStructs::WhileStatement& statement
);

std::string transpile_statement(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const NodeStructs::BreakStatement& statement
);

std::string transpile_statement(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const NodeStructs::ReturnStatement& statement
);




// expressions

std::string transpile(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const NodeStructs::Expression& expr
);

std::string transpile_statement(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const NodeStructs::Expression& statement
);

std::string transpile(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const Token<NUMBER>& expr
);

std::string transpile(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const std::string& expr
);

std::string transpile(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const NodeStructs::AssignmentExpression& expr
);

std::string transpile(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const NodeStructs::ConditionalExpression& expr
);

std::string transpile(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const NodeStructs::OrExpression& expr
);

std::string transpile(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const NodeStructs::AndExpression& expr
);

std::string transpile(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const NodeStructs::EqualityExpression& expr
);

std::string transpile(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const NodeStructs::CompareExpression& expr
);

std::string transpile(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const NodeStructs::AdditiveExpression& expr
);

std::string transpile(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const NodeStructs::MultiplicativeExpression& expr
);

std::string transpile(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const NodeStructs::UnaryExpression& expr
);

std::string transpile(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const NodeStructs::PostfixExpression& expr
);

std::string transpile(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const NodeStructs::ParenExpression& expr
);

std::string transpile(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const NodeStructs::BraceExpression& expr
);

std::string transpile_args(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const std::vector<NodeStructs::Expression>& args
);
