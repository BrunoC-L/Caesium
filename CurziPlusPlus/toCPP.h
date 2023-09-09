#pragma once
#include <fstream>
#include <string>
#include <sstream>
#include <map>

#include "node_structs.h"

template <size_t token>
std::string symbol_as_text() {
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
std::string _symbol_as_text(Token<token>) {
	return symbol_as_text<token>();
}

template <size_t... tokens>
std::string symbol_variant_as_text(const std::variant<Token<tokens>...>& token) {
	return std::visit(
		[&](const auto& tk) { return _symbol_as_text(tk); },
		token
	);
}

struct Named {
	template <typename T> using map2ptr = std::map<std::string, const T*>;
	map2ptr<NodeStructs::Function> functions;
	map2ptr<NodeStructs::Template<NodeStructs::Function>> function_templates;
	map2ptr<NodeStructs::Type> types;
	map2ptr<NodeStructs::Template<NodeStructs::Type>> type_templates;
	map2ptr<NodeStructs::Block> blocks;
	map2ptr<NodeStructs::Template<NodeStructs::Block>> block_templates;
};

struct cpp_std {
	const NodeStructs::Template<NodeStructs::Type> unordered_set = {
		std::vector<std::variant<std::string, NodeStructs::Template<std::string>>>{std::string{"T"}},
		NodeStructs::Type{
			.name = std::string{"std::unordered_set"},
			//.aliases = std::vector<NodeStructs::Alias>{},
			//.constructors = std::vector<NodeStructs::Constructor>{},
			.methods = std::vector<NodeStructs::Function>{},
			.memberVariables = std::vector<NodeStructs::MemberVariable>{},
		}
	};
	const NodeStructs::Template<NodeStructs::Type> vector = {
		std::vector<std::variant<std::string, NodeStructs::Template<std::string>>>{std::string{"T"}},
		NodeStructs::Type{
			.name = std::string{"std::vector"},
			//.aliases = std::vector<NodeStructs::Alias>{},
			//.constructors = std::vector<NodeStructs::Constructor>{},
			.methods = std::vector<NodeStructs::Function>{},
			.memberVariables = std::vector<NodeStructs::MemberVariable>{},
		}
	};
	const NodeStructs::Template<NodeStructs::Type> unordered_map = {
		std::vector<std::variant<std::string, NodeStructs::Template<std::string>>>{std::string{"T"}},
		NodeStructs::Type{
			.name = std::string{"std::unordered_map"},
			//.aliases = std::vector<NodeStructs::Alias>{},
			//.constructors = std::vector<NodeStructs::Constructor>{},
			.methods = std::vector<NodeStructs::Function>{},
			.memberVariables = std::vector<NodeStructs::MemberVariable>{},
		}
	};
	const NodeStructs::Type _int = {
		.name = "int",
	};
	const NodeStructs::Type string = {
		.name = "std::string",
	};
};

class toCPP {
public:
	static constexpr auto default_includes = std::string_view("#pragma once\n"
		"#include <memory>\n"
		"#include <utility>\n"
		"#include <variant>\n"
		"#include <vector>\n"
		"#include <string>\n"
		"#include <sstream>\n"
		"#include <fstream>\n"
		"\n");

	void insert_all_named_recursive_with_imports(const std::vector<NodeStructs::File>& project, Named& named, const std::string& filename) {
		for (const NodeStructs::File& file : project)
			if (file.filename == filename) {
				for (const auto& e : file.types)
					named.types[e.name] = &e;
				for (const auto& e : file.functions)
					named.functions[e.name] = &e;
				for (const auto& e : file.blocks)
					named.blocks[e.name] = &e;
				for (const auto& i : file.imports)
					insert_all_named_recursive_with_imports(project, named, i.imported);
				return;
			}
		auto err = std::string("Invalid import \"") + filename + "\"";
		throw std::runtime_error(err);
	}

	const NodeStructs::Template<NodeStructs::Type>& type_template_of_typename(
		std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
		const Named& named,
		const NodeStructs::BaseTypename& type
	) {
		if (named.type_templates.contains(type.type))
			return *named.type_templates.at(type.type);
		auto err = "Missing type " + type.type;
		throw std::runtime_error(err);
	}

	const NodeStructs::Template<NodeStructs::Type>& type_template_of_typename(
		std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
		const Named& named,
		const NodeStructs::NamespacedTypename& type
	) {
		auto err = "Missing type ";
		throw std::runtime_error(err);
	}

	const NodeStructs::Template<NodeStructs::Type>& type_template_of_typename(
		std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
		const Named& named,
		const NodeStructs::TemplatedTypename& type
	) {
		const auto& templated = type_template_of_typename(variables, named, *type.type.get());
		auto err = "Missing type ";
		throw std::runtime_error(err);
	}

	const NodeStructs::Template<NodeStructs::Type>& type_template_of_typename(
		std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
		const Named& named,
		const NodeStructs::Typename& type
	) {
		return std::visit(
			[&](const auto& t) -> const NodeStructs::Template<NodeStructs::Type>& {
				auto s = transpile(variables, named, type);
				return type_template_of_typename(variables, named, t);
			},
			type
		);
	}

	NodeStructs::TypeOrTypeTemplateInstance type_of_typename(
		std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
		const Named& named,
		const NodeStructs::BaseTypename& type
	) {
		if (named.types.contains(type.type))
			return named.types.at(type.type);
		auto err = "Missing type " + type.type;
		throw std::runtime_error(err);
	}

	NodeStructs::TypeOrTypeTemplateInstance type_of_typename(
		std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
		const Named& named,
		const NodeStructs::NamespacedTypename& type
	) {
		auto err = "Missing type ";
		throw std::runtime_error(err);
	}

	NodeStructs::TypeOrTypeTemplateInstance type_of_typename(
		std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
		const Named& named,
		const NodeStructs::TemplatedTypename& type
	) {
		const auto& templated = type_template_of_typename(variables, named, *type.type.get());
		auto templated_with = std::vector<NodeStructs::TypeOrTypeTemplateInstance>{};
		for (const auto& t : type.templated_with)
			templated_with.push_back(type_of_typename(variables, named, t));
		return NodeStructs::TypeTemplateInstance{
			templated,
			templated_with
		};
	}

	NodeStructs::TypeOrTypeTemplateInstance type_of_typename(
		std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
		const Named& named,
		const NodeStructs::Typename& type
	) {
		auto s = transpile(variables, named, type);
		return std::visit(
			[&](const auto& t) {
				return type_of_typename(variables, named, t);
			},
			type
		);
	}

	NodeStructs::TypeOrTypeTemplateInstance type_of_expr(
		std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
		const Named& named,
		const NodeStructs::Expression& expr
	) {
		std::string s = transpile(variables, named, expr);
		if (variables.contains(s)) {
			return variables.at(s).back();
		}
		std::string err = "failed to get type of variable " + s;
		throw std::runtime_error(err);
	}

	std::vector<NodeStructs::TypeOrTypeTemplateInstance> decomposed_type(
		std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
		const Named& named,
		const NodeStructs::TypeOrTypeTemplateInstance& type
	) {
		return std::visit(
			overload(
				[&](const NodeStructs::Type* type) {
					std::vector<NodeStructs::TypeOrTypeTemplateInstance> res;
					res.reserve(type->memberVariables.size());
					throw std::runtime_error("");
					for (const auto& mv : type->memberVariables)
						res.push_back(type_of_typename(variables, named, mv.type));
					return res;
				},
				[&](const NodeStructs::TypeTemplateInstance& type) -> std::vector<NodeStructs::TypeOrTypeTemplateInstance> {
					if (type.type_template.templated.name == "std::unordered_set")
						if (type.template_arguments.size() == 1)
							return { type.template_arguments.at(0) };
						else
							throw std::runtime_error("");
					else
						if (type.type_template.templated.name == "std::vector")
							if (type.template_arguments.size() == 1)
								return { type.template_arguments.at(0) };
							else
								throw std::runtime_error("");
					int a = 0;
					throw std::runtime_error("");
				}
			),
			type
		);
	}

	std::string transpile(const std::vector<NodeStructs::File>& project) {
		std::map<std::string, Named> named_by_file;
		std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>> variables;

		for (const auto& file : project) {
			Named named_of_file;
			insert_all_named_recursive_with_imports(project, named_of_file, file.filename);
			named_by_file[file.filename] = named_of_file;
		}

		for (const auto& file : project)
			for (const auto& fn : file.functions)
				if (fn.name == "main") {
					return transpile_main(variables, named_by_file[file.filename], fn);
				}
		throw std::runtime_error("Missing \"main\" function\n");
	}

	std::string transpile_main(
		std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
		Named& named,
		const NodeStructs::Function& fn
	) {
		cpp_std stuff_from_cpp {};
		named.type_templates["Set"] = &stuff_from_cpp.unordered_set;
		named.type_templates["Vector"] = &stuff_from_cpp.vector;
		named.type_templates["Map"] = &stuff_from_cpp.unordered_map;
		named.types["Int"] = &stuff_from_cpp._int;
		named.types["String"] = &stuff_from_cpp.string;

		if (fn.parameters.size() > 1)
			throw std::runtime_error("\"main\" function declaration requires 0 or 1 argument\n");
		if (fn.parameters.size() == 0)
			return transpile(variables, named, fn);
		else {
			const auto& [parameter_type, parameter_name] = fn.parameters.at(0);
			const auto std_vector_str = []() {
				auto std_vector = NodeStructs::NamespacedTypename{
					std::make_unique<NodeStructs::Typename>(NodeStructs::BaseTypename{ "std" }),
					std::make_unique<NodeStructs::Typename>(NodeStructs::BaseTypename{ "vector" })
				};
				auto std_string = NodeStructs::NamespacedTypename{
					std::make_unique<NodeStructs::Typename>(NodeStructs::BaseTypename{ "std" }),
					std::make_unique<NodeStructs::Typename>(NodeStructs::BaseTypename{ "string" })
				};
				auto res = NodeStructs::TemplatedTypename {
					std::make_unique<NodeStructs::Typename>(NodeStructs::Typename{ std::move(std_vector) }),
					{}
				};
				res.templated_with.push_back(NodeStructs::Typename{ std::move(std_string) });
				return res;
			}();
			bool is_vec_str = parameter_type == std_vector_str;

			if (!is_vec_str)
				throw std::runtime_error("\"main\" function declaration using 1 argument must be of std::vector<std::string> type\n");

			const_cast<NodeStructs::Function&>(fn).name = "_main";
			std::stringstream ss;
			ss << transpile(variables, named, fn);
			ss << "int main(int argc, char** argv) {\n"
				"std::vector<std::string> args {};\n"
				"for (int i = 0; i < argc; ++i)\n"
				"    args.push_back(std::string(argv[i]));"
				"return _main(std::move(args));\n"
				"};\n";
			return ss.str();
		}
	}

	std::string transpile(
		std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
		const Named& named,
		const NodeStructs::Function& fn
	) {
		return transpile(variables, named, fn.returnType) + " " +
			fn.name + "(" + transpile(variables, named, fn.parameters) +
			") {\n" +
			transpile(variables, named, fn.statements) +
			"};";
	}

	std::string transpile(
		std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
		const Named& named,
		const std::vector<std::pair<NodeStructs::Typename, std::string>>& parameters
	) {
		std::stringstream ss;
		bool first = true;
		for (const auto& [type, name] : parameters) {
			ss << transpile(variables, named, type) << " " << name;
			if (first)
				first = false;
			else
				ss << ", ";
		}
		return ss.str();
	}

	std::string transpile(
		std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
		const Named& named,
		const NodeStructs::Typename& type
	) {
		return std::visit(
			[&](const auto& type) {
				return transpile(variables, named, type);
			},
			type
		);
	}

	std::string transpile(
		std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
		const Named& named,
		const NodeStructs::BaseTypename& type
	) {
		return type.type;
	}

	std::string transpile(
		std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
		const Named& named,
		const NodeStructs::NamespacedTypename& type
	) {
		return transpile(variables, named, *type.name_space) + "::" + transpile(variables, named, *type.name_in_name_space);
	}

	std::string transpile(
		std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
		const Named& named,
		const NodeStructs::TemplatedTypename& type
	) {
		std::stringstream ss;
		ss << transpile(variables, named, *type.type) << "<";
		bool first = true;
		for (const auto& t : type.templated_with) {
			if (first)
				first = false;
			else
				ss << ", ";
			ss << transpile(variables, named, t);
		}
		ss << ">";
		return ss.str();
	}

	std::string transpile(
		std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
		const Named& named,
		const std::vector<NodeStructs::Statement>& statements
	) {
		std::stringstream ss;
		for (const auto& statement : statements)
			ss << transpile_statement(variables, named, statement);
		for (const auto& statement : statements)
			remove_added_variables(variables, named, statement);
		return ss.str();
	}

	std::string transpile_statement(
		std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
		const Named& named,
		const NodeStructs::Statement& statement
	) {
		return std::visit(
			overload(
				[&](const auto& e) {
					return transpile_statement(variables, named, e);
				}
			),
			statement.statement
		);
	}

	void remove_added_variables(
		std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
		const Named& named,
		const NodeStructs::Statement& statement
	) {
		std::visit(
			overload(
				[&](const NodeStructs::VariableDeclarationStatement& declaration) {
					variables[declaration.name].pop_back();
				},
				[&](const NodeStructs::BlockStatement& statement) {
					auto s = std::get<NodeStructs::BaseTypename>(statement.parametrized_block).type;

					if (named.blocks.contains(s)) {
						const NodeStructs::Block& block = *named.blocks.at(s);
						for (const auto& statement : block.statements)
							remove_added_variables(variables, named, statement);
					}
					else {
						throw std::runtime_error("bad block name" + s);
					}
				},
				[&](const auto&) { }
			),
			statement.statement
		);
	}

	std::string transpile_statement(
		std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
		const Named& named,
		const NodeStructs::VariableDeclarationStatement& statement
	) {
		variables[statement.name].push_back(type_of_typename(variables, named, statement.type));
		return "\t" + transpile(variables, named, statement.type) + " " + statement.name + ";\n";
	}

	std::string transpile_statement(
		std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
		const Named& named,
		const NodeStructs::BlockStatement& statement
	) {
		auto s = std::get<NodeStructs::BaseTypename>(statement.parametrized_block).type;
		if (named.blocks.contains(s)) {
			const NodeStructs::Block& block = *named.blocks.at(s);
			std::stringstream ss;
			for (const auto& statement : block.statements)
				ss << transpile_statement(variables, named, statement);
			return ss.str();
		}
		else {
			throw std::runtime_error("bad block name" + s);
		}
		return "";
	}

	std::string transpile_statement(
		std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
		const Named& named,
		const NodeStructs::IfStatement& statement
	) {
		if (statement.elseExprStatements.has_value())
			return "\tif (" +
			transpile(variables, named, statement.ifExpr) +
			") {\n" +
			transpile(variables, named, statement.ifStatements) +
			"} else " +
			std::visit(
				overload(
					[&](const std::unique_ptr<NodeStructs::IfStatement>& elseif) {
						return transpile_statement(variables, named, *elseif.get());
					},
					[&](const std::vector<NodeStructs::Statement>& justelse) {
						return "{" + transpile(variables, named, justelse) + "}";
					}
				),
				statement.elseExprStatements.value()
			);
		else
			return "\tif (" +
			transpile(variables, named, statement.ifExpr) +
			") {\n" +
			transpile(variables, named, statement.ifStatements) +
			"}";
	}

	std::string transpile_statement(
		std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
		const Named& named,
		const NodeStructs::ForStatement& statement
	) {
		add_for_iterator_variables(variables, named, statement);
		
		std::stringstream ss;
		ss << "for (auto&& [";


		remove_for_iterator_variables(variables, statement);
		return ss.str();
	}

	void add_for_iterator_variables(
		std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
		const Named& named,
		const NodeStructs::ForStatement& statement
	) {
		auto decomposed_types = decomposed_type(variables, named, type_of_expr(variables, named, statement.collection));
		for (int i = 0; i < statement.iterators.size(); ++i) {
			const auto& iterator = statement.iterators.at(i);
			std::visit(
				overload(
					[&](const NodeStructs::VariableDeclaration& it) {
						variables[it.name].push_back(type_of_typename(variables, named, it.type));
					},
					[&](const std::string& it) {
						variables[it].emplace_back(decomposed_types.at(i));
					}
				),
				iterator
			);
		}
	}

	void remove_for_iterator_variables(
		std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
		const NodeStructs::ForStatement& statement
	) {
		for (const auto& iterator : statement.iterators)
			std::visit(
				overload(
					[&](const NodeStructs::VariableDeclaration& it) {
						variables[it.name].pop_back();
					},
					[&](const std::string& it) {
						variables[it].pop_back();
					}
				),
				iterator
			);
	}

	std::string transpile_statement(
		std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
		const Named& named,
		const NodeStructs::IForStatement& statement
	) {
		throw std::runtime_error("not implemented");
		return "";
	}

	std::string transpile_statement(
		std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
		const Named& named,
		const NodeStructs::WhileStatement& statement
	) {
		throw std::runtime_error("not implemented");
		return "";
	}

	std::string transpile_statement(
		std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
		const Named& named,
		const NodeStructs::BreakStatement& statement
	) {
		if (statement.ifExpr.has_value())
			return "if (" + transpile(variables, named, statement.ifExpr.value()) + ") break;\n";
		else
			return "break;\n";
	}

	std::string transpile_statement(
		std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
		const Named& named,
		const NodeStructs::ReturnStatement& statement
	) {
		std::string return_value = [&]() {
			if (statement.returnExpr.size() == 0)
				return std::string{};
			else if (statement.returnExpr.size() == 1)
				return transpile(variables, named, statement.returnExpr.at(0));
			else
				return "{" + transpile_args(variables, named, statement.returnExpr) + "}";
		}();
		if (statement.ifExpr.has_value())
			return "if (" + transpile(variables, named, statement.ifExpr.value()) + ") return " + return_value + ";\n";
		else
			return "return " + return_value + ";\n";
	}




	// expressions

	std::string transpile(
		std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
		const Named& named,
		const NodeStructs::Expression& expr
	) {
		return std::visit(
			overload(
				[&](const auto& expr) -> std::string {
					return transpile(variables, named, expr);
				}
			),
			*expr.expression.get()
		);
		/*std::stringstream ss;
		ss << transpile(variables, named, expr.expr);
		for (const auto& statement : expr.assignments)
			ss << " " << symbol_variant_as_text(statement.first) <<
			" " << transpile(variables, named, statement.second);
		return ss.str();*/
	}

	std::string transpile_statement(
		std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
		const Named& named,
		const NodeStructs::Expression& statement
	) {
		return transpile(variables, named, statement) + ";\n";
	}

	std::string transpile(
		std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
		const Named& named,
		const Token<NUMBER>& expr
	) {
		return expr.value;
	}

	std::string transpile(
		std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
		const Named& named,
		const NodeStructs::AssignmentExpression& expr
	) {
		std::stringstream ss;
		ss << transpile(variables, named, expr.expr);
		for (const auto& expr : expr.assignments)
			ss << symbol_variant_as_text(expr.first) << transpile(variables, named, expr.second);
		return ss.str();
	}

	std::string transpile(
		std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
		const Named& named,
		const NodeStructs::ConditionalExpression& expr
	) {
		if (expr.ifElseExprs.has_value()) {
			// x if first else second
			//                  ([&] () { if (first) return x; else return second; }());
			return std::string("([&] () { if (") +
				transpile(variables, named, expr.ifElseExprs.value().first) +
				") return " +
				transpile(variables, named, expr.expr) +
				"; else return " +
				transpile(variables, named, expr.ifElseExprs.value().second) +
				"; }())";
		}
		else {
			return transpile(variables, named, expr.expr);
		}
	}

	std::string transpile(
		std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
		const Named& named,
		const NodeStructs::OrExpression& expr
	) {
		std::stringstream ss;
		ss << transpile(variables, named, expr.expr);
		for (const auto& expr : expr.ors)
			ss << " || " << transpile(variables, named, expr);
		return ss.str();
	}

	std::string transpile(
		std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
		const Named& named,
		const NodeStructs::AndExpression& expr
	) {
		std::stringstream ss;
		ss << transpile(variables, named, expr.expr);
		for (const auto& expr : expr.ands)
			ss << " && " << transpile(variables, named, expr);
		return ss.str();
	}

	std::string transpile(
		std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
		const Named& named,
		const NodeStructs::EqualityExpression& expr
	) {
		std::stringstream ss;
		ss << transpile(variables, named, expr.expr);
		for (const auto& [op, expr] : expr.equals)
			ss << " " << symbol_variant_as_text(op) << " " << transpile(variables, named, expr);
		return ss.str();
	}

	std::string transpile(
		std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
		const Named& named,
		const NodeStructs::CompareExpression& expr
	) {
		std::stringstream ss;
		ss << transpile(variables, named, expr.expr);
		for (const auto& [op, expr] : expr.comparisons)
			ss << " " << symbol_variant_as_text(op) << " " << transpile(variables, named, expr);
		return ss.str();
	}

	std::string transpile(
		std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
		const Named& named,
		const NodeStructs::AdditiveExpression& expr
	) {
		std::stringstream ss;
		ss << transpile(variables, named, expr.expr);
		for (const auto& [op, expr] : expr.adds)
			ss << " " << symbol_variant_as_text(op) << " " << transpile(variables, named, expr);
		return ss.str();
	}

	std::string transpile(
		std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
		const Named& named,
		const NodeStructs::MultiplicativeExpression& expr
	) {
		std::stringstream ss;
		ss << transpile(variables, named, expr.expr);
		for (const auto& [op, expr] : expr.muls)
			ss << " " << symbol_variant_as_text(op) << " " << transpile(variables, named, expr);
		std::string x = ss.str();
		return ss.str();
	}

	std::string transpile(
		std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
		const Named& named,
		const NodeStructs::UnaryExpression& expr
	) {
		throw std::runtime_error("");
		/*return std::visit(
			overload(
				[&](const std::pair<NodeStructs::UnaryExpression::op_types, std::unique_ptr<NodeStructs::UnaryExpression>>& op_expr) {
					return std::visit(
						overload(
							[&](const NodeStructs::Typename& type) {
								return "(" + transpile(variables, named, type) + ")";
							},
							[&](const auto& op) {
								return _symbol_as_text(op);
							}
						),
						op_expr.first
					) + " " + transpile(variables, named, *op_expr.second.get());
				},
				[&](const NodeStructs::PostfixExpression& expr) {
					return transpile(variables, named, expr);
				}
			),
			expr.expr
		);*/
	}

	std::string transpile(
		std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
		const Named& named,
		const NodeStructs::PostfixExpression& expr
	) {
		std::stringstream ss;
		ss << transpile(variables, named, expr.expr);
		for (const auto& op : expr.postfixes)
			ss << std::visit(
				overload(
					[&](const std::string& property_name) {
						return "." + property_name;
					},
					[&](const NodeStructs::ParenExpression& e) {
						return "(" + transpile_args(variables, named, e.args) + ")";
					},
					[&](const NodeStructs::BracketArguments& e) {
						return "[" + transpile_args(variables, named, e.args) + "]";
					},
					[&](const NodeStructs::BraceExpression& e) {
						return "{" + transpile_args(variables, named, e.args) + "}";
					},
					[&](const auto& op) {
						return _symbol_as_text(op);
					}
				),
				op
			);
		return ss.str();
	}

	std::string transpile(
		std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
		const Named& named,
		const NodeStructs::ParenExpression& expr
	) {
		return "(" + transpile_args(variables, named, expr.args) + ")";
	}

	std::string transpile(
		std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
		const Named& named,
		const NodeStructs::BraceExpression& expr
	) {
		return "{" + transpile_args(variables, named, expr.args) + "}";
	}

	std::string transpile_args(
		std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
		const Named& named,
		const std::vector<NodeStructs::Expression>& args
	) {
		std::stringstream ss;
		bool first = true;
		for (const auto& arg : args) {
			if (first)
				first = false;
			else
				ss << ", ";
			ss << transpile(variables, named, arg);
		}
		return ss.str();
	}
};
