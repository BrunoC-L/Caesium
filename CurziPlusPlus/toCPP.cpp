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
			/*for (const auto& e : file.block_templates)
				named.blocks[e.name] = &e;*/

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
				cpp_std stuff_from_cpp{};
				variables_t variables;
				{
					variables["True"].push_back({ NodeStructs::Value{}, { stuff_from_cpp._bool } });
					variables["False"].push_back({ NodeStructs::Value{}, { stuff_from_cpp._bool } });
				}

				std::map<std::string, Named> named_by_file;

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

				transpilation_state state{ std::move(variables), Named(named_by_file[file.filename]) };
				state.traversed_functions.insert(stuff_from_cpp.println);
				state.traversed_types.insert(stuff_from_cpp._int);
				state.traversed_types.insert(stuff_from_cpp._bool);
				state.traversed_types.insert(stuff_from_cpp.string);
				state.traversed_types.insert(stuff_from_cpp._void);

				transpile_header_cpp_t k = transpile_main(state, fn);
				if (!k.has_value())
					return std::unexpected{ k.error() };

				std::stringstream h, cpp;
				h << default_includes << k.value().first;
				cpp << "#include \"header.h\"\n";

				auto reverse_iterator = state.transpile_in_reverse_order.rbegin();
				for (; reverse_iterator != state.transpile_in_reverse_order.rend(); ++reverse_iterator) {
					const auto& [_h, _cpp] = *reverse_iterator;
					h << _h;
					cpp << _cpp;
				}
				cpp << k.value().second
					<< "\n"
					"int main(int argc, char** argv) {\n"
					"\tstd::vector<std::string> args {};\n"
					"\tfor (int i = 0; i < argc; ++i)\n"
					"\t\targs.push_back(std::string(argv[i]));\n"
					"\treturn _redirect_main(args);\n"
					"};\n";

				return std::pair{ h.str(), cpp.str() };
			}
	return std::unexpected{ user_error{ "Missing \"main\" function" } };
}

transpile_header_cpp_t transpile_main(
	transpilation_state& state,
	const NodeStructs::Function& fn
) {
	if (fn.parameters.size() != 1)
		return std::unexpected{ user_error{ "\"main\" function requires 1 argument of type `Vector<String> ref`" } };

	const auto& [type, cat, name] = fn.parameters.at(0);
	auto vector_str = NodeStructs::Typename{ NodeStructs::TemplatedTypename{
		NodeStructs::Typename{ NodeStructs::BaseTypename{ "Vector" } },
		{ NodeStructs::Typename{ NodeStructs::BaseTypename{ "String" } } }
	} };
	bool is_vec_str = type <=> vector_str == 0;
	if (!is_vec_str)
		return std::unexpected{ user_error{ "\"main\" function using 1 argument must be of `Vector<String> ref` type" } };
	return transpile(
		state,
		NodeStructs::Function{
			.name = "_redirect_main",
			.returnType = fn.returnType,
			.parameters = fn.parameters,
			.statements = fn.statements
		}
	);
}

transpile_header_cpp_t transpile(
	transpilation_state& state,
	const NodeStructs::Function& fn
) {
	auto return_type = transpile_typename_visitor{ {}, state }(fn.returnType);
	if (!return_type.has_value())
		return std::unexpected{ return_type.error() };
	auto parameters = transpile(state, fn.parameters);
	if (!parameters.has_value())
		return std::unexpected{ parameters.error() };
	for (auto&& [type_name, value_cat, name] : fn.parameters)
		state.variables[name].push_back(std::pair{ value_cat, type_of_typename_visitor{ {}, state }(type_name).value() });
	auto _ = state.indent_sentinel();
	auto statements = transpile(state, fn.statements);
	if (!statements.has_value())
		return std::unexpected{ statements.error() };
	for (auto&& [type_name, value_cat, name] : fn.parameters)
		state.variables[name].pop_back();
	auto common = return_type.value() + " " + fn.name + "(" + parameters.value() + ")";
	auto h = common + ";\n";
	auto cpp = common + " {\n" + statements.value() + "};\n";
	return std::pair{ h, cpp };
}

transpile_header_cpp_t transpile(
	transpilation_state& state,
	const NodeStructs::Type& type
) {
	std::stringstream cpp;
	cpp << "struct " << type.name << " {\n";

	for (const auto& alias : type.aliases) {
		throw;
		/*if (std::holds_alternative<NodeStructs::BaseTypename>(alias.aliasTo.value))
			cpp << "using "
			<< std::get<NodeStructs::BaseTypename>(alias.aliasTo.value).type
			<< " = " << transpile_typename_visitor{ {}, state }(alias.aliasFrom).value()
			<< ";\n";
		else {
			auto err = "cannot alias to " + transpile_typename_visitor{ {}, state }(alias.aliasTo).value();
			throw std::runtime_error(err);
		}*/
	}

	for (const auto& member : type.memberVariables) {
		auto type = type_of_typename_visitor{ {}, state }(member.type);
		if (!type.has_value())
			return std::unexpected{ type.error() };
		auto transpiled = transpile_typename_visitor{ {}, state }(member.type);
		if (!transpiled.has_value())
			return std::unexpected{ transpiled.error() };
		cpp << transpiled.value() << " " << member.name << ";\n";
	}

	for (const auto& constructor : type.constructors) {
		throw;
		//cpp << transpile(state, constructor, type).value() << "\n";
	}

	for (const auto& fn : type.methods) {
		throw;
		/*auto [_h, _cpp] = transpile(state, fn).value();
		cpp << _cpp;*/
	}

	cpp << "};\n\n";
	return std::pair{ "struct " + type.name + ";\n", cpp.str() };
}

transpile_t transpile(
	transpilation_state& state,
	const std::vector<std::tuple<NodeStructs::Typename, NodeStructs::ValueCategory, std::string>>& parameters
) {
	std::stringstream ss;
	bool first = true;
	for (const auto& [type, cat, name] : parameters) {
		auto s = transpile_typename_visitor{ {}, state }(type).value();
		if (std::holds_alternative<NodeStructs::Reference>(cat))
			s = "const " + std::move(s) + "&";
		else if (std::holds_alternative<NodeStructs::MutableReference>(cat))
			s = std::move(s) + "&";
		else if (std::holds_alternative<NodeStructs::Value>(cat))
			s = std::move(s) + "&&";
		if (first)
			first = false;
		else
			ss << ", ";
		ss << s << " " << name;
	}
	return ss.str();
}

transpile_t transpile(
	transpilation_state& state,
	const std::vector<NodeStructs::Statement>& statements
) {
	auto transpile_statement = transpile_statement_visitor{ {}, state };
	std::stringstream ss;
	for (const auto& statement : statements) {
		auto k = transpile_statement(statement);
		if (k.has_value())
			ss << indent(state.indent) << k.value();
		else
			return std::unexpected{ k.error() };
	}
	for (const auto& statement : statements)
		remove_added_variables(state, statement);

	return ss.str();
}

void remove_added_variables(
	transpilation_state& state,
	const NodeStructs::Statement& statement
) {
	std::visit(
		overload(
			[&](const NodeStructs::VariableDeclarationStatement& declaration) {
				state.variables[declaration.name].pop_back();
			},
			[&](const NodeStructs::BlockStatement& statement) {
				auto s = std::get<NodeStructs::BaseTypename>(statement.parametrized_block.value).type;

				if (state.named.blocks.contains(s)) {
					const NodeStructs::Block& block = *state.named.blocks.at(s);
					for (const auto& statement_in_block : block.statements)
						remove_added_variables(state, statement_in_block);
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
	transpilation_state& state,
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
									.type_template = *state.named.type_templates.at("Map"),
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
	transpilation_state& state,
	const NodeStructs::TypeCategory& type
) {
	return std::visit(
		overload(overload_default_error,
			[&](const std::reference_wrapper<const NodeStructs::Type>& _type) {
				const auto& type = _type.get();
				if (type.name == "Int" || type.name == "String" || type.name == "Bool")
					throw std::runtime_error("Cannot decompose type 'Int'");
				return type.memberVariables
					| std::views::transform([&](const auto& e) { return type_of_typename_visitor{ {}, state }(e.type).value(); })
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

std::optional<user_error> add_decomposed_for_iterator_variables(
	transpilation_state& state,
	const std::vector<std::variant<NodeStructs::VariableDeclaration, std::string>>& iterators,
	const NodeStructs::TypeCategory& it_type
) {
	if (iterators.size() == 0)
		return user_error{ "Expected at least 1 iterator" };
	auto decomposed_types = decomposed_type(state, it_type);
	if (iterators.size() != decomposed_types.size())
		throw std::runtime_error("");
	for (int i = 0; i < iterators.size(); ++i) {
		const auto& iterator = iterators.at(i);
		auto opt_error = std::visit(
			overload(overload_default_error,
				[&](const NodeStructs::VariableDeclaration& it) -> std::optional<user_error> {
					NodeStructs::TypeCategory iterator_type = type_of_typename_visitor{ {}, state }(it.type).value();
					if (decomposed_types.at(i) <=> iterator_type != std::weak_ordering::equivalent) {
						auto t = transpile_typename_visitor{ {}, state }(it.type);
						if (!t.has_value())
							return t.error();
						return user_error{ "Invalid type of iterator " + t.value() };
					}
					state.variables[it.name].push_back({ NodeStructs::Reference{}, type_of_typename_visitor{ {}, state }(it.type).value() });
					return std::nullopt;
				},
				[&](const std::string& it) -> std::optional<user_error> {
					state.variables[it].push_back({ NodeStructs::Reference{}, decomposed_types.at(i) });
					return std::nullopt;
				}
			),
			iterator
		);
		if (opt_error.has_value()) {
			for (int j = 0; j < i; ++j) {
				std::visit(
					overload(overload_default_error,
						[&](const NodeStructs::VariableDeclaration& it) {
							state.variables[it.name].pop_back();
						},
						[&](const std::string& it) {
							state.variables[it].pop_back();
						}
					),
					iterator
				);
			}
			return opt_error;
		}
	}
	return std::nullopt;
}

std::optional<user_error> add_for_iterator_variable(
	transpilation_state& state,
	const std::vector<std::variant<NodeStructs::VariableDeclaration, std::string>>& iterators,
	const NodeStructs::TypeCategory& it_type
) {
	if (iterators.size() != 1)
		return user_error{ "Expected 1 iterator" };
	return std::visit(
		overload(overload_default_error,
			[&](const NodeStructs::VariableDeclaration& it) -> std::optional<user_error> {
				NodeStructs::TypeCategory iterator_type = type_of_typename_visitor{ {}, state }(it.type).value();
				if (it_type <=> iterator_type != std::weak_ordering::equivalent) {
					auto t = transpile_typename_visitor{ {}, state }(it.type);
					if (!t.has_value())
						return t.error();
					return user_error{ "Invalid type of iterator " + t.value() };
				}
				state.variables[it.name].push_back({ NodeStructs::Reference{}, type_of_typename_visitor{ {}, state }(it.type).value() });
				return std::nullopt;
			},
			[&](const std::string& it) -> std::optional<user_error> {
				state.variables[it].push_back({ NodeStructs::Reference{}, it_type });
				return std::nullopt;
			}
		),
		iterators.at(0)
	);
}

void remove_for_iterator_variables(
	transpilation_state& state,
	const NodeStructs::ForStatement& statement
) {
	for (const auto& iterator : statement.iterators)
		std::visit(
			overload(overload_default_error,
				[&](const NodeStructs::VariableDeclaration& it) {
					state.variables[it.name].pop_back();
				},
				[&](const std::string& it) {
					state.variables[it].pop_back();
				}
			),
			iterator
		);
}

transpile_t transpile_arg(
	transpilation_state& state,
	const NodeStructs::FunctionArgument& arg
) {
	return transpile_expression_visitor{ {}, state }(std::get<NodeStructs::Expression>(arg));
}

transpile_t transpile_args(
	transpilation_state& state,
	const std::vector<NodeStructs::FunctionArgument>& args
) {
	std::stringstream ss;
	bool first = true;
	for (const auto& arg : args) {
		if (first)
			first = false;
		else
			ss << ", ";
		ss << transpile_arg(state, arg).value();
	}
	return ss.str();
}

std::expected<std::pair<NodeStructs::ValueCategory, NodeStructs::TypeCategory>, user_error> type_of_postfix_member(
	transpilation_state& state,
	const NodeStructs::Type& t,
	const std::string& property_name
) {
	const auto& v = t.memberVariables;
	auto pos = std::find_if(v.begin(), v.end(), [&](const NodeStructs::MemberVariable& m) { return m.name == property_name; });
	if (pos == v.end()) {
		auto u = transpile_type_visitor{ {}, state }(t);
		if (u.has_value())
			return std::unexpected{ user_error{ "Error: object of type `" + std::move(u).value() + "` has no member `" + property_name + "`\n"} };
		else
			return std::unexpected{ std::move(u).error() };
	}
	else
		return std::pair{ NodeStructs::Value{}, type_of_typename_visitor{ {}, state }(pos->type).value() };
}
