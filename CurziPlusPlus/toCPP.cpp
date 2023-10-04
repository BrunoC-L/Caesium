#include "toCPP.h"
#include "type_of_expr.h"
#include "type_of_typename.h"

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
std::string transpile(const std::vector<NodeStructs::File>& project) {
	for (const auto& file : project)
		for (const auto& fn : file.functions)
			if (fn.name == "main") {

				std::map<std::string, Named> named_by_file;
				std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>> variables;

				cpp_std stuff_from_cpp{};

				for (const auto& file : project) {
					Named named_of_file;
					insert_all_named_recursive_with_imports(project, named_of_file, file.filename);
					{
						named_of_file.type_templates["Set"] = &stuff_from_cpp.unordered_set;
						named_of_file.type_templates["Vector"] = &stuff_from_cpp.vector;
						named_of_file.type_templates["Map"] = &stuff_from_cpp.unordered_map;
						named_of_file.type_templates["Pair"] = &stuff_from_cpp.pair;
						named_of_file.types["Int"] = &stuff_from_cpp._int;
						named_of_file.types["Bool"] = &stuff_from_cpp._bool;
						named_of_file.types["String"] = &stuff_from_cpp.string;
					}
					named_by_file[file.filename] = named_of_file;
				}
				std::stringstream ss;
				ss << default_includes;

				ss << "struct Main {\n";

				for (const auto& file2 : project) {
					for (const auto& fn2 : file2.functions)
						if (fn2.name != "main")
							ss << transpile(variables, named_by_file[file.filename], fn2);
					for (const auto& t : file2.types)
						ss << transpile(variables, named_by_file[file.filename], t);
				}

				ss
				<< transpile_main(variables, named_by_file[file.filename], fn)
				<<  "};\n"
					"int main(int argc, char** argv) {\n"
					"std::vector<std::string> args {};\n"
					"for (int i = 0; i < argc; ++i)\n"
					"    args.push_back(std::string(argv[i]));"
					"return Main{}.main(std::move(args));\n"
					"};\n";

				return ss.str();
			}
	throw std::runtime_error("Missing \"main\" function\n");
}

std::string transpile_main(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	Named& named,
	const NodeStructs::Function& fn
) {

	if (fn.parameters.size() != 1)
		throw std::runtime_error("\"main\" function declaration requires 1 argument of type Vector<String>\n");

	const auto& [parameter_type, parameter_name] = fn.parameters.at(0);
	const auto vector_str = []() {
		auto res = NodeStructs::TemplatedTypename{
			NodeStructs::BaseTypename{ "Vector" },
			{}
		};
		res.templated_with.push_back(NodeStructs::BaseTypename{ "String" });
		return res;
	}();
	bool is_vec_str = parameter_type == vector_str;

	if (!is_vec_str)
		throw std::runtime_error("\"main\" function declaration using 1 argument must be of std::vector<std::string> type\n");

	return transpile(
		variables,
		named,
		NodeStructs::Function{
			.name = "main",
			.returnType = fn.returnType,
			.parameters = fn.parameters,
			.statements = fn.statements
		});
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
		"};\n";
}

std::string transpile(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const NodeStructs::Constructor& fn,
	const NodeStructs::Type& type
) {
	return type.name + "(" + transpile(variables, named, fn.parameters) +
		") {\n" +
		transpile(variables, named, fn.statements) +
		"};";
}

std::string transpile(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const NodeStructs::Type& type
) {
	std::stringstream ss;
	ss << "struct " << type.name << " {\n";

	for (const auto& alias : type.aliases)
		if (std::holds_alternative<NodeStructs::BaseTypename>(alias.aliasTo))
			ss << "using " << std::get<NodeStructs::BaseTypename>(alias.aliasTo).type << " = " << transpile(variables, named, alias.aliasFrom) << ";\n";
		else {
			auto err = "cannot alias to " + transpile(variables, named, alias.aliasTo);
			throw std::runtime_error(err);
		}

	for (const auto& member : type.memberVariables)
		ss << transpile(variables, named, member.type) << " " << member.name << ";\n";

	if (type.constructors.size()) {
		for (const auto& constructor : type.constructors)
			ss << transpile(variables, named, constructor, type) << "\n";
		ss << type.name << "(const " << type.name << "&) = default;\n";
		ss << type.name << "& operator=(const " << type.name << "&) = default;\n";
		ss << type.name << "(" << type.name << "&&) = default;\n";
		ss << type.name << "& operator=(" << type.name << "&&) = default;\n";
		ss << "~" << type.name << "() = default;\n";
	}

	for (const auto& fn : type.methods)
		ss << transpile(variables, named, fn);

	ss << "};\n\n";
	return ss.str();
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
	return transpile(variables, named, type.name_space.get()) + "::" + transpile(variables, named, type.name_in_name_space.get());
}

std::string transpile(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const NodeStructs::TemplatedTypename& type
) {
	std::stringstream ss;
	ss << transpile(variables, named, type.type.get()) << "<";
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
			[&](const auto&) {}
		),
		statement.statement
	);
}

std::string transpile_statement(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const NodeStructs::VariableDeclarationStatement& statement
) {
	variables[statement.name].push_back(std::move(type_of_typename(variables, named, statement.type)));
	return transpile(variables, named, statement.type) + " " + statement.name + " = " + transpile(variables, named, statement.expr) + ";\n";
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
		return "if (" +
		transpile(variables, named, statement.ifExpr) +
		") {\n" +
		transpile(variables, named, statement.ifStatements) +
		"} else " +
		std::visit(
			overload(
				[&](const Allocated<NodeStructs::IfStatement>& elseif) {
					return transpile_statement(variables, named, elseif.get());
				},
				[&](const std::vector<NodeStructs::Statement>& justelse) {
					return "{" + transpile(variables, named, justelse) + "}";
				}
					),
			statement.elseExprStatements.value()
		);
	else
		return "if (" +
		transpile(variables, named, statement.ifExpr) +
		") {\n" +
		transpile(variables, named, statement.ifStatements) +
		"}";
}

NodeStructs::TypeOrTypeTemplateInstance iterator_type(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const NodeStructs::TypeOrTypeTemplateInstance& type
) {
	return std::visit(
		overload(
			[&](const NodeStructs::Type* type) -> NodeStructs::TypeOrTypeTemplateInstance {
				throw std::runtime_error("");
			},
			[&](const NodeStructs::TypeTemplateInstance& type) -> NodeStructs::TypeOrTypeTemplateInstance {
				const auto tn = type.type_template->templated.name;
				if (tn == "Set")
					if (type.template_arguments.size() == 1)
						return type.template_arguments.at(0);
					else
						throw std::runtime_error("");
				else
					if (tn == "Vector")
						if (type.template_arguments.size() == 1)
							return type.template_arguments.at(0);
						else
							throw std::runtime_error("");
					else
						if (tn == "Map")
							if (type.template_arguments.size() == 2)
								return NodeStructs::TypeTemplateInstance{
									.type_template = named.type_templates.at("Map"),
									.template_arguments = { type.template_arguments.at(0), type.template_arguments.at(1) },
								};
							else
								throw std::runtime_error("");
				int a = 0;
				throw std::runtime_error("");
			}
		),
		type
	);
}

std::vector<NodeStructs::TypeOrTypeTemplateInstance> decomposed_type(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const NodeStructs::TypeOrTypeTemplateInstance& type
) {
	return std::visit(
		overload(
			[&](const NodeStructs::Type* type) {
				if (type->name == "Int" || type->name == "String" || type->name == "Bool")
					throw std::runtime_error("Cannot decompose type 'Int'");
				std::vector<NodeStructs::TypeOrTypeTemplateInstance> res;
				res.reserve(type->memberVariables.size());
				for (const auto& mv : type->memberVariables)
					res.push_back(type_of_typename(variables, named, mv.type));
				return res;
			},
			[&](const NodeStructs::TypeTemplateInstance& type) -> std::vector<NodeStructs::TypeOrTypeTemplateInstance> {
				const auto tn = type.type_template->templated.name;
				if (tn == "Set")
					if (type.template_arguments.size() == 1)
						return { type.template_arguments.at(0) };
					else
						throw std::runtime_error("");
				else
					if (tn == "Vector")
						if (type.template_arguments.size() == 1)
							return { type.template_arguments.at(0) };
						else
							throw std::runtime_error("");
				else
					if (tn == "Map")
						if (type.template_arguments.size() == 2)
							return { type.template_arguments.at(0), type.template_arguments.at(1) };
						else
							throw std::runtime_error("");
				else
					if (tn == "Pair")
						if (type.template_arguments.size() == 2)
							return { type.template_arguments.at(0), type.template_arguments.at(1) };
						else
							throw std::runtime_error("");
				int a = 0;
				throw std::runtime_error("");
			}
		),
		type
	);
}

void add_for_iterator_variables(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const NodeStructs::ForStatement& statement,
	const NodeStructs::TypeOrTypeTemplateInstance& it_type
) {
	if (statement.iterators.size() == 0)
		throw std::runtime_error("");
	bool should_be_decomposed = statement.iterators.size() > 1;
	bool can_be_decomposed = should_be_decomposed && [&]() {
		if (std::holds_alternative<const NodeStructs::Type*>(it_type)) {
			const auto* t = std::get<const NodeStructs::Type*>(it_type);
			if (t->name == "Int")
				return false;
		}
		return true;
	}();
	if (can_be_decomposed) {
		auto decomposed_types = decomposed_type(variables, named, it_type);
		if (statement.iterators.size() != decomposed_types.size())
			throw std::runtime_error("");
		for (int i = 0; i < statement.iterators.size(); ++i) {
			const auto& iterator = statement.iterators.at(i);
			std::visit(
				overload(
					[&](const NodeStructs::VariableDeclaration& it) {
						/*if (decomposed_types.at(i) != type_of_typename(variables, named, it.type)) {
							auto err = "Invalid type of iterator " + transpile(variables, named, it.type);
							throw std::runtime_error(err);
						}*/
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
	else {
		if (statement.iterators.size() > 1)
			throw std::runtime_error("Expected 1 iterator");
		else
			std::visit(
				overload(
					[&](const NodeStructs::VariableDeclaration& it) {
						/*if (decomposed_types.at(i) != type_of_typename(variables, named, it.type)) {
							auto err = "Invalid type of iterator " + transpile(variables, named, it.type);
							throw std::runtime_error(err);
						}*/
						variables[it.name].push_back(type_of_typename(variables, named, it.type));
					},
					[&](const std::string& it) {
						variables[it].emplace_back(it_type);
					}
				),
				statement.iterators.at(0)
			);
	}
}

std::string transpile_statement(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const NodeStructs::ForStatement& statement
) {
	auto coll_type = type_of_expr(variables, named, statement.collection);
	auto it_type = iterator_type(variables, named, coll_type);

	bool can_be_decomposed = [&]() {
		if (std::holds_alternative<const NodeStructs::Type*>(it_type)) {
			const auto* t = std::get<const NodeStructs::Type*>(it_type);
			if (t->name == "Int")
				return false;
		}
		return true;
	}();
	add_for_iterator_variables(variables, named, statement, it_type);

	std::stringstream ss;
	if (can_be_decomposed) {
		ss << "for (auto&& [";
		bool first = true;
		for (const auto& iterator : statement.iterators) {
			if (first)
				first = false;
			else
				ss << ", ";
			ss << std::visit(overload(
				[&](const NodeStructs::VariableDeclaration& iterator) {
					return iterator.name;
				},
				[&](const std::string& iterator) {
					return iterator;
				}), iterator);
		}
		ss << "]";
	}
	else {
		if (statement.iterators.size() > 1)
			throw std::runtime_error("Expected 1 iterator");
		ss << "for (auto&& " << std::visit(overload(
			[&](const NodeStructs::VariableDeclaration& iterator) {
				return iterator.name;
			},
			[&](const std::string& iterator) {
				return iterator;
			}), statement.iterators.at(0));
	}
	ss  << " : "
		<< transpile(variables, named, statement.collection)
		<< ") {\n"
		<< transpile(variables, named, statement.statements)
		<< "}\n";

	remove_for_iterator_variables(variables, statement);
	return ss.str();
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
	return "while (" + transpile(variables, named, statement.whileExpr) + ") {\n" + transpile(variables, named, statement.statements) + "}";
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
		expr.expression.get()
	);
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
	const std::string& expr
) {
	return expr;
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
	return std::visit(overload(
		[&](const NodeStructs::Typename& typecast) {
			return "(" + transpile(variables, named, typecast) + ")";
		},
		[&](const auto& token_expr) {
			return symbol_as_text(token_expr);
		}
	), expr.unary_operator) + transpile(variables, named, expr.expr);
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
					return symbol_as_text(op);
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
