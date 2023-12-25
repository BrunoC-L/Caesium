#include <ranges>
#include <algorithm>
#include <source_location>
#include <format>
#include <stacktrace>

#include "toCPP.hpp"
#include "type_of_expression_visitor.hpp"
#include "ranges_fold_left_pipe.hpp"
#include "overload.hpp"

#include "type_of_function_like_call_with_args_visitor.hpp"
#include "transpile_type_visitor.hpp"
#include "type_of_typename_visitor.hpp"
#include "type_of_postfix_member_visitor.hpp"
#include "transpile_expression_visitor.hpp"
#include "transpile_statement_visitor.hpp"
#include "transpile_typename_visitor.hpp"

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


std::expected<std::pair<std::string, std::string>, user_error> transpile(const std::vector<NodeStructs::File>& project) {
	for (const auto& file : project)
		for (const auto& fn : file.functions)
			if (fn.name == "main") {

				std::map<std::string, Named> named_by_file;
				variables_t variables;

				cpp_std stuff_from_cpp{};

				for (const auto& file2 : project) {
					Named named_of_file;
					insert_all_named_recursive_with_imports(project, named_of_file, file2.filename);
					{
						named_of_file.type_templates["Set"] = &stuff_from_cpp.unordered_set;
						named_of_file.type_templates["Vector"] = &stuff_from_cpp.vector;
						named_of_file.type_templates["Map"] = &stuff_from_cpp.unordered_map;
						named_of_file.type_templates["Pair"] = &stuff_from_cpp.pair;

						named_of_file.types["Int"] = &stuff_from_cpp._int;
						named_of_file.types["Bool"] = &stuff_from_cpp._bool;
						named_of_file.types["String"] = &stuff_from_cpp.string;
						named_of_file.types["Void"] = &stuff_from_cpp._void;

						named_of_file.functions["println"] = &stuff_from_cpp.println;
					}
					named_by_file[file2.filename] = named_of_file;
				}

				std::stringstream h, cpp;

				h << default_includes;

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
					<< "};\n"
					"\n"
					"int main(int argc, char** argv) {\n"
					"\tstd::vector<std::string> args {};\n"
					"\tfor (int i = 0; i < argc; ++i)\n"
					"\t\targs.push_back(std::string(argv[i]));\n"
					"\treturn Main{}.main(std::move(args));\n"
					"};\n";

				return std::pair{ h.str(), cpp.str() };
			}
	return std::unexpected{ user_error{ "Missing \"main\" function" } };
}

transpile_t transpile_main_definition(
	variables_t& variables,
	const Named& named,
	const NodeStructs::Function& fn
) {

	if (fn.parameters.size() != 1)
		return std::unexpected{ user_error{ "\"main\" function declaration requires 1 argument of type Vector<String>" } };

	const auto& [parameter_type, cat, _] = fn.parameters.at(0);
	auto vector_str = NodeStructs::Typename{ NodeStructs::TemplatedTypename{
		NodeStructs::Typename{ NodeStructs::BaseTypename{ "Vector" } },
		{ NodeStructs::Typename{ NodeStructs::BaseTypename{ "String" } } }
	} };
	bool is_vec_str = parameter_type <=> vector_str == 0;
	if (!is_vec_str)
		return std::unexpected{ user_error{ "\"main\" function declaration using 1 argument must be of Vector<String> type" } };
	return transpile_definition(
		variables,
		named,
		NodeStructs::Function{
			.name = "main",
			.returnType = fn.returnType,
			.parameters = fn.parameters,
			.statements = fn.statements
		}
	);
}

transpile_t transpile_declaration(
	variables_t& variables,
	const Named& named,
	const NodeStructs::Function& fn
) {
	return transpile_typename_visitor{ {}, variables, named }(fn.returnType).value() + " " +
		fn.name + "(" + transpile(variables, named, fn.parameters).value() + ");\n";
}

transpile_t transpile_definition(
	variables_t& variables,
	const Named& named,
	const NodeStructs::Function& fn
) {
	auto k1 = transpile_typename_visitor{ {}, variables, named }(fn.returnType);
	if (!k1.has_value())
		return std::unexpected{ k1.error() };
	auto k2 = transpile(variables, named, fn.parameters);
	if (!k2.has_value())
		return std::unexpected{ k2.error() };
	for (auto&& [type_name, value_cat, name] : fn.parameters)
		variables[name].push_back(std::pair{ value_cat, type_of_typename_visitor{ {}, variables, named }(type_name) });
	auto k3 = transpile(variables, named, fn.statements);
	if (!k3.has_value())
		return std::unexpected{ k3.error() };
	for (auto&& [type_name, value_cat, name] : fn.parameters)
		variables[name].pop_back();
	return k1.value() + " " + fn.name + "(" + k2.value() + ") {\n" + k3.value() + "};\n";
}

transpile_t transpile_definition(
	variables_t& variables,
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
	variables_t& variables,
	const Named& named,
	const NodeStructs::Type& type
) {
	return "struct " + type.name + ";\n";
}

transpile_t transpile_definition(
	variables_t& variables,
	const Named& named,
	const NodeStructs::Type& type
) {
	std::stringstream ss;
	ss << "struct " << type.name << " {\n";

	for (const auto& alias : type.aliases)
		if (std::holds_alternative<NodeStructs::BaseTypename>(alias.aliasTo.value))
			ss << "using " << std::get<NodeStructs::BaseTypename>(alias.aliasTo.value).type << " = " << transpile_typename_visitor{ {}, variables, named }(alias.aliasFrom).value() << ";\n";
		else {
			auto err = "cannot alias to " + transpile_typename_visitor{ {}, variables, named }(alias.aliasTo).value();
			throw std::runtime_error(err);
		}

	for (const auto& member : type.memberVariables)
		ss << transpile_typename_visitor{ {}, variables, named }(member.type).value() << " " << member.name << ";\n";

	for (const auto& constructor : type.constructors)
		ss << transpile_definition(variables, named, constructor, type).value() << "\n";

	for (const auto& fn : type.methods)
		ss << transpile_definition(variables, named, fn).value();

	ss << "};\n\n";
	return ss.str();
}

template<typename T>
transpile_t transpile_declaration(
	variables_t& variables,
	const Named& named,
	const NodeStructs::Template<T>& tmpl
) {
	if (tmpl.arguments.arguments.size()) {
		auto opt_s =
			tmpl.arguments.arguments
			| LIFT_TRANSFORM_X(str, "typename " + str)
			| std::ranges::fold_left_([](const auto& a, const auto& b) { return a + ", " + b; });
		return "template <" + std::move(opt_s.value()) + ">\n" + transpile_declaration(variables, named, tmpl.templated);
	}
	else {
		return "template <>\n" + transpile_declaration(variables, named, tmpl.templated);
	}
}

template<typename T>
transpile_t transpile_definition(
	variables_t& variables,
	const Named& named,
	const NodeStructs::Template<T>& tmpl
) {
	if (tmpl.arguments.arguments.size()) {
		auto opt_s =
			tmpl.arguments.arguments
			| LIFT_TRANSFORM_X(str, "typename " + str)
			| std::ranges::fold_left_([](const auto& a, const auto& b) { return a + ", " + b; });
		return "template <" + std::move(opt_s.value()) + ">\n" + transpile_definition(variables, named, tmpl.templated);
	}
	else {
		return "template <>\n" + transpile_definition(variables, named, tmpl.templated);
	}
}

transpile_t transpile(
	variables_t& variables,
	const Named& named,
	const std::vector<std::tuple<NodeStructs::Typename, NodeStructs::ValueCategory, std::string>>& parameters
) {
	std::stringstream ss;
	bool first = true;
	for (const auto& [type, cat, name] : parameters) {
		ss << transpile_typename_visitor{ {}, variables, named }(type).value() << " " << name;
		if (first)
			first = false;
		else
			ss << ", ";
	}
	return ss.str();
}

transpile_t transpile(
	variables_t& variables,
	const Named& named,
	const std::vector<NodeStructs::Statement>& statements
) {
	auto transpile_statement = transpile_statement_visitor{ {}, variables, named };
	std::stringstream ss;
	for (const auto& statement : statements) {
		auto k = transpile_statement(statement);
		if (k.has_value())
			ss << k.value();
		else
			return std::unexpected{ k.error() };
	}
	for (const auto& statement : statements)
		remove_added_variables(variables, named, statement);
	return ss.str();
}

void remove_added_variables(
	variables_t& variables,
	const Named& named,
	const NodeStructs::Statement& statement
) {
	std::visit(
		overload(
			[&](const NodeStructs::VariableDeclarationStatement& declaration) {
				variables[declaration.name].pop_back();
			},
			[&](const NodeStructs::BlockStatement& statement) {
				auto s = std::get<NodeStructs::BaseTypename>(statement.parametrized_block.value).type;

				if (named.blocks.contains(s)) {
					const NodeStructs::Block& block = *named.blocks.at(s);
					for (const auto& statement_in_block : block.statements)
						remove_added_variables(variables, named, statement_in_block);
				}
				else {
					throw std::runtime_error("bad block name" + s);
				}
			},
			[&](const auto&) {
				// do nothing
			}
		),
		statement.statement
	);
}

NodeStructs::TypeCategory iterator_type(
	variables_t& variables,
	const Named& named,
	const NodeStructs::TypeCategory& type
) {
	return std::visit(
		overload(overload_default_error,
			[&](const std::reference_wrapper<const NodeStructs::Type>& type) -> NodeStructs::TypeCategory {
				throw;
			},
			[&](const NodeStructs::TypeTemplateInstanceType& type) -> NodeStructs::TypeCategory {
				const auto tn = type.type_template.get().templated.name;
				if (tn == "Set")
					if (type.template_arguments.size() == 1)
						return type.template_arguments.at(0);
					else
						throw;
				else
					if (tn == "Vector")
						if (type.template_arguments.size() == 1)
							return type.template_arguments.at(0);
						else
							throw;
					else
						if (tn == "Map")
							if (type.template_arguments.size() == 2)
								return NodeStructs::TypeCategory{ NodeStructs::TypeTemplateInstanceType{
									.type_template = *named.type_templates.at("Map"),
									.template_arguments = { type.template_arguments.at(0), type.template_arguments.at(1) },
								} };
							else
								throw;
						else
							throw;
			},
			[&](const NodeStructs::AggregateType&) -> NodeStructs::TypeCategory {
				throw;
			},
			[&](const NodeStructs::TypeType&) -> NodeStructs::TypeCategory {
				throw;
			},
			[&](const NodeStructs::TypeTemplateType&) -> NodeStructs::TypeCategory {
				throw;
			},
			[&](const NodeStructs::FunctionType&) -> NodeStructs::TypeCategory {
				throw;
			},
			[&](const NodeStructs::FunctionTemplateType&) -> NodeStructs::TypeCategory {
				throw;
			},
			[&](const NodeStructs::UnionType&) -> NodeStructs::TypeCategory {
				throw;
			}
		),
		type.value
	);
}

std::vector<NodeStructs::TypeCategory> decomposed_type(
	variables_t& variables,
	const Named& named,
	const NodeStructs::TypeCategory& type
) {
	return std::visit(
		overload(overload_default_error,
			[&](const std::reference_wrapper<const NodeStructs::Type>& _type) {
				const auto& type = _type.get();
				if (type.name == "Int" || type.name == "String" || type.name == "Bool")
					throw std::runtime_error("Cannot decompose type 'Int'");
				return type.memberVariables
					| std::views::transform([&](const auto& e) { return type_of_typename_visitor{ {}, variables, named }(e.type); })
					| to_vec();
			},
			[&](const NodeStructs::TypeTemplateInstanceType& type) -> std::vector<NodeStructs::TypeCategory> {
				const auto tn = type.type_template.get().templated.name;
				if (tn == "Set")
					if (type.template_arguments.size() == 1)
						return { type.template_arguments.at(0) };
					else
						throw;
				else
					if (tn == "Vector")
						if (type.template_arguments.size() == 1)
							return { type.template_arguments.at(0) };
						else
							throw;
					else
						if (tn == "Map")
							if (type.template_arguments.size() == 2)
								return { type.template_arguments.at(0), type.template_arguments.at(1) };
							else
								throw;
						else
							if (tn == "Pair")
								if (type.template_arguments.size() == 2)
									return { type.template_arguments.at(0), type.template_arguments.at(1) };
								else
									throw;
				throw;
			},
			[&](const NodeStructs::AggregateType&) -> std::vector<NodeStructs::TypeCategory> {
				throw;
			},
			[&](const NodeStructs::TypeType&) -> std::vector<NodeStructs::TypeCategory> {
				throw;
			},
			[&](const NodeStructs::TypeTemplateType&) -> std::vector<NodeStructs::TypeCategory> {
				throw;
			},
			[&](const NodeStructs::FunctionType&) -> std::vector<NodeStructs::TypeCategory> {
				throw;
			},
			[&](const NodeStructs::FunctionTemplateType&) -> std::vector<NodeStructs::TypeCategory> {
				throw;
			},
			[&](const NodeStructs::UnionType&) -> std::vector<NodeStructs::TypeCategory> {
				throw;
			}
		),
		type.value
	);
}

void add_for_iterator_variables(
	variables_t& variables,
	const Named& named,
	const NodeStructs::ForStatement& statement,
	const NodeStructs::TypeCategory& it_type
) {
	if (statement.iterators.size() == 0)
		throw std::runtime_error("");
	bool should_be_decomposed = statement.iterators.size() > 1;
	bool can_be_decomposed = should_be_decomposed && [&]() {
		if (std::holds_alternative<std::reference_wrapper<const NodeStructs::Type>>(it_type.value)) {
			const auto t = std::get<std::reference_wrapper<const NodeStructs::Type>>(it_type.value);
			if (t.get().name == "Int")
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
					overload(overload_default_error,
						[&](const NodeStructs::VariableDeclaration& it) {
							/*if (decomposed_types.at(i) != type_of_typename(variables, named, it.type)) {
								auto err = "Invalid type of iterator " + transpile(variables, named, it.type);
								throw std::runtime_error(err);
							}*/
							variables[it.name].push_back({ NodeStructs::Reference{}, type_of_typename_visitor{ {}, variables, named }(it.type) });
						},
						[&](const std::string& it) {
							variables[it].push_back({ NodeStructs::Reference{}, decomposed_types.at(i) });
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
					overload(overload_default_error,
						[&](const NodeStructs::VariableDeclaration& it) {
							/*if (decomposed_types.at(i) != type_of_typename(variables, named, it.type)) {
								auto err = "Invalid type of iterator " + transpile(variables, named, it.type);
								throw std::runtime_error(err);
							}*/
							variables[it.name].push_back({ NodeStructs::Reference{}, type_of_typename_visitor{ {}, variables, named }(it.type) });
						},
						[&](const std::string& it) {
							variables[it].push_back({ NodeStructs::Reference{}, it_type });
						}
					),
					statement.iterators.at(0)
				);
		}
}

void remove_for_iterator_variables(
	variables_t& variables,
	const NodeStructs::ForStatement& statement
) {
	for (const auto& iterator : statement.iterators)
		std::visit(
			overload(overload_default_error,
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

transpile_t transpile_arg(
	variables_t& variables,
	const Named& named,
	const NodeStructs::FunctionArgument& arg
) {
	return transpile_expression_visitor{ {}, variables, named }(std::get<NodeStructs::Expression>(arg));
}

transpile_t transpile_args(
	variables_t& variables,
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

std::expected<std::pair<NodeStructs::ValueCategory, NodeStructs::TypeCategory>, user_error> type_of_postfix_member(
	variables_t& variables,
	const Named& named,
	const NodeStructs::Type& t,
	const std::string& property_name
) {
	const auto& v = t.memberVariables;
	auto pos = std::find_if(v.begin(), v.end(), [&](const NodeStructs::MemberVariable& m) { return m.name == property_name; });
	if (pos == v.end()) {
		auto u = transpile_type_visitor{ {}, variables, named }(t);
		if (u.has_value())
			return std::unexpected{ user_error{ "Error: object of type `" + std::move(u).value() + "` has no member `" + property_name + "`\n"} };
		else
			return std::unexpected{ std::move(u).error() };
	}
	else
		return std::pair{ NodeStructs::Value{}, type_of_typename_visitor{ {}, variables, named }(pos->type) };
}
