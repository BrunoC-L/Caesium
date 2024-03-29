#include <algorithm>
#include <source_location>
#include <format>
#include <stacktrace>
#include <numeric>

#include "toCPP.hpp"

#include "../utility/overload.hpp"
#include "../utility/vec_of_expected_to_expected_of_vec.hpp"
#include "default_includes.hpp"

static void add_builtins(Named& named, const builtins& _builtins) {
	named.types["Int"].push_back(&_builtins.builtin_int);
	named.types["Bool"].push_back(&_builtins.builtin_bool);
	named.types["String"].push_back(&_builtins.builtin_string);
	named.types["Floating"].push_back(&_builtins.builtin_double);
	named.types["Void"].push_back(&_builtins.builtin_void);

	named.templates["Vector"].push_back(&_builtins.builtin_vector);
	named.templates["Set"].push_back(&_builtins.builtin_set);
	named.templates["Map"].push_back(&_builtins.builtin_map);

	named.templates["push"].push_back(&_builtins.builtin_push);
	named.templates["insert"].push_back(&_builtins.builtin_insert);

	named.templates["print"].push_back(&_builtins.builtin_print);
	named.templates["println"].push_back(&_builtins.builtin_println);

	named.namespaces["filesystem"].push_back(&_builtins.filesystem_ns);
}

static void traverse_builtins(transpilation_state& state, const builtins& _builtins) {
	state.traversed_types.insert(_builtins.builtin_int);
	state.traversed_types.insert(_builtins.builtin_bool);
	state.traversed_types.insert(_builtins.builtin_string);
	state.traversed_types.insert(_builtins.builtin_double);
	state.traversed_types.insert(_builtins.builtin_void);

	state.traversed_functions.insert(_builtins.filesystem_ns.functions.at(0));
	state.traversed_functions.insert(_builtins.filesystem_ns.functions.at(1));
	state.traversed_types.insert(_builtins.filesystem_ns.types.at(0));
	state.traversed_types.insert(_builtins.filesystem_ns.types.at(1));
}

std::optional<error> insert_all_named_recursive_with_imports(
	const std::vector<NodeStructs::File>& project,
	std::map<std::string, Named>& named_by_file,
	const std::string& filename
) {
	for (const NodeStructs::File& file : project)
		if (file.filename == filename) {
			Named& named = named_by_file[filename];

			for (const auto& e : file.types)
				named.types[e.name].push_back(&e);

			for (const auto& e : file.functions)
				if (uses_auto(e))
					named.functions_using_auto[e.name].push_back(&e);
				else
					named.functions[e.name].push_back(&e);

			for (const auto& e : file.interfaces)
				named.interfaces[e.name].push_back(&e);

			for (const auto& e : file.blocks)
				named.blocks[e.name].push_back(&e);

			for (const auto& e : file.templates)
				named.templates[e.name].push_back(&e);

			for (const auto& e : file.namespaces)
				named.namespaces[e.name].push_back(&e);

			for (const auto& i : file.imports) {
				auto opt_error = insert_all_named_recursive_with_imports(project, named_by_file, i.imported);
				if (opt_error.has_value())
					return opt_error;
				Named& imported_named = named_by_file[i.imported];

				named.types.insert(imported_named.types.begin(), imported_named.types.end());
				named.functions.insert(imported_named.functions.begin(), imported_named.functions.end());
				named.functions_using_auto.insert(imported_named.functions_using_auto.begin(), imported_named.functions_using_auto.end());
				named.interfaces.insert(imported_named.interfaces.begin(), imported_named.interfaces.end());
				named.blocks.insert(imported_named.blocks.begin(), imported_named.blocks.end());
				named.templates.insert(imported_named.templates.begin(), imported_named.templates.end());
				named.namespaces.insert(imported_named.namespaces.begin(), imported_named.namespaces.end());
			}

			return std::nullopt;
		}
	return error{
		"user error",
		std::string("Invalid import \"") + filename + "\""
	};
}

std::optional<error> insert_aliases_recursive_with_imports(
	const std::vector<NodeStructs::File>& project,
	std::map<std::string, Named>& named_by_file,
	const std::string& filename
) {
	for (const NodeStructs::File& file : project)
		if (file.filename == filename) {
			Named& named = named_by_file[filename];

			auto state = transpilation_state{ {}, Named(named) };
			for (const auto& alias : file.aliases)
				named.type_aliases_typenames.emplace(alias.aliasFrom, alias.aliasTo);
			for (const auto& i : file.imports) {
				auto opt_e = insert_aliases_recursive_with_imports(project, named_by_file, i.imported);
				if (opt_e.has_value())
					return opt_e;
				Named& imported_named = named_by_file[i.imported];
				named.type_aliases_typenames.insert(imported_named.type_aliases_typenames.begin(), imported_named.type_aliases_typenames.end());
			}

			return std::nullopt;
		}
	return error{
		"user error",
		"invalid import `" + filename + "`"
	};
}

transpile_header_cpp_t transpile(const std::vector<NodeStructs::File>& project) {
	for (const auto& file : project)
		for (const auto& fn : file.functions)
			if (fn.name == "main") {
				builtins _builtins{};
				variables_t variables{};
				{
					variables["True"].push_back(
						{
							NodeStructs::Value{},
							{ NodeStructs::PrimitiveType{ true } }
						}
					);
					variables["False"].push_back(
						{
							NodeStructs::Value{},
							{ NodeStructs::PrimitiveType{ false } }
						}
					);
				}

				std::map<std::string, Named> named_by_file;

				for (const auto& file2 : project) {
					Named named_of_file;
					add_builtins(named_of_file, _builtins);
					named_by_file[file2.filename] = named_of_file;
					auto opt_error = insert_all_named_recursive_with_imports(project, named_by_file, file2.filename);
					if (opt_error.has_value())
						return opt_error.value();
				}

				for (const auto& file2 : project) {
					Named& named_of_file = named_by_file[file2.filename];
					auto opt_e = insert_aliases_recursive_with_imports(project, named_by_file, file2.filename);
					if (opt_e.has_value())
						return opt_e.value();
				}

				transpilation_state state{ std::move(variables), Named(named_by_file[file.filename]) };

				traverse_builtins(state, _builtins);

				transpile_header_cpp_t k = transpile_main({ state, 0 }, fn);
				if (k.has_error())
					return k;

				std::stringstream h, cpp;
				h << "#pragma once\n#include \"defaults.hpp\"\n\n" << k.value().first;
				cpp << "#include \"expected.hpp\"\n";

				auto reverse_iterator = state.transpile_in_reverse_order.rbegin();
				for (; reverse_iterator != state.transpile_in_reverse_order.rend(); ++reverse_iterator) {
					const auto& [_h, _cpp] = *reverse_iterator;
					h << _h;
					cpp << _cpp;
				}
				for (const auto& [interface, members] : state.interface_symbol_to_members) {
					auto unindented = transpilation_state_with_indent{ state, 0 };
					auto interface_repr = transpile_typename(unindented, interface);
					return_if_error(interface_repr);
					auto k = members | LIFT_TRANSFORM_X(T, typename_of_type(unindented, T)) | to_vec();
					auto v = vec_of_expected_to_expected_of_vec(k);
					return_if_error(v);
					auto members_repr = transpile_typenames(unindented, v.value());
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
	if (type <=> vector_str != std::weak_ordering::equivalent)
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
	for (auto&& [type_name, value_cat, name] : fn.parameters) {
		auto t = type_of_typename(state, type_name);
		return_if_error(t);
		if (std::holds_alternative<NodeStructs::Value>(value_cat))
			state.state.variables[name].push_back({ NodeStructs::MutableReference{}, t.value() });
		else
			state.state.variables[name].push_back({ value_cat, t.value() });
	}
	auto statements = transpile(state.indented(), fn.statements);
	return_if_error(statements);
	for (auto&& [type_name, value_cat, name] : fn.parameters)
		state.state.variables[name].pop_back();
	if (fn.name_space.has_value()) {
		auto common = return_type.value() + " " + transpile_typename(state, fn.name_space.value()).value() + "__" + fn.name + "(" + parameters.value() + ")";
		auto h = common + ";\n";
		auto cpp = common + " {\n" + statements.value() + "};\n";
		return std::pair{ h, cpp };
	}
	else {
		auto common = return_type.value() + " " + fn.name + "(" + parameters.value() + ")";
		auto h = common + ";\n";
		auto cpp = common + " {\n" + statements.value() + "};\n";
		return std::pair{ h, cpp };
	}
}

transpile_header_cpp_t transpile(
	transpilation_state_with_indent state,
	const NodeStructs::Type& type
) {
	std::stringstream h, cpp;
	if (type.name_space.has_value()) {
		h << "struct " << transpile_typename(state, type.name_space.value()).value() << "__" << type.name << ";\n";
		cpp << "struct " << transpile_typename(state, type.name_space.value()).value() << "__" << type.name << " {\n";
	}
	else {
		h << "struct " << type.name << ";\n";
		cpp << "struct " << type.name << " {\n";
	}

	for (const auto& alias : type.aliases)
		throw;

	for (const auto& member : type.memberVariables) {
		auto type = type_of_typename(state, member.type);
		return_if_error(type);
		auto transpiled = transpile_typename(state, member.type);
		return_if_error(transpiled);
		cpp << transpiled.value() << " " << member.name << ";\n";
	}

	cpp << "};\n\n";
	return std::pair{ h.str(), cpp.str()};
}

transpile_header_cpp_t transpile(
	transpilation_state_with_indent state,
	const NodeStructs::Interface& interface
) {
	return std::pair{ "", ""};
}

transpile_t transpile(
	transpilation_state_with_indent state,
	const std::vector<NodeStructs::FunctionParameter>& parameters
) {
	std::stringstream ss;
	bool first = true;
	for (const auto& [type, cat, name] : parameters) {
		auto tn = transpile_typename(state, type);
		return_if_error(tn);
		auto s = tn.value();
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

NodeStructs::MetaType iterator_type(
	transpilation_state_with_indent state,
	const NodeStructs::MetaType& type
) {
	return std::visit(overload(
		[&](const auto& x) -> NodeStructs::MetaType {
			throw;
		},
		[&](const NodeStructs::VectorType& vt) ->NodeStructs::MetaType {
			return vt.value_type.get();
		}
	), type.type);
}

std::vector<NodeStructs::MetaType> decompose_type(
	transpilation_state_with_indent state,
	const NodeStructs::MetaType& type
) {
	return std::visit(
		overload(overload_default_error,
			[&](const NodeStructs::PrimitiveType& _type) -> std::vector<NodeStructs::MetaType> {
				throw;
			},
			[&](const std::reference_wrapper<const NodeStructs::Type>& _type) -> std::vector<NodeStructs::MetaType> {
				throw;
			},
			/*[&](const std::reference_wrapper<const NodeStructs::Type>& _type) -> std::vector<NodeStructs::MetaType> {
				const auto& type = _type.get();
				if (type.name == "Int" || type.name == "String" || type.name == "Bool")
					throw std::runtime_error("Cannot decompose type 'Int'");
				return type.memberVariables
					| std::views::transform([&](const auto& e) { return type_of_typename(state, e.type).value(); })
					| to_vec();
			},
			[&](const NodeStructs::TypeTemplateInstanceType& type) -> std::vector<NodeStructs::MetaType> {
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
			/*[&](const NodeStructs::AggregateType&) -> std::vector<NodeStructs::MetaType> {
				throw;
			},*/
			[&](const NodeStructs::FunctionType&) -> std::vector<NodeStructs::MetaType> {
				throw;
			},
			[&](const NodeStructs::InterfaceType&) -> std::vector<NodeStructs::MetaType> {
				throw;
			},
			[&](const NodeStructs::NamespaceType&) -> std::vector<NodeStructs::MetaType> {
				throw;
			},
			[&](const NodeStructs::UnionType&) -> std::vector<NodeStructs::MetaType> {
				throw;
			},
			[&](const NodeStructs::TemplateType&) -> std::vector<NodeStructs::MetaType> {
				throw;
			},
			[&](const NodeStructs::Vector&) -> std::vector<NodeStructs::MetaType> {
				throw;
			},
			[&](const NodeStructs::VectorType&) -> std::vector<NodeStructs::MetaType> {
				throw;
			},
			[&](const NodeStructs::Set&) -> std::vector<NodeStructs::MetaType> {
				throw;
			},
			[&](const NodeStructs::SetType&) -> std::vector<NodeStructs::MetaType> {
				throw;
			},
			[&](const NodeStructs::Map&) -> std::vector<NodeStructs::MetaType> {
				throw;
			},
			[&](const NodeStructs::MapType&) -> std::vector<NodeStructs::MetaType> {
				throw;
			}
		),
		type.type
	);
}

std::optional<error> add_decomposed_for_iterator_variables(
	transpilation_state_with_indent state,
	const std::vector<std::variant<NodeStructs::VariableDeclaration, std::string>>& iterators,
	const NodeStructs::MetaType& it_type
) {
	if (iterators.size() == 0)
		return error{ "user error","Expected at least 1 iterator" };
	auto decomposed_type = decompose_type(state, it_type);
	if (iterators.size() != decomposed_type.size())
		throw std::runtime_error("");
	for (int i = 0; i < iterators.size(); ++i) {
		const auto& iterator = iterators.at(i);
		auto opt_error = std::visit(
			overload(overload_default_error,
				[&](const NodeStructs::VariableDeclaration& it) -> std::optional<error> {
					NodeStructs::MetaType iterator_type = type_of_typename(state, it.type).value();
					if (decomposed_type.at(i) <=> iterator_type != std::weak_ordering::equivalent) {
						auto t = transpile_typename(state, it.type);
						return_if_error(t);
						return error{ "user error","Invalid type of iterator " + t.value() };
					}
					throw;
					//state.state.variables[it.name].push_back({ NodeStructs::Reference{}/*, it.type*/, type_of_typename(state, it.type).value() });
					return std::nullopt;
				},
				[&](const std::string& it) -> std::optional<error> {
					throw;
					//state.state.variables[it].push_back({ NodeStructs::Reference{}, decomposed_type.at(i) });
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
	const NodeStructs::MetaType& it_type
) {
	if (iterators.size() != 1)
		return error{ "user error","Expected 1 iterator" };
	return std::visit(
		overload(overload_default_error,
			[&](const NodeStructs::VariableDeclaration& it) -> std::optional<error> {
				NodeStructs::MetaType iterator_type = type_of_typename(state, it.type).value();
				if (it_type <=> iterator_type != std::weak_ordering::equivalent) {
					auto t = transpile_typename(state, it.type);
					return_if_error(t);
					return error{ "user error","Invalid type of iterator " + t.value() };
				}
				throw;
				//state.state.variables[it.name].push_back({ NodeStructs::Reference{}, type_of_typename(state, it.type).value() });
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
	auto expr_info = transpile_expression(state, arg.expr);
	return_if_error(expr_info);
	if (!std::holds_alternative<non_type_information>(expr_info.value()))
		throw;
	const auto& expr_info_ok = std::get<non_type_information>(expr_info.value());

	if (!arg.category.has_value()) {
		return std::visit(overload(
			[&](const NodeStructs::Reference& e) -> transpile_t {
				return expr_info_ok.representation;
			},
			[&](const NodeStructs::Value& e) -> transpile_t {
				return expr_info_ok.representation;
			},
			[&](const NodeStructs::MutableReference& e) -> transpile_t {
				return expr_info_ok.representation;
			}
		), expr_info_ok.value_category);
	}
	else {
		return std::visit(overload(
			[&](const NodeStructs::Reference& expr_cat, const NodeStructs::Reference& arg_cat) -> transpile_t {
				return expr_info_ok.representation;
			},
			[&](const NodeStructs::Value& expr_cat, const NodeStructs::Reference& arg_cat) -> transpile_t {
				return error{ "user error", "can't reference a temporary value" };
			},
			[&](const NodeStructs::MutableReference& expr_cat, const NodeStructs::Reference& arg_cat) -> transpile_t {
				return expr_info_ok.representation;
			},
			[&](const NodeStructs::Reference& expr_cat, const NodeStructs::MutableReference& arg_cat) -> transpile_t {
				return error{ "user error", "can't mutably reference a reference" };
			},
			[&](const NodeStructs::Value& expr_cat, const NodeStructs::MutableReference& arg_cat) -> transpile_t {
				return error{ "user error", "can't mutably reference a temporary value" };
			},
			[&](const NodeStructs::MutableReference& expr_cat, const NodeStructs::MutableReference& arg_cat) -> transpile_t {
				return expr_info_ok.representation;
			},
			[&](const NodeStructs::Reference& expr_cat, const NodeStructs::Copy& arg_cat) -> transpile_t {
				// todo check if copyable
				auto tn = typename_of_type(state, expr_info_ok.type.type);
				return_if_error(tn);
				auto tn_repr = transpile_typename(state, tn.value());
				return_if_error(tn_repr);
				return tn_repr.value() + "{ " + expr_info_ok.representation + " }";
			},
			[&](const NodeStructs::Value& expr_cat, const NodeStructs::Copy& arg_cat) -> transpile_t {
				return error{ "user error", "do not copy temporary values" };
			},
			[&](const NodeStructs::MutableReference& expr_cat, const NodeStructs::Copy& arg_cat) -> transpile_t {
				// todo check if copyable
				auto tn = typename_of_type(state, expr_info_ok.type.type);
				return_if_error(tn);
				auto tn_repr = transpile_typename(state, tn.value());
				return_if_error(tn_repr);
				return tn_repr.value() + "{ " + expr_info_ok.representation + " }";
			},
			[&](const NodeStructs::Reference& expr_cat, const NodeStructs::Move& arg_cat) -> transpile_t {
				return error{ "user error", "do not move from a non mutable reference" };
			},
			[&](const NodeStructs::Value& expr_cat, const NodeStructs::Move& arg_cat) -> transpile_t {
				return error{ "user error", "do not move temporary values" };
			},
			[&](const NodeStructs::MutableReference& expr_cat, const NodeStructs::Move& arg_cat) -> transpile_t {
				return "std::move(" + expr_info_ok.representation + ")";
			}
		), expr_info_ok.value_category, arg.category.value());
	}
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
		if (!std::holds_alternative<non_type_information>(repr.value()))
			throw;
		const auto& repr_ok = std::get<non_type_information>(repr.value());
		ss << repr_ok.representation;
	}
	return ss.str();
}

transpile_t transpile_typenames(transpilation_state_with_indent state, const std::vector<NodeStructs::Typename>& args) {
	auto vec = vec_of_expected_to_expected_of_vec(args | LIFT_TRANSFORM_X(T, transpile_typename(state, T)) | to_vec());
	return_if_error(vec);
	return std::accumulate(
		std::next(vec.value().begin()),
		vec.value().end(),
		vec.value()[0],
		[](const std::string& a, const std::string& b){ return a + ", " + b; }
	);
}

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

bool is_assignable_to(
	transpilation_state_with_indent state,
	const NodeStructs::MetaType& parameter,
	const NodeStructs::MetaType& argument
) {
	return std::visit(
		overload(
			[&](const auto& e, const auto& u) {
				throw;
				return false;
			},
			[&](const NodeStructs::VectorType& e, const NodeStructs::VectorType& u) {
				return e.value_type <=> u.value_type == std::weak_ordering::equivalent;
			},
			[&](const NodeStructs::InterfaceType& e, std::reference_wrapper<const NodeStructs::Type> u) -> bool {
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
				auto& interfacemembers = state.state.interface_symbol_to_members[
					NodeStructs::Typename{ NodeStructs::BaseTypename{ e.interface.get().name} }
				];
				auto new_member = NodeStructs::MetaType{ u };
				for (const auto& member : interfacemembers)
					if (member <=> new_member == std::weak_ordering::equivalent)
						return true;
				interfacemembers.push_back(std::move(new_member));
				return true;
			},
			[&](std::reference_wrapper<const NodeStructs::Type> e, std::reference_wrapper<const NodeStructs::Type> u) {
				return e.get() <=> u.get() == std::weak_ordering::equivalent;
			}
		),
		parameter.type, argument.type
	);
}

transpile_t expr_to_printable(transpilation_state_with_indent state, const NodeStructs::Expression& expr) {
	auto t = transpile_expression(state, expr);
	return_if_error(t);
	if (!std::holds_alternative<non_type_information>(t.value()))
		throw;
	const auto& t_ok = std::get<non_type_information>(t.value());

	std::stringstream ss;
	if (std::holds_alternative<std::reference_wrapper<const NodeStructs::Type>>(t_ok.type.type.type)) {
		const auto& t_ref = std::get<std::reference_wrapper<const NodeStructs::Type>>(t_ok.type.type.type).get();
		if (t_ref.name == "String") {
			auto expr_repr = transpile_expression(state, expr);
			return_if_error(expr_repr);
			if (!std::holds_alternative<non_type_information>(expr_repr.value()))
				throw;
			const auto& expr_repr_ok = std::get<non_type_information>(expr_repr.value());
			ss << "String(\"\\\"\") + " << expr_repr_ok.representation << " + String(\"\\\"\")";
			return ss.str();
		}
		else {
			ss << t_ref.name << "{";
			for (const auto& [member_typename, member_name] : t_ref.memberVariables) {
				auto typename_t = type_of_typename(state, member_typename);
				return_if_error(typename_t);
				auto member = NodeStructs::PropertyAccessExpression{ .operand = expr, .property_name = member_name };
				auto member_repr = expr_to_printable(state, { member });
				return_if_error(member_repr);
				ss << member_name << " = \" << " << member_repr.value();
			}
		}
	}
	else if (std::holds_alternative<NodeStructs::InterfaceType>(t_ok.type.type.type)) {
		const auto& t_ref = std::get<NodeStructs::InterfaceType>(t_ok.type.type.type).interface.get();
		std::stringstream ss;
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

bool uses_auto(const NodeStructs::Function& fn) {
	if (fn.returnType <=> NodeStructs::Typename{ NodeStructs::BaseTypename{ "auto" } } == std::weak_ordering::equivalent)
		return true;
	for (const auto& param : fn.parameters)
		if (uses_auto(param))
			return true;
	for (const auto& statement : fn.statements)
		if (uses_auto(statement))
			return true;
	return false;
}

bool uses_auto(const NodeStructs::FunctionParameter& param) {
	return uses_auto(param.typename_);
}

bool uses_auto(const NodeStructs::Statement& statement) {
	if (std::holds_alternative<NodeStructs::VariableDeclarationStatement>(statement.statement)) {
		const auto& var_decl = std::get<NodeStructs::VariableDeclarationStatement>(statement.statement);
		return uses_auto(var_decl.type);
	}
	else
		return false;
}

static bool uses_auto(const NodeStructs::BaseTypename& t) {
	return t <=> NodeStructs::BaseTypename{ "auto" } == std::weak_ordering::equivalent;
}

static bool uses_auto(const NodeStructs::NamespacedTypename& t) {
	if (t.name_in_name_space == "auto")
		throw;
	return uses_auto(t.name_space.get());
}

static bool uses_auto(const NodeStructs::TemplatedTypename& t) {
	if (uses_auto(t.type.get()))
		return true;
	for (const auto& param : t.templated_with)
		if (uses_auto(param))
			return true;
	return false;
}

static bool uses_auto(const NodeStructs::UnionTypename& t) {
	for (const auto& param : t.ors)
		if (uses_auto(param))
			return true;
	return false;
}

bool uses_auto(const NodeStructs::Typename& t) {
	return std::visit([](const auto& t_) { return uses_auto(t_); }, t.value);
}

expected<NodeStructs::Function> realise_function_using_auto(
	transpilation_state_with_indent state,
	const NodeStructs::Function& fn_using_auto,
	const std::vector<NodeStructs::FunctionArgument>& args
) {
	if (fn_using_auto.parameters.size() != args.size())
		throw;

	NodeStructs::Function realised = fn_using_auto;

	bool uses_auto_param = false;

	for (const auto& [index, param] : enumerate(fn_using_auto.parameters))
		if (uses_auto(param)) {
			uses_auto_param = true;
			if (param.typename_ <=> NodeStructs::Typename{ NodeStructs::BaseTypename{ "auto" } } == std::weak_ordering::equivalent) {
				auto info = transpile_expression(state, args.at(index).expr);
				return_if_error(info);
				if (!std::holds_alternative<non_type_information>(info.value()))
					throw;
				const auto& info_ok = std::get<non_type_information>(info.value());
				auto tn = typename_of_type(state, info_ok.type.type);
				return_if_error(tn);
				auto& p = realised.parameters.at(index);
				auto name = p.name;
				auto cat = p.category;
				auto temp = NodeStructs::FunctionParameter{ .typename_ = tn.value(), .category = cat, .name = name };
				std::swap(p, temp);
			}
			else
				throw;
		}
	if (uses_auto_param)
		return realise_function_using_auto(state, realised, args);

	for (auto&& statement : fn_using_auto.statements)
		if (uses_auto(statement))
			throw;

	bool at_least_one = false;
	bool only_one = false;
	for (const auto& statement : fn_using_auto.statements)
		if (std::holds_alternative<NodeStructs::ReturnStatement>(statement.statement)) {
			only_one = at_least_one ? false : true;
			at_least_one = true;
		}
	if (at_least_one == false) {
		return NodeStructs::Function{
			.name = fn_using_auto.name,
			.returnType = NodeStructs::Typename{ NodeStructs::BaseTypename{ "Void" } },
			.parameters = fn_using_auto.parameters,
			.statements = fn_using_auto.statements
		};
	}
	else if (only_one == false)
		throw;

	for (auto&& [type_name, value_cat, name] : fn_using_auto.parameters)
		if (std::holds_alternative<NodeStructs::Value>(value_cat)) {
			auto e_t = type_of_typename(state, type_name);
			return_if_error(e_t);
			state.state.variables[name].push_back({ NodeStructs::MutableReference{}, e_t.value() });
		}
		else {
			auto e_t = type_of_typename(state, type_name);
			return_if_error(e_t);
			state.state.variables[name].push_back({ value_cat, e_t.value() });
		}

	for (int i = 0; i < fn_using_auto.statements.size(); ++i) {
		const auto& statement = fn_using_auto.statements.at(i);
		if (std::holds_alternative<NodeStructs::ReturnStatement>(statement.statement)) {
			const auto& return_expr = std::get<NodeStructs::ReturnStatement>(statement.statement).returnExpr;
			if (return_expr.size() != 1)
				throw;

			auto deduced_t = transpile_expression(state, return_expr.at(0).expr);

			if (deduced_t.has_error()) {
				for (auto&& [type_name, value_cat, name] : fn_using_auto.parameters)
					state.state.variables[name].pop_back();
				for (int j = 0; j < fn_using_auto.statements.size(); ++j)
					remove_added_variables(state, fn_using_auto.statements.at(i));
			}
			return_if_error(deduced_t);
			if (!std::holds_alternative<non_type_information>(deduced_t.value()))
				throw;
			const auto& deduced_t_ok = std::get<non_type_information>(deduced_t.value());

			auto return_tn = typename_of_type(state, deduced_t_ok.type.type);

			for (auto&& [type_name, value_cat, name] : fn_using_auto.parameters)
				state.state.variables[name].pop_back();
			for (int j = 0; j < fn_using_auto.statements.size(); ++j)
				remove_added_variables(state, fn_using_auto.statements.at(i));

			return_if_error(return_tn);

			return NodeStructs::Function{
				.name = fn_using_auto.name,
				.returnType = return_tn.value(),
				.parameters = fn_using_auto.parameters,
				.statements = fn_using_auto.statements
			};
		}
	}
	throw;
}

NodeStructs::Typename typename_of_primitive(const NodeStructs::PrimitiveType& primitive_t) {
	return std::visit(overload(overload_default_error,
		[&](const std::string&) {
			return NodeStructs::Typename{ NodeStructs::BaseTypename{ "String" } };
		},
		[&](const double&) {
			return NodeStructs::Typename{ NodeStructs::BaseTypename{ "Floating" } };
		},
		[&](const int&) {
			return NodeStructs::Typename{ NodeStructs::BaseTypename{ "Int" } };
		},
		[&](const bool&) {
			return NodeStructs::Typename{ NodeStructs::BaseTypename{ "Bool" } };
		}
	), primitive_t.value);
}
