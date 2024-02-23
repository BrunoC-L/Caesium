#include <ranges>
#include <algorithm>
#include <source_location>
#include <format>
#include <stacktrace>
#include <numeric>

#include "toCPP.hpp"

#include "../utility/overload.hpp"
#include "../utility/vec_of_expected_to_expected_of_vec.hpp"

void insert_all_named_recursive_with_imports(const std::vector<NodeStructs::File>& project, std::map<std::string, Named>& named_by_file, const std::string& filename) {
	for (const NodeStructs::File& file : project)
		if (file.filename == filename) {
			Named& named = named_by_file[filename];

			for (const auto& e : file.types)
				named.types[e.name].push_back(&e);

			for (const auto& e : file.functions)
				named.functions[e.name].push_back(&e);

			for (const auto& e : file.interfaces)
				named.interfaces[e.name].push_back(&e);

			for (const auto& e : file.blocks)
				named.blocks[e.name].push_back(&e);

			for (const auto& e : file.templates)
				named.templates[e.name].push_back(&e);

			for (const auto& i : file.imports) {
				insert_all_named_recursive_with_imports(project, named_by_file, i.imported);
				Named& imported_named = named_by_file[i.imported];

				named.types.insert(imported_named.types.begin(), imported_named.types.end());
				named.functions.insert(imported_named.functions.begin(), imported_named.functions.end());
				named.interfaces.insert(imported_named.interfaces.begin(), imported_named.interfaces.end());
				named.blocks.insert(imported_named.blocks.begin(), imported_named.blocks.end());
				named.templates.insert(imported_named.templates.begin(), imported_named.templates.end());
			}

			return;
		}
	auto err = std::string("Invalid import \"") + filename + "\"";
	throw std::runtime_error(err);
}

void insert_aliases_recursive_with_imports(const std::vector<NodeStructs::File>& project, std::map<std::string, Named>& named_by_file, const std::string& filename) {
	for (const NodeStructs::File& file : project)
		if (file.filename == filename) {
			Named& named = named_by_file[filename];

			auto state = transpilation_state{ {}, Named(named) };
			for (const auto& alias : file.aliases) {
				auto t = type_of_typename({ state, 0 }, alias.aliasFrom);
				if (t.has_error())
					throw;
				named.type_aliases_typenames.emplace(alias.aliasTo, alias.aliasFrom);
				named.type_aliases.emplace(alias.aliasTo, std::move(t).value());
			}
			for (const auto& i : file.imports) {
				insert_aliases_recursive_with_imports(project, named_by_file, i.imported);
				Named& imported_named = named_by_file[i.imported];
				named.type_aliases.insert(imported_named.type_aliases.begin(), imported_named.type_aliases.end());
				named.type_aliases_typenames.insert(imported_named.type_aliases_typenames.begin(), imported_named.type_aliases_typenames.end());
			}

			return;
		}
	auto err = std::string("Invalid import \"") + filename + "\"";
	throw std::runtime_error(err);
}

transpile_header_cpp_t transpile(const std::vector<NodeStructs::File>& project) {
	for (const auto& file : project)
		for (const auto& fn : file.functions)
			if (fn.name == "main") {
				builtins _builtins{};
				variables_t variables;
				{
					variables["True"].push_back({ NodeStructs::Value{}, { _builtins.builtin_bool } });
					variables["False"].push_back({ NodeStructs::Value{}, { _builtins.builtin_bool } });
				}

				std::map<std::string, Named> named_by_file;

				for (const auto& file2 : project) {
					Named named_of_file;
					{
						named_of_file.templates["Set"].push_back(&_builtins.builtin_set);
						named_of_file.templates["Vector"].push_back(&_builtins.builtin_vector);
						named_of_file.templates["Map"].push_back(&_builtins.builtin_map);
						named_of_file.templates["push"].push_back(&_builtins.builtin_push);
						named_of_file.templates["insert"].push_back(&_builtins.builtin_insert);

						named_of_file.templates["print"].push_back(&_builtins.builtin_print);
						named_of_file.templates["println"].push_back(&_builtins.builtin_println);
						
						named_of_file.types["Int"].push_back(&_builtins.builtin_int);
						named_of_file.types["Bool"].push_back(&_builtins.builtin_bool);
						named_of_file.types["String"].push_back(&_builtins.builtin_string);
						named_of_file.types["Void"].push_back(&_builtins.builtin_void);
					}
					named_by_file[file2.filename] = named_of_file;
					insert_all_named_recursive_with_imports(project, named_by_file, file2.filename);
				}

				for (const auto& file2 : project) {
					Named& named_of_file = named_by_file[file2.filename];
					insert_aliases_recursive_with_imports(project, named_by_file, file2.filename);
				}

				transpilation_state state{ std::move(variables), Named(named_by_file[file.filename]) };
				state.traversed_types.insert(_builtins.builtin_int);
				state.traversed_types.insert(_builtins.builtin_bool);
				state.traversed_types.insert(_builtins.builtin_string);
				state.traversed_types.insert(_builtins.builtin_void);

				transpile_header_cpp_t k = transpile_main({ state, 0 }, fn);
				if (k.has_error())
					return k;

				std::stringstream h, cpp;
				h << default_includes << k.value().first;
				cpp << "#include \"expected.hpp\"\n";

				auto reverse_iterator = state.transpile_in_reverse_order.rbegin();
				for (; reverse_iterator != state.transpile_in_reverse_order.rend(); ++reverse_iterator) {
					const auto& [_h, _cpp] = *reverse_iterator;
					h << _h;
					cpp << _cpp;
				}
				for (const auto& [interface, members] : state.interface_symbol_to_members) {
					auto interface_repr = transpile_typename({ state, 0 }, interface);
					return_if_error(interface_repr);
					auto members_repr = transpile_types({ state, 0 }, members);
					return_if_error(members_repr);
					h << "using " << interface_repr.value() << " = Variant<" << members_repr.value() << ">;\n";
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
	return error{ "user error", "Missing \"main\" function" };
}

transpile_header_cpp_t transpile_main(
	transpilation_state_with_indent state,
	const NodeStructs::Function& fn
) {
	if (fn.parameters.size() != 1)
		return error{ "user error","\"main\" function requires 1 argument of type `Vector<String> ref`" };

	const auto& [type, cat, name] = fn.parameters.at(0);
	auto vector_str = NodeStructs::Typename{ NodeStructs::TemplatedTypename{
		NodeStructs::Typename{ NodeStructs::BaseTypename{ "Vector" } },
		{ NodeStructs::Typename{ NodeStructs::BaseTypename{ "String" } } }
	} };
	bool is_vec_str = type <=> vector_str == 0;
	if (!is_vec_str)
		return error{ "user error","\"main\" function using 1 argument must be of `Vector<String> ref` type" };
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
	transpilation_state_with_indent state,
	const NodeStructs::Function& fn
) {
	auto return_type = transpile_typename(state, fn.returnType);
	return_if_error(return_type);
	auto parameters = transpile(state, fn.parameters);
	return_if_error(parameters);
	for (auto&& [type_name, value_cat, name] : fn.parameters)
		state.state.variables[name].push_back(std::pair{ value_cat, type_of_typename(state, type_name).value() });
	auto statements = transpile(state.indented(), fn.statements);
	return_if_error(statements);
	for (auto&& [type_name, value_cat, name] : fn.parameters)
		state.state.variables[name].pop_back();
	auto common = return_type.value() + " " + fn.name + "(" + parameters.value() + ")";
	auto h = common + ";\n";
	auto cpp = common + " {\n" + statements.value() + "};\n";
	return std::pair{ h, cpp };
}

transpile_header_cpp_t transpile(
	transpilation_state_with_indent state,
	const NodeStructs::Type& type
) {
	std::stringstream cpp;
	cpp << "struct " << type.name << " {\n";

	for (const auto& alias : type.aliases) {
		throw;
		/*if (std::holds_alternative<NodeStructs::BaseTypename>(alias.aliasTo.value))
			cpp << "using "
			<< std::get<NodeStructs::BaseTypename>(alias.aliasTo.value).type
			<< " = " <<  transpile_typename(state, alias.aliasFrom).value()
			<< ";\n";
		else {
			auto err = "cannot alias to " + transpile_typename(state, alias.aliasTo).value();
			throw std::runtime_error(err);
		}*/
	}

	for (const auto& member : type.memberVariables) {
		auto type = type_of_typename(state, member.type);
		return_if_error(type);
		auto transpiled = transpile_typename(state, member.type);
		return_if_error(transpiled);
		cpp << transpiled.value() << " " << member.name << ";\n";
	}

	cpp << "};\n\n";
	return std::pair{ "struct " + type.name + ";\n", cpp.str() };
}

transpile_header_cpp_t transpile(
	transpilation_state_with_indent state,
	const NodeStructs::Interface& interface
) {
	return std::pair{ "", ""};
}

transpile_t transpile(
	transpilation_state_with_indent state,
	const std::vector<std::tuple<NodeStructs::Typename, NodeStructs::ParameterCategory, std::string>>& parameters
) {
	std::stringstream ss;
	bool first = true;
	for (const auto& [type, cat, name] : parameters) {
		auto s = transpile_typename(state, type).value();
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
	transpilation_state_with_indent state,
	const std::vector<NodeStructs::Statement>& statements
) {
	std::stringstream ss;
	for (const auto& statement : statements) {
		auto k = transpile_statement(state, statement);
		if (k.has_value())
			ss << indent(state.indent) << k.value();
		else
			return k.error();
	}
	for (const auto& statement : statements)
		remove_added_variables(state, statement);

	return ss.str();
}

void remove_added_variables(
	transpilation_state_with_indent state,
	const NodeStructs::Statement& statement
) {
	std::visit(
		overload(
			[&](const NodeStructs::VariableDeclarationStatement& declaration) {
				state.state.variables[declaration.name].pop_back();
			},
			[&](const NodeStructs::BlockStatement& statement) {
				auto s = std::get<NodeStructs::BaseTypename>(statement.parametrized_block.value).type;

				if (state.state.named.blocks.contains(s)) {
					const NodeStructs::Block& block = *state.state.named.blocks.at(s).back();
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

NodeStructs::UniversalType iterator_type(
	transpilation_state_with_indent state,
	const NodeStructs::UniversalType& type
) {
	return std::visit(
		overload(overload_default_error,
			[&](const std::reference_wrapper<const NodeStructs::Type>& type) -> NodeStructs::UniversalType {
				throw;
			},
			/*[&](const NodeStructs::TypeTemplateInstanceType& type) -> NodeStructs::UniversalType {
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
								return NodeStructs::UniversalType{ NodeStructs::TypeTemplateInstanceType{
									.type_template = *state.state.named.type_templates.at("Map").back(),
									.template_arguments = { type.template_arguments.at(0), type.template_arguments.at(1) },
								} };
							else
								throw;
						else
							throw;
			},*/
			[&](const NodeStructs::AggregateType&) -> NodeStructs::UniversalType {
				throw;
			},
			[&](const NodeStructs::TypeType&) -> NodeStructs::UniversalType {
				throw;
			},
			[&](const NodeStructs::FunctionType&) -> NodeStructs::UniversalType {
				throw;
			},
			[&](const NodeStructs::InterfaceType&) -> NodeStructs::UniversalType {
				throw;
			},
			[&](const NodeStructs::UnionType&) -> NodeStructs::UniversalType {
				throw;
			},
			[&](const NodeStructs::VectorType& t) -> NodeStructs::UniversalType {
				return t.value_type;
			},
			[&](const NodeStructs::SetType&) -> NodeStructs::UniversalType {
				throw;
			},
			[&](const NodeStructs::MapType&) -> NodeStructs::UniversalType {
				throw;
			},
			[&](const NodeStructs::Template&) -> NodeStructs::UniversalType {
				throw;
			},
			[&](const NodeStructs::BuiltInType&) -> NodeStructs::UniversalType {
				throw;
			},
			[&](const std::string&) -> NodeStructs::UniversalType {
				throw;
			},
			[&](const double&) -> NodeStructs::UniversalType {
				throw;
			},
			[&](const int&) -> NodeStructs::UniversalType {
				throw;
			},
			[&](const bool&) -> NodeStructs::UniversalType {
				throw;
			}
		),
		type.value
	);
}

std::vector<NodeStructs::UniversalType> decomposed_type(
	transpilation_state_with_indent state,
	const NodeStructs::UniversalType& type
) {
	return std::visit(
		overload(overload_default_error,
			[&](const std::reference_wrapper<const NodeStructs::Type>& _type) {
				const auto& type = _type.get();
				if (type.name == "Int" || type.name == "String" || type.name == "Bool")
					throw std::runtime_error("Cannot decompose type 'Int'");
				return type.memberVariables
					| std::views::transform([&](const auto& e) { return type_of_typename(state, e.type).value(); })
					| to_vec();
			},
			/*[&](const NodeStructs::TypeTemplateInstanceType& type) -> std::vector<NodeStructs::UniversalType> {
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
			},*/
			[&](const NodeStructs::AggregateType&) -> std::vector<NodeStructs::UniversalType> {
				throw;
			},
			[&](const NodeStructs::TypeType&) -> std::vector<NodeStructs::UniversalType> {
				throw;
			},
			[&](const NodeStructs::FunctionType&) -> std::vector<NodeStructs::UniversalType> {
				throw;
			},
			[&](const NodeStructs::InterfaceType&) -> std::vector<NodeStructs::UniversalType> {
				throw;
			},
			[&](const NodeStructs::UnionType&) -> std::vector<NodeStructs::UniversalType> {
				throw;
			},
			[&](const NodeStructs::VectorType&) -> std::vector<NodeStructs::UniversalType> {
				throw;
			},
			[&](const NodeStructs::SetType&) -> std::vector<NodeStructs::UniversalType> {
				throw;
			},
			[&](const NodeStructs::MapType&) -> std::vector<NodeStructs::UniversalType> {
				throw;
			},
			[&](const NodeStructs::Template&) -> std::vector<NodeStructs::UniversalType> {
				throw;
			},
			[&](const NodeStructs::BuiltInType&) -> std::vector<NodeStructs::UniversalType> {
				throw;
			},
			[&](const std::string&) -> std::vector<NodeStructs::UniversalType> {
				throw;
			},
			[&](const double&) -> std::vector<NodeStructs::UniversalType> {
				throw;
			},
			[&](const int&) -> std::vector<NodeStructs::UniversalType> {
				throw;
			},
			[&](const bool&) -> std::vector<NodeStructs::UniversalType> {
				throw;
			}
		),
		type.value
	);
}

std::optional<error> add_decomposed_for_iterator_variables(
	transpilation_state_with_indent state,
	const std::vector<std::variant<NodeStructs::VariableDeclaration, std::string>>& iterators,
	const NodeStructs::UniversalType& it_type
) {
	if (iterators.size() == 0)
		return error{ "user error","Expected at least 1 iterator" };
	auto decomposed_types = decomposed_type(state, it_type);
	if (iterators.size() != decomposed_types.size())
		throw std::runtime_error("");
	for (int i = 0; i < iterators.size(); ++i) {
		const auto& iterator = iterators.at(i);
		auto opt_error = std::visit(
			overload(overload_default_error,
				[&](const NodeStructs::VariableDeclaration& it) -> std::optional<error> {
					NodeStructs::UniversalType iterator_type = type_of_typename(state, it.type).value();
					if (decomposed_types.at(i) <=> iterator_type != std::weak_ordering::equivalent) {
						auto t = transpile_typename(state, it.type);
						return_if_error(t);
						return error{ "user error","Invalid type of iterator " + t.value() };
					}
					state.state.variables[it.name].push_back({ NodeStructs::Reference{}, type_of_typename(state, it.type).value() });
					return std::nullopt;
				},
				[&](const std::string& it) -> std::optional<error> {
					state.state.variables[it].push_back({ NodeStructs::Reference{}, decomposed_types.at(i) });
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
							state.state.variables[it.name].pop_back();
						},
						[&](const std::string& it) {
							state.state.variables[it].pop_back();
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

std::optional<error> add_for_iterator_variable(
	transpilation_state_with_indent state,
	const std::vector<std::variant<NodeStructs::VariableDeclaration, std::string>>& iterators,
	const NodeStructs::UniversalType& it_type
) {
	if (iterators.size() != 1)
		return error{ "user error","Expected 1 iterator" };
	return std::visit(
		overload(overload_default_error,
			[&](const NodeStructs::VariableDeclaration& it) -> std::optional<error> {
				NodeStructs::UniversalType iterator_type = type_of_typename(state, it.type).value();
				if (it_type <=> iterator_type != std::weak_ordering::equivalent) {
					auto t = transpile_typename(state, it.type);
					return_if_error(t);
					return error{ "user error","Invalid type of iterator " + t.value() };
				}
				state.state.variables[it.name].push_back({ NodeStructs::Reference{}, type_of_typename(state, it.type).value() });
				return std::nullopt;
			},
			[&](const std::string& it) -> std::optional<error> {
				state.state.variables[it].push_back({ NodeStructs::Reference{}, it_type });
				return std::nullopt;
			}
		),
		iterators.at(0)
	);
}

void remove_for_iterator_variables(
	transpilation_state_with_indent state,
	const NodeStructs::ForStatement& statement
) {
	for (const auto& iterator : statement.iterators)
		std::visit(
			overload(overload_default_error,
				[&](const NodeStructs::VariableDeclaration& it) {
					state.state.variables[it.name].pop_back();
				},
				[&](const std::string& it) {
					state.state.variables[it].pop_back();
				}
			),
			iterator
		);
}

transpile_t transpile_arg(
	transpilation_state_with_indent state,
	const NodeStructs::FunctionArgument& arg
) {
	return transpile_expression(state, std::get<NodeStructs::Expression>(arg)).transform([](auto&& x) { return x.representation; });
}

transpile_t transpile_args(
	transpilation_state_with_indent state,
	const std::vector<NodeStructs::FunctionArgument>& args
) {
	std::stringstream ss;
	bool first = true;
	for (const auto& arg : args) {
		if (first)
			first = false;
		else
			ss << ", ";

		auto repr = transpile_arg(state, arg);
		return_if_error(repr);
		ss << repr.value();
	}
	return ss.str();
}

transpile_t transpile_expressions(transpilation_state_with_indent state, const std::vector<NodeStructs::Expression>& args) {
	std::stringstream ss;
	bool first = true;
	for (const auto& arg : args) {
		if (first)
			first = false;
		else
			ss << ", ";

		auto repr = transpile_expression(state, arg);
		return_if_error(repr);
		ss << repr.value().representation;
	}
	return ss.str();
}

transpile_t transpile_types(transpilation_state_with_indent state, const std::vector<NodeStructs::UniversalType>& args) {
	auto vec = vec_of_expected_to_expected_of_vec(args | LIFT_TRANSFORM_X(T, transpile_type(state, T)) | to_vec());
	return_if_error(vec);
	return std::accumulate(
		std::next(vec.value().begin()),
		vec.value().end(),
		vec.value()[0],
		[](const std::string& a, const std::string& b){ return a + ", " + b; }
	);
}

//expected<std::pair<NodeStructs::ParameterCategory, NodeStructs::UniversalType>> type_of_postfix_member(
//	transpilation_state_with_indent state,
//	const NodeStructs::Type& t,
//	const std::string& property_name
//) {
//	const auto& v = t.memberVariables;
//	auto pos = std::find_if(v.begin(), v.end(), [&](const NodeStructs::MemberVariable& m) { return m.name == property_name; });
//	if (pos == v.end()) {
//		auto u = transpile_type(state, { t });
//		if (u.has_value())
//			return error{ "user error","Error: object of type `" + std::move(u).value() + "` has no member `" + property_name + "`\n"};
//		else
//			return std::move(u).error();
//	}
//	else
//		return std::pair{ NodeStructs::Value{}, type_of_typename(state, pos->type).value() };
//}

std::string template_name(std::string original_name, const std::vector<std::string>& args) {
	std::stringstream ss;
	ss << original_name;
	for (const auto& arg : args)
		ss << "_" << arg;
	return ss.str();
}

std::string template_name(std::string original_name, const std::vector<NodeStructs::Expression>& arguments) {
	return template_name(original_name, arguments | LIFT_TRANSFORM(expression_for_template) | to_vec());
}

// return source is assignable to target
bool is_assignable_to(
	transpilation_state_with_indent state,
	const NodeStructs::UniversalType& target,
	const NodeStructs::UniversalType& source
) {
	return std::visit(
		overload(
			[&](const auto& e, const auto& u) {
				return false;
			},
			[&](const NodeStructs::InterfaceType& e, std::reference_wrapper<const NodeStructs::Type> u) {
				for (const auto& [type_name, name] : e.interface.get().memberVariables) {
					auto t = type_of_typename(state, type_name);
					if (t.has_error())
						throw;
					for (const auto& [source_type_name, source_name] : u.get().memberVariables)
						if (name == source_name) {
							auto t2 = type_of_typename(state, source_type_name);
							if (t2.has_error())
								throw;
							if (t.value() <=> t2.value() != std::weak_ordering::equivalent)
								return false;
						}
				}
				auto& interface_members = state.state.interface_symbol_to_members[
					NodeStructs::Typename{ NodeStructs::BaseTypename{ e.interface.get().name} }
				];
				auto new_member = NodeStructs::UniversalType{ u };
				for (const auto& member : interface_members)
					if (member <=> new_member == std::weak_ordering::equivalent)
						return true;
				interface_members.push_back(std::move(new_member));
				return true;
			},
			[&](const NodeStructs::InterfaceType& e, const NodeStructs::TypeType& u) {
				return false;
			}
		),
		target.value, source.value
	);
}

transpile_t expr_to_printable(transpilation_state_with_indent state, const NodeStructs::Expression& expr) {
	auto t = transpile_expression(state, expr).transform([](auto&& x) { return std::pair{ std::move(x).value_category, std::move(x).type }; });
	return_if_error(t);

	if (std::holds_alternative<std::reference_wrapper<const NodeStructs::Type>>(t.value().second.value)) {
		std::stringstream ss;
		const NodeStructs::Type& t_ref = std::get<std::reference_wrapper<const NodeStructs::Type>>(t.value().second.value).get();
		if (t_ref.name == "String") {
			auto expr_repr = transpile_expression(state, expr);
			return_if_error(expr_repr);
			ss << "std::string(\"\\\"\") + " << expr_repr.value().representation << " + std::string(\"\\\"\")";
		}
		else {
			throw;
			/*ss << t_ref.name << "{";
			for (const auto& [member_typename, member_name] : t_ref.memberVariables) {
				auto typename_t = type_of_typename(state, member_typename);
				return_if_error(typename_t);
				auto member = NodeStructs::PropertyAccessExpression{ .operand = expr, .property_name = member_name };
				auto member_repr = expr_to_printable(state, { member });
				return_if_error(member_repr);
				ss << member_name << " = \" << " << member_repr.value();
			}*/
		}
		return ss.str();
	}
	else if (std::holds_alternative<NodeStructs::InterfaceType>(t.value().second.value)) {
		std::stringstream ss;
		const auto& t_ref = std::get<NodeStructs::InterfaceType>(t.value().second.value).interface.get();
		ss << "\"" << t_ref.name << "{";
		for (const auto& [member_typename, member_name] : t_ref.memberVariables) {
			auto typename_t = type_of_typename(state, member_typename);
			return_if_error(typename_t);
			auto member = NodeStructs::PropertyAccessExpression{ .operand = expr, .property_name = member_name };
			auto member_repr = expr_to_printable(state, { member });
			return_if_error(member_repr);
			ss << member_name << " = \" << " << member_repr.value();
		}
		ss << " << \"}\"";
		return ss.str();
	}
	else {
		throw;
	}
}

#include "../type_visitor/transpile_type_visitor.hpp";
transpile_t transpile_type(
	transpilation_state_with_indent state,
	const NodeStructs::UniversalType& type
) {
	return transpile_type_visitor{ {}, state }(type);
}

#include "../type_visitor/traverse_type_visitor.hpp";
std::optional<error> traverse_type(
	transpilation_state_with_indent state,
	const NodeStructs::UniversalType& type
) {
	return traverse_type_visitor{ {}, state }(type);
}

#include "../type_visitor/type_of_function_like_call_with_args_visitor.hpp";
expected<std::pair<NodeStructs::ParameterCategory, NodeStructs::UniversalType>> type_of_function_like_call_with_args(
	transpilation_state_with_indent state,
	const std::vector<NodeStructs::FunctionArgument>& arguments,
	const NodeStructs::UniversalType& type
) {
	return type_of_function_like_call_with_args_visitor{ {}, state, arguments }(type);
}

#include "../type_visitor/type_of_postfix_member_visitor.hpp";
expected<std::pair<NodeStructs::ParameterCategory, NodeStructs::UniversalType>> type_of_postfix_member(
	transpilation_state_with_indent state,
	const std::string& property_name,
	const NodeStructs::UniversalType& type
) {
	return type_of_postfix_member_visitor{ {}, state, property_name }(type);
}

#include "../expression_visitor/expression_for_template_visitor.hpp"
std::string expression_for_template(
	const NodeStructs::Expression& expr
) {
	return expression_for_template_visitor{ {} }(expr);
}

#include "../expression_visitor/transpile_call_expression_with_args.hpp"
transpile_t transpile_call_expression_with_args(
	transpilation_state_with_indent state,
	const std::vector<NodeStructs::FunctionArgument>& arguments,
	const NodeStructs::Expression& expr
) {
	return transpile_call_expression_with_args_visitor{ {}, state, arguments }(expr);
}

#include "../expression_visitor/transpile_expression_visitor.hpp"
transpile_t2 transpile_expression(
	transpilation_state_with_indent state,
	const NodeStructs::Expression& expr
) {
	return transpile_expression_visitor{ {}, state }(expr);
}

//#include "../expression_visitor/type_of_expression_visitor.hpp"
//expected<std::pair<NodeStructs::ValueCategory, NodeStructs::UniversalType>> type_of_expression(
//	transpilation_state_with_indent state,
//	const NodeStructs::Expression& expr
//) {
//	return type_of_expression_visitor{ {}, state }(expr);
//}

#include "../statement_visitor/transpile_statement_visitor.hpp"
transpile_t transpile_statement(
	transpilation_state_with_indent state,
	const NodeStructs::Statement& statement
) {
	return transpile_statement_visitor{ {}, state }(statement);
}

#include "../typename_visitor/transpile_typename_visitor.hpp"
transpile_t transpile_typename(
	transpilation_state_with_indent state,
	const NodeStructs::Typename& tn
) {
	return transpile_typename_visitor{ {}, state }(tn);
}

#include "../typename_visitor/type_of_typename_visitor.hpp"
expected<NodeStructs::UniversalType> type_of_typename(
	transpilation_state_with_indent state,
	const NodeStructs::Typename& tn
) {
	return type_of_typename_visitor{ {}, state }(tn);
}

#include "../typename_visitor/type_template_of_typename_visitor.hpp"
expected<NodeStructs::UniversalType> type_template_of_typename(
	transpilation_state_with_indent state,
	const std::vector<NodeStructs::Typename>& templated_with,
	const NodeStructs::Typename& tn
) {
	return type_template_of_typename_visitor{ {}, state, templated_with }(tn);
}
