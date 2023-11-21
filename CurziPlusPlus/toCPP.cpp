#include "toCPP.h"
#include "type_of_expr.h"
#include "type_of_typename.h"
#include <ranges>
#include <algorithm>
#include "cpp23_ranges_to.h"

void insert_all_named_recursive_with_imports(const std::vector<NodeStructs::File>& project, Named& named, const std::string& filename) {
	for (const NodeStructs::File& file : project)
		if (file.filename == filename) {
			for (const auto& e : file.types)
				named.types[e.name] = &e;
			for (const auto& e : file.type_templates)
				named.type_templates[e.templated.name] = &e;

			for (const auto& e : file.functions)
				named.functions[e.name] = &e;
			for (const auto& e : file.function_templates)
				named.function_templates[e.templated.name] = &e;

			for (const auto& e : file.blocks)
				named.blocks[e.name] = &e;
			for (const auto& i : file.imports)
				insert_all_named_recursive_with_imports(project, named, i.imported);
			return;
		}
	auto err = std::string("Invalid import \"") + filename + "\"";
	throw std::runtime_error(err);
}

void transpile_recursive(
	const std::vector<NodeStructs::File>& project,
	const std::map<std::string, Named>& named_by_file,
	const NodeStructs::File& file,
	std::stringstream& h,
	std::stringstream& cpp
) {
	/*for (const auto& i : file.imports)
		transpile_recursive(project, named_by_file, i, h, cpp);*/
}


std::expected<std::pair<std::string, std::string>, user_error> transpile(const std::vector<NodeStructs::File>& project) {
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

						named_of_file.function_templates["println"] = &stuff_from_cpp.println;
					}
					named_by_file[file.filename] = named_of_file;
				}

				std::stringstream h, cpp;

				h << default_includes;

				transpile_recursive(project, named_by_file, file, h, cpp);

				for (const auto& file2 : project) {
					for (const auto& t : file2.types) {
						auto k = transpile_declaration(variables, named_by_file[file.filename], t);
						h << k.value();
					}
					/*for (const auto& t : file2.type_templates)
						h << transpile_declaration(variables, named_by_file[file.filename], t);*/
					for (const auto& fn2 : file2.functions)
						if (fn2.name != "main") {
							auto k = transpile_declaration(variables, named_by_file[file.filename], fn2);
							h << k.value();
						}
				}

				cpp << "#include \"header.h\"\n";

				for (const auto& file2 : project) {
					for (const auto& t : file2.types) {
						auto k = transpile_definition(variables, named_by_file[file.filename], t);
						cpp << k.value();
					}
					/*for (const auto& t : file2.type_templates)
						cpp << transpile_definition(variables, named_by_file[file.filename], t);*/
					for (const auto& fn2 : file2.functions)
						if (fn2.name != "main") {
							auto k = transpile_definition(variables, named_by_file[file.filename], fn2);
							cpp << k.value();
						}
				}

				auto k = transpile_main_definition(variables, named_by_file[file.filename], fn);
				if (!k.has_value())
					return std::unexpected{ k.error() };
				cpp
				<< "struct Main {\n"
				<< k.value()
				<<  "};\n"
					"\n"
					"int main(int argc, char** argv) {\n"
					"\tstd::vector<std::string> args {};\n"
					"\tfor (int i = 0; i < argc; ++i)\n"
					"\t\targs.push_back(std::string(argv[i]));\n"
					"\treturn Main{}.main(std::move(args));\n"
					"};\n";

				return std::pair{ h.str(), cpp.str() };
			}
	throw std::runtime_error("Missing \"main\" function\n");
}

transpile_t transpile_main_definition(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const NodeStructs::Function& fn
) {

	if (fn.parameters.size() != 1)
		throw std::runtime_error("\"main\" function declaration requires 1 argument of type Vector<String>\n");

	const auto& [parameter_type, cat, _] = fn.parameters.at(0);
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

	return transpile_definition(
		variables,
		named,
		NodeStructs::Function{
			.name = "main",
			.returnType = fn.returnType,
			.parameters = fn.parameters,
			.statements = fn.statements
		});
}

transpile_t transpile_declaration(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const NodeStructs::Function& fn
) {
	return transpile(variables, named, fn.returnType).value() + " " +
		fn.name + "(" + transpile(variables, named, fn.parameters).value() + ");\n";
}

transpile_t transpile_definition(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const NodeStructs::Function& fn
) {
	auto k1 = transpile(variables, named, fn.returnType);
	if (!k1.has_value())
		return std::unexpected{ k1.error() };
	auto k2 = transpile(variables, named, fn.parameters);
	if (!k2.has_value())
		return std::unexpected{ k2.error() };
	auto k3 = transpile(variables, named, fn.statements);
	if (!k3.has_value())
		return std::unexpected{ k3.error() };
	return k1.value() + " " + fn.name + "(" + k2.value() + ") {\n" + k3.value() + "};\n";
}

transpile_t transpile_definition(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const NodeStructs::Constructor& fn,
	const NodeStructs::Type& type
) {
	return type.name + "(" + transpile(variables, named, fn.parameters).value() +
		") {\n" +
		transpile(variables, named, fn.statements).value() +
		"};";
}

transpile_t transpile_declaration(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const NodeStructs::Type& type
) {
	return "struct " + type.name + ";\n";
}

transpile_t transpile_definition(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const NodeStructs::Type& type
) {
	std::stringstream ss;
	ss << "struct " << type.name << " {\n";

	for (const auto& alias : type.aliases)
		if (std::holds_alternative<NodeStructs::BaseTypename>(alias.aliasTo))
			ss << "using " << std::get<NodeStructs::BaseTypename>(alias.aliasTo).type << " = " << transpile(variables, named, alias.aliasFrom).value() << ";\n";
		else {
			auto err = "cannot alias to " + transpile(variables, named, alias.aliasTo).value();
			throw std::runtime_error(err);
		}

	for (const auto& member : type.memberVariables)
		ss << transpile(variables, named, member.type).value() << " " << member.name << ";\n";

	if (type.constructors.size()) {
		for (const auto& constructor : type.constructors)
			ss << transpile_definition(variables, named, constructor, type).value() << "\n";
		ss << type.name << "(const " << type.name << "&) = default;\n";
		ss << type.name << "& operator=(const " << type.name << "&) = default;\n";
		ss << type.name << "(" << type.name << "&&) = default;\n";
		ss << type.name << "& operator=(" << type.name << "&&) = default;\n";
		ss << "~" << type.name << "() = default;\n";
	}

	for (const auto& fn : type.methods)
		ss << transpile_definition(variables, named, fn).value();

	ss << "};\n\n";
	return ss.str();
}

template<typename T>
transpile_t transpile_declaration(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const NodeStructs::Template<T>& tmpl
) {
	if (tmpl.arguments.arguments.size()) {
		auto opt_s =
			tmpl.arguments.arguments
			| std::views::transform([](const std::string& str) { return "typename " + str; })
			| std::ranges::fold_left_([](const auto& a, const auto& b) { return a + ", " + b; });
		return "template <" + std::move(opt_s.value()) + ">\n" + transpile_declaration(variables, named, tmpl.templated);
	}
	else {
		return "template <>\n" + transpile_declaration(variables, named, tmpl.templated);
	}
}

template<typename T>
transpile_t transpile_definition(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const NodeStructs::Template<T>& tmpl
) {
	if (tmpl.arguments.arguments.size()) {
		auto opt_s =
			tmpl.arguments.arguments
			| std::views::transform([](const std::string& str) { return "typename " + str; })
			| std::ranges::fold_left_([](const auto& a, const auto& b) { return a + ", " + b; });
		return "template <" + std::move(opt_s.value()) + ">\n" + transpile_definition(variables, named, tmpl.templated);
	}
	else {
		return "template <>\n" + transpile_definition(variables, named, tmpl.templated);
	}
}

transpile_t transpile(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const std::vector<std::tuple<NodeStructs::Typename, NodeStructs::ValueCategory, std::string>>& parameters
) {
	std::stringstream ss;
	bool first = true;
	for (const auto& [type, cat, name] : parameters) {
		ss << transpile(variables, named, type).value() << " " << name;
		if (first)
			first = false;
		else
			ss << ", ";
	}
	return ss.str();
}

transpile_t transpile(
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

transpile_t transpile(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const NodeStructs::BaseTypename& type
) {
	return type.type;
}

transpile_t transpile(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const NodeStructs::NamespacedTypename& type
) {
	return transpile(variables, named, type.name_space.get()).value() + "::" + transpile(variables, named, type.name_in_name_space.get()).value();
}

transpile_t transpile(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const NodeStructs::TemplatedTypename& type
) {
	std::stringstream ss;
	ss << transpile(variables, named, type.type.get()).value() << "<";
	bool first = true;
	for (const auto& t : type.templated_with) {
		if (first)
			first = false;
		else
			ss << ", ";
		ss << transpile(variables, named, t).value();
	}
	ss << ">";
	return ss.str();
}





transpile_t transpile(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const NodeStructs::TypeOrTypeTemplateInstance& type
) {
	return std::visit([&](const auto& t) { return transpile(variables, named, t);  }, type);
}

transpile_t transpile(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const NodeStructs::Type* type
) {
	return type->name;
}

transpile_t transpile(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const NodeStructs::TypeTemplateInstance& type
) {
	std::stringstream ss;
	ss << type.type_template->templated.name << "<";
	bool has_previous = false;
	for (const auto& e : type.template_arguments) {
		if (has_previous)
			ss << ", ";
		else
			has_previous = true;
		ss << transpile(variables, named, e).value();
	}
	ss << ">";
	return ss.str();
}






transpile_t transpile(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const std::vector<NodeStructs::Statement>& statements
) {
	std::stringstream ss;
	for (const auto& statement : statements) {
		auto k = transpile_statement(variables, named, statement);
		if (k.has_value())
			ss << k.value();
		else
			return std::unexpected{ k.error() };
	}
	for (const auto& statement : statements)
		remove_added_variables(variables, named, statement);
	return ss.str();
}

transpile_t transpile_statement(
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
			[&](const auto&) {
				throw std::runtime_error("");
			}
		),
		statement.statement
	);
}

transpile_t transpile_statement(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const NodeStructs::VariableDeclarationStatement& statement
) {
	variables[statement.name].push_back(std::move(type_of_typename(variables, named, statement.type)));
	return transpile(variables, named, statement.type).value() + " " + statement.name + " = " + transpile(variables, named, statement.expr).value() + ";\n";
}

transpile_t transpile_statement(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const NodeStructs::BlockStatement& statement
) {
	auto s = std::get<NodeStructs::BaseTypename>(statement.parametrized_block).type;
	if (named.blocks.contains(s)) {
		const NodeStructs::Block& block = *named.blocks.at(s);
		std::stringstream ss;
		for (const auto& statement : block.statements)
			ss << transpile_statement(variables, named, statement).value();
		return ss.str();
	}
	else {
		throw std::runtime_error("bad block name" + s);
	}
	return "";
}

transpile_t transpile_statement(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const NodeStructs::IfStatement& statement
) {
	if (statement.elseExprStatements.has_value())
		return "if (" +
		transpile(variables, named, statement.ifExpr).value() +
		") {\n" +
		transpile(variables, named, statement.ifStatements).value() +
		"} else " +
		std::visit(
			overload(
				[&](const Allocated<NodeStructs::IfStatement>& elseif) {
					return transpile_statement(variables, named, elseif.get()).value();
				},
				[&](const std::vector<NodeStructs::Statement>& justelse) {
					return "{" + transpile(variables, named, justelse).value() + "}";
				}
					),
			statement.elseExprStatements.value()
		);
	else
		return "if (" +
		transpile(variables, named, statement.ifExpr).value() +
		") {\n" +
		transpile(variables, named, statement.ifStatements).value() +
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
				throw std::runtime_error("");
			}/*,
			[&](const NodeStructs::BuiltIn& builtin) -> NodeStructs::TypeOrTypeTemplateInstance {
				throw std::runtime_error("");
			}*/
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
			}/*,
			[&](const NodeStructs::BuiltIn& builtin) -> std::vector<NodeStructs::TypeOrTypeTemplateInstance> {
				throw std::runtime_error("");
			}*/
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
						variables[it].push_back(decomposed_types.at(i));
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

transpile_t transpile_statement(
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
		<< transpile(variables, named, statement.collection).value()
		<< ") {\n"
		<< transpile(variables, named, statement.statements).value()
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

transpile_t transpile_statement(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const NodeStructs::IForStatement& statement
) {
	throw std::runtime_error("not implemented");
	return "";
}

transpile_t transpile_statement(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const NodeStructs::WhileStatement& statement
) {
	return "while (" + transpile(variables, named, statement.whileExpr).value() + ") {\n" + transpile(variables, named, statement.statements).value() + "}";
}

transpile_t transpile_statement(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const NodeStructs::BreakStatement& statement
) {
	if (statement.ifExpr.has_value())
		return "if (" + transpile(variables, named, statement.ifExpr.value()).value() + ") break;\n";
	else
		return "break;\n";
}

transpile_t transpile_statement(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const NodeStructs::ReturnStatement& statement
) {
	std::string return_value = [&]() {
		if (statement.returnExpr.size() == 0)
			return std::string{};
		else if (statement.returnExpr.size() == 1)
			return transpile(variables, named, statement.returnExpr.at(0)).value();
		else {
			throw std::runtime_error("");
			//return "{" + transpile_args(variables, named, statement.returnExpr).value() + "}";
		}
	}();
	if (statement.ifExpr.has_value())
		return "if (" + transpile(variables, named, statement.ifExpr.value()).value() + ") return " + return_value + ";\n";
	else
		return "return " + return_value + ";\n";
}




// expressions

transpile_t transpile(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const NodeStructs::Expression& expr
) {
	return std::visit(
		[&](const auto& expr) {
			return transpile(variables, named, expr);
		},
		expr.expression.get()
	);
}

transpile_t transpile_statement(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const NodeStructs::Expression& statement
) {
	return transpile(variables, named, statement).transform([](auto s) { return std::move(s) + ";\n"; });
}

transpile_t transpile(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const Token<NUMBER>& expr
) {
	return expr.value;
}

transpile_t transpile(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const std::string& expr
) {
	return expr;
}

transpile_t transpile(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const NodeStructs::AssignmentExpression& expr
) {
	std::stringstream ss;
	ss << transpile(variables, named, expr.expr).value();
	for (const auto& expr : expr.assignments)
		ss << symbol_variant_as_text(expr.first) << transpile(variables, named, expr.second).value();
	return ss.str();
}

transpile_t transpile(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const NodeStructs::ConditionalExpression& expr
) {
	if (expr.ifElseExprs.has_value()) {
		// x if first else second
		//                  ([&] () { if (first) return x; else return second; }());
		return std::string("([&] () { if (") +
			transpile(variables, named, expr.ifElseExprs.value().first).value() +
			") return " +
			transpile(variables, named, expr.expr).value() +
			"; else return " +
			transpile(variables, named, expr.ifElseExprs.value().second).value() +
			"; }())";
	}
	else {
		return transpile(variables, named, expr.expr);
	}
}

transpile_t transpile(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const NodeStructs::OrExpression& expr
) {
	std::stringstream ss;
	ss << transpile(variables, named, expr.expr).value();
	for (const auto& expr : expr.ors)
		ss << " || " << transpile(variables, named, expr).value();
	return ss.str();
}

transpile_t transpile(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const NodeStructs::AndExpression& expr
) {
	std::stringstream ss;
	ss << transpile(variables, named, expr.expr).value();
	for (const auto& expr : expr.ands)
		ss << " && " << transpile(variables, named, expr).value();
	return ss.str();
}

transpile_t transpile(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const NodeStructs::EqualityExpression& expr
) {
	std::stringstream ss;
	ss << transpile(variables, named, expr.expr).value();
	for (const auto& [op, expr] : expr.equals)
		ss << " " << symbol_variant_as_text(op) << " " << transpile(variables, named, expr).value();
	return ss.str();
}

transpile_t transpile(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const NodeStructs::CompareExpression& expr
) {
	std::stringstream ss;
	ss << transpile(variables, named, expr.expr).value();
	for (const auto& [op, expr] : expr.comparisons)
		ss << " " << symbol_variant_as_text(op) << " " << transpile(variables, named, expr).value();
	return ss.str();
}

transpile_t transpile(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const NodeStructs::AdditiveExpression& expr
) {
	std::stringstream ss;
	ss << transpile(variables, named, expr.expr).value();
	for (const auto& [op, expr] : expr.adds)
		ss << " " << symbol_variant_as_text(op) << " " << transpile(variables, named, expr).value();
	return ss.str();
}

transpile_t transpile(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const NodeStructs::MultiplicativeExpression& expr
) {
	std::stringstream ss;
	ss << transpile(variables, named, expr.expr).value();
	for (const auto& [op, expr] : expr.muls)
		ss << " " << symbol_variant_as_text(op) << " " << transpile(variables, named, expr).value();
	std::string x = ss.str();
	return ss.str();
}

transpile_t transpile(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const NodeStructs::UnaryExpression& expr
) {
	return std::visit(overload(
		[&](const NodeStructs::Typename& typecast) {
			return "(" + transpile(variables, named, typecast).value() + ")";
		},
		[&](const auto& token_expr) {
			return symbol_as_text(token_expr);
		}
	), expr.unary_operator) + transpile(variables, named, expr.expr).value();
}

transpile_t transpile(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const NodeStructs::PostfixExpression& expr
) {
	std::stringstream ss;
	ss << transpile(variables, named, expr.expr).value();
	auto ct = type_of_expr(variables, named, expr.expr);
	for (const auto& op : expr.postfixes) {
		auto k = std::visit(
			overload(
				[&](const std::string& property_name) -> transpile_t {
					auto t = type_of_postfix_member(variables, named, ct, property_name);
					if (t.has_value()) {
						ct = t.value();
						return "." + property_name;
					}
					else {
						std::cout << "Error: object of type `" << transpile(variables, named, ct).value() << "` has no member `" + property_name + "`\n";
						throw std::runtime_error("");
					}
				},
				[&](const NodeStructs::ParenExpression& e) -> transpile_t {
					throw std::runtime_error("");
					/*auto t = type_of_call_with(variables, named, ct, e.args);
					if (t.has_value()) {
						ct = t.value();
						return "(" + transpile_args(variables, named, e.args).value() + ")";
					}
					else {
						std::stringstream ss;
						ss << "Error: `" << transpile(variables, named, ct).value() << "` is not callable with `(";
						bool has_previous = false;
						for (const auto& arg : e.args) {
							if (has_previous)
								ss << ", ";
							else
								has_previous = true;
							ss << transpile(variables, named, type_of_expr(variables, named, arg)).value();
						}
						ss << ")`\n";

						std::string e = ss.str();
						return std::unexpected{ user_error{ e } };
					}*/
				},
				[&](const NodeStructs::BracketArguments& e) -> transpile_t {
					return "[" + transpile_args(variables, named, e.args).value() + "]";
				},
				[&](const NodeStructs::BraceExpression& e) -> transpile_t {
					throw std::runtime_error("");
					//return "{" + transpile_args(variables, named, e.args).value() + "}";
				},
				[&](const NodeStructs::BraceArguments& e) -> transpile_t {
					throw std::runtime_error("");
				},
				[&](const NodeStructs::ParenArguments& e) -> transpile_t {
					throw std::runtime_error("");
				},
				[&](const auto& op) -> transpile_t {
					return symbol_as_text(op);
				}
			),
			op
		);
		if (k.has_value())
			ss << k.value();
		else
			return std::unexpected{ k.error() };
	}
	return ss.str();
}

transpile_t transpile(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const NodeStructs::ParenExpression& expr
) {
	throw std::runtime_error("");
	//return "(" + transpile_args(variables, named, expr.args).value() + ")";
}

transpile_t transpile(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const NodeStructs::BraceExpression& expr
) {
	throw std::runtime_error("");
	//return "{" + transpile_args(variables, named, expr.args).value() + "}";
}

transpile_t transpile_arg(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const NodeStructs::FunctionArgument& arg
) {
	return transpile(variables, named, std::get<1>(arg));
}

transpile_t transpile_args(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const std::vector<NodeStructs::FunctionArgument>& args
) {
	std::stringstream ss;
	bool first = true;
	for (const auto& arg : args) {
		if (first)
			first = false;
		else
			ss << ", ";
		ss << transpile_arg(variables, named, arg).value();
	}
	return ss.str();
}

std::optional<NodeStructs::TypeOrTypeTemplateInstance> type_of_call_with(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const NodeStructs::TypeOrTypeTemplateInstance& t,
	const std::vector<NodeStructs::FunctionArgument>& args
) {
	return std::nullopt;
}

std::optional<NodeStructs::TypeOrTypeTemplateInstance> type_of_postfix_member(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const NodeStructs::TypeOrTypeTemplateInstance& t,
	const std::string& property_name
) {
	return std::visit(
		overload(
			[&](const NodeStructs::Type* t) {
				return type_of_postfix_member(variables, named, *t, property_name);
			},
			[&](const NodeStructs::TypeTemplateInstance&) {
				return std::optional<NodeStructs::TypeOrTypeTemplateInstance>{};
			}
		),
		t
	);
}

std::optional<NodeStructs::TypeOrTypeTemplateInstance> type_of_postfix_member(
	std::map<std::string, std::vector<NodeStructs::TypeOrTypeTemplateInstance>>& variables,
	const Named& named,
	const NodeStructs::Type& t,
	const std::string& property_name
) {
	const auto& v = t.memberVariables;
	auto pos = std::find_if(v.begin(), v.end(), [&](const NodeStructs::MemberVariable& m) { return m.name == property_name; });
	if (pos == v.end())
		return std::nullopt;
	else
		return type_of_typename(variables, named, pos->type);
}
