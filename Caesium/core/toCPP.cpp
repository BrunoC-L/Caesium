#include <numeric>

#include "../utility/overload.hpp"
#include "../utility/vec_of_expected_to_expected_of_vec.hpp"
#include "../utility/vec_of_variant_to_optional_vector_single_type.hpp"

#include "toCPP.hpp"
#include "builtins.hpp"
#include "default_includes.hpp"

void add_impl(auto& v, auto&& e) {
	if (std::find_if(v.begin(), v.end(), [&](const auto& x) { return cmp(x, e) == std::weak_ordering::equivalent; }) != v.end())
		return;
	v.push_back(std::move(e));
}

void add(Namespace& name_space, NodeStructs::Template&& e) {
	return add_impl(name_space.templates[e.name], std::move(e));
}

void add(Namespace& name_space, NodeStructs::Builtin&& e) {
	return add_impl(name_space.builtins[e.name], std::move(e));
}

void add(Namespace& name_space, NodeStructs::Type&& e) {
	return add_impl(name_space.types[e.name], std::move(e));
}

void add(Namespace& name_space, NodeStructs::Function&& e) {
	return add_impl(name_space.functions[e.name], std::move(e));
}

void add_auto_fn(Namespace& name_space, NodeStructs::Function&& e) {
	return add_impl(name_space.functions_using_auto[e.name], std::move(e));
}

void add(Namespace& name_space, NodeStructs::Interface&& e) {
	return add_impl(name_space.interfaces[e.name], std::move(e));
}

void add(Namespace& name_space, NodeStructs::Block&& e) {
	return add_impl(name_space.blocks[e.name], std::move(e));
}

void add(Namespace& name_space, NodeStructs::Enum&& e) {
	return add_impl(name_space.enums[e.name], std::move(e));
}

void add(Namespace& name_space, Namespace&& ns) {
	if (name_space.namespaces.contains(ns.name))
		throw;
	name_space.namespaces.emplace(ns.name, std::move(ns));
}

void add_builtins(Namespace& name_space) {
	builtins _builtins;

	add(name_space, std::move(_builtins.builtin_variant));

	add(name_space, std::move(_builtins.builtin_vector));
	add(name_space, std::move(_builtins.builtin_set));
	add(name_space, std::move(_builtins.builtin_map));

	add(name_space, std::move(_builtins.builtin_push));
	add(name_space, std::move(_builtins.builtin_insert));
	add(name_space, std::move(_builtins.builtin_size));

	add(name_space, std::move(_builtins.builtin_print));
	add(name_space, std::move(_builtins.builtin_println));
	add(name_space, std::move(_builtins.builtin_file));
	add(name_space, std::move(_builtins.builtin_directory));

	add(name_space, std::move(_builtins.entries_str));
	add(name_space, std::move(_builtins.entries_dir));

	add(name_space, std::move(_builtins.filesystem_ns));
}

static void traverse_builtins(transpilation_state& state) {
	builtins _builtins;

	state.traversed_functions.insert(std::move(_builtins.entries_str));
	state.traversed_functions.insert(std::move(_builtins.entries_dir));
}

std::optional<error> insert_all_named_recursive_with_imports(
	Namespace& named,
	const NodeStructs::NameSpace& to_insert
) {
	for (const auto& e : to_insert.types)
		add(named, copy(e));

	for (const auto& e : to_insert.functions)
		add(named, copy(e));

	for (const auto& e : to_insert.functions_using_auto)
		add_auto_fn(named, copy(e));

	for (const auto& e : to_insert.interfaces)
		add(named, copy(e));

	for (const auto& e : to_insert.blocks)
		add(named, copy(e));

	for (const auto& e : to_insert.templates)
		add(named, copy(e));

	for (const auto& e : to_insert.namespaces)
		insert_all_named_recursive_with_imports(named.namespaces.at(e.name), e);

	for (const auto& e : to_insert.enums)
		add(named, copy(e));

	return std::nullopt;
}

template <typename T>
auto add(Namespace& named, std::map<std::string, std::vector<T>>& map_of_vec) {
	for (const auto& [key, vec] : map_of_vec)
		for (const T& e : vec)
			add(named, copy(e));
}

auto add_auto_fns(Namespace& named, std::map<std::string, std::vector<NodeStructs::Function>>& map_of_vec) {
	for (const auto& [key, vec] : map_of_vec)
		for (const NodeStructs::Function& e : vec)
			add_auto_fn(named, copy(e));
}

std::optional<error> insert_all_named_recursive_with_imports(
	const std::vector<NodeStructs::File>& project,
	std::map<std::string, Namespace>& named_by_file,
	const std::string& filename,
	std::set<std::string>& inserted
) {
	if (inserted.contains(filename))
		return std::nullopt;
	inserted.insert(filename);
	for (const NodeStructs::File& file : project)
		if (file.content.name == filename) {
			Namespace& named = named_by_file[filename];
			auto x = insert_all_named_recursive_with_imports(named, file.content);
			if (x.has_value())
				return x.value();

			for (const auto& i : file.imports) {
				auto opt_error = insert_all_named_recursive_with_imports(project, named_by_file, i.imported , inserted);
				if (opt_error.has_value())
					return opt_error;
				Namespace& imported_named = named_by_file.at(i.imported);

				add(named, imported_named.types);
				add(named, imported_named.functions);
				add_auto_fns(named, imported_named.functions_using_auto);
				add(named, imported_named.interfaces);
				add(named, imported_named.blocks);
				add(named, imported_named.templates);
				add(named, imported_named.enums);
				for (const auto& [k, e] : imported_named.namespaces)
					named.namespaces.emplace(k, copy(e));
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
	std::map<std::string, Namespace>& named_by_file,
	const std::string& filename,
	std::set<std::string>& inserted
) {
	if (inserted.contains(filename))
		return std::nullopt;
	inserted.insert(filename);
	for (const NodeStructs::File& file : project)
		if (file.content.name == filename) {
			Namespace& named = named_by_file.at(filename);

			auto state = transpilation_state{ copy(named) };
			for (const auto& alias : file.content.aliases)
				named.aliases.emplace(alias.aliasFrom, copy(alias.aliasTo));
			for (const auto& i : file.imports) {
				auto opt_e = insert_aliases_recursive_with_imports(project, named_by_file, i.imported, inserted);
				if (opt_e.has_value())
					return opt_e;
				Namespace& imported_named = named_by_file[i.imported];
				for (const auto& alias : imported_named.aliases)
					named.aliases.emplace(alias.first, copy(alias.second));
			}

			return std::nullopt;
		}
	return error{
		"user error",
		"invalid import `" + filename + "`"
	};
}

transpile_t transpile(const std::vector<NodeStructs::File>& project) {
	for (const auto& file : project)
		for (const auto& fn : file.content.functions)
			if (fn.name == "main") {
				std::map<std::string, Namespace> named_by_file;

				std::set<std::string> inserted_named = {};
				for (const auto& file2 : project) {
					Namespace named_of_file = {};
					add_builtins(named_of_file);
					named_by_file.emplace(file2.content.name, std::move(named_of_file));
					if (auto opt_error = insert_all_named_recursive_with_imports(project, named_by_file, file2.content.name, inserted_named); opt_error.has_value())
						return opt_error.value();
				}

				std::set<std::string> inserted_aliases = {};
				for (const auto& file2 : project)
					if (auto opt_e = insert_aliases_recursive_with_imports(project, named_by_file, file2.content.name, inserted_aliases); opt_e.has_value())
						return opt_e.value();

				for (const auto& [_, named] : named_by_file)
					for (const auto& [_, templates] : named.templates)
					if (auto opt_e = validate_templates(templates); opt_e.has_value())
						return opt_e.value();

				transpilation_state state{ copy(named_by_file.at(file.content.name)) };

				traverse_builtins(state);

				transpile_header_cpp_t k = transpile_main({ state, 0 }, fn);
				return_if_error(k);

				std::stringstream cpp;
				cpp <<  "#include \"defaults.hpp\"\n"
						"\n";

				{
					std::stringstream declarations;
					std::stringstream definitions;
					for (const auto& t : state.types_to_transpile) {
						if (!state.traversed_types.contains(t))
							throw;
						auto res = transpile({ state, 0 }, t);
						return_if_error(res);
						declarations << res.value().first;
						definitions << res.value().second;
					}
					for (const auto& enum_ : state.enums_to_transpile) {
						size_t val = 1;
						std::string enum_prefix = enum_.name + "__";
						for (const auto& enum_val : enum_.values)
							declarations << "static constexpr Int " << enum_prefix << enum_val << " = " << val++ << ";\n";
					}
					for (const auto& i : state.interfaces_to_transpile) {
						if (!state.traversed_interfaces.contains(i))
							throw;
						if (i.name_space.has_value())
							throw;
						const auto& members = state.interface_symbol_to_members.at({NodeStructs::BaseTypename{ i.name }});
						if (members.size() == 0)
							throw;
						auto unindented = transpilation_state_with_indent{ state, 0 };
						/*auto interface_repr = transpile_typename(unindented, NodeStructs::Typename{ NodeStructs::BaseTypename{ i.name } });
						return_if_error(interface_repr);*/
						auto k = members
							| std::views::transform([&](auto&& T) { return typename_of_type(unindented, T); })
							| to_vec();
						auto v = vec_of_expected_to_expected_of_vec(std::move(k));
						return_if_error(v);
						auto members_repr = transpile_typenames(unindented, v.value());
						return_if_error(members_repr);
						declarations << "struct " << i.name << ";\n";
						definitions << "struct " << i.name << " {\n\tVariant<" << members_repr.value() << "> value;\n};\n";
					}
					cpp << declarations.str() << "\n"
						<< definitions.str() << "\n";
				}
				{
					std::stringstream declarations;
					std::stringstream definitions;
					size_t size = state.functions_to_transpile.size();
					std::vector<NodeStructs::Function> v;
					for (const auto& f : state.functions_to_transpile)
						v.push_back(copy(f));
					for (const auto& f : v) {
						if (!state.traversed_functions.contains(f))
							throw;
						if (uses_auto(f))
							throw;
						auto res = transpile({ state, 0 }, f);
						return_if_error(res);
						declarations << res.value().first;
						definitions << res.value().second;
					}
					if (size != state.functions_to_transpile.size()) {
						puts("size != state.functions_to_transpile.size()\n");
					}
					declarations << k.value().first;
					definitions << k.value().second;
					cpp << declarations.str() << "\n"
						<< definitions.str() << "\n";
				}

				cpp << 
					"int main(int argc, char** argv) {\n"
					"\tstd::vector<std::string> args {};\n"
					"\tfor (int i = 0; i < argc; ++i)\n"
					"\t\targs.push_back(std::string(argv[i]));\n"
					"\treturn _redirect_main(args);\n"
					"};\n";

				return cpp.str();
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
		as_vec(NodeStructs::Typename{ NodeStructs::BaseTypename{ "String" } })
	} };
	if (type <=> vector_str != std::weak_ordering::equivalent)
		return error{ "user error","\"main\" function using 1 argument must be of `Vector<String> ref` type" };

	return transpile(
		state,
		NodeStructs::Function{
			.name = "_redirect_main",
			.name_space = std::nullopt,
			.returnType = copy(fn.returnType),
			.parameters = copy(fn.parameters),
			.statements = copy(fn.statements)
		}
	);
}

std::optional<error> stack(
	transpilation_state_with_indent state,
	variables_t& variables,
	const std::vector<NodeStructs::FunctionParameter>& parameters
) {
	for (auto&& [type_name, value_cat, name] : parameters) {
		auto t = type_of_typename(state, type_name);
		return_if_error(t);
	}
	for (auto&& [type_name, value_cat, name] : parameters) {
		auto t = type_of_typename(state, type_name);
		if (t.has_error())
			throw;
		if (std::holds_alternative<NodeStructs::Value>(value_cat._value))
			variables[name].push_back({ NodeStructs::MutableReference{}, std::move(t).value() });
		else
			variables[name].push_back({ copy(value_cat), std::move(t).value() });
	}
	return std::nullopt;
}

transpile_header_cpp_t transpile(
	transpilation_state_with_indent state,
	const NodeStructs::Function& fn
) {
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
	auto return_typename = transpile_typename(state, fn.returnType);
	return_if_error(return_typename);
	auto return_type = type_of_typename(state, fn.returnType);
	return_if_error(return_type);
	auto parameters = transpile(state, fn.parameters);
	return_if_error(parameters);
	auto stack_params_opt_error = stack(state, variables, fn.parameters);
	if (stack_params_opt_error.has_value())
		return stack_params_opt_error.value();
	auto statements = transpile(state.indented(), variables, fn.statements, return_type.value());
	return_if_error(statements);
	if (fn.name_space.has_value()) {
		auto common = return_typename.value() + " " + transpile_typename(state, fn.name_space.value()).value() + "__" + fn.name + "(" + parameters.value() + ")";
		auto h = common + ";\n";
		auto cpp = common + " {\n" + statements.value() + "};\n";
		return std::pair{ h, cpp };
	}
	else {
		auto common = return_typename.value() + " " + fn.name + "(" + parameters.value() + ")";
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

	if (type.aliases.size() != 0)
		throw;

	for (const auto& member : type.member_variables) {
		auto type = type_of_typename(state, member.type);
		return_if_error(type);
		auto transpiled = transpile_typename(state, member.type);
		return_if_error(transpiled);
		if (transpiled.value() == "TOKENS")
			throw;
		cpp << transpiled.value() << " " << member.name << ";\n";
	}

	cpp << "};\n\n";
	return std::pair{ h.str(), cpp.str()};
}

transpile_header_cpp_t transpile(
	transpilation_state_with_indent state,
	const NodeStructs::Interface& interface
) {
	return std::pair{ "//todo const NodeStructs::Interface& interface;", "//todo const NodeStructs::Interface& interface;"};
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
		if (std::holds_alternative<NodeStructs::Reference>(cat._value))
			s = "const " + std::move(s) + "&";
		else if (std::holds_alternative<NodeStructs::MutableReference>(cat._value))
			s = std::move(s) + "&";
		else if (std::holds_alternative<NodeStructs::Value>(cat._value))
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
	variables_t& variables,
	const std::vector<NodeStructs::Statement>& statements,
	const NodeStructs::MetaType& expected_return_type
) {
	std::stringstream ss;
	for (const auto& statement : statements) {
		auto k = transpile_statement(state, variables, statement, expected_return_type);
		if (k.has_value())
			ss << indent(state.indent) << k.value();
		else
			return k.error();
	}

	return ss.str();
}

NodeStructs::MetaType iterator_type(
	transpilation_state_with_indent state,
	const NodeStructs::MetaType& type
) {
	return std::visit(overload(
		[&](const auto& x) -> NodeStructs::MetaType {
			throw;
		},
		[&](const NodeStructs::VectorType& vt) -> NodeStructs::MetaType {
			return copy(vt.value_type.get());
		}
	), type.type._value);
}

std::vector<NodeStructs::MetaType> decompose_type(
	transpilation_state_with_indent state,
	const NodeStructs::MetaType& type
) {
	throw;
}

//std::optional<error> add_decomposed_for_iterator_variables(
//	transpilation_state_with_indent state,
//	const std::vector<Variant<NodeStructs::VariableDeclaration, std::string>>& iterators,
//	const NodeStructs::MetaType& it_type
//) {
//	if (iterators.size() == 0)
//		return error{ "user error","Expected at least 1 iterator" };
//	auto decomposed_type = decompose_type(state, it_type);
//	if (iterators.size() != decomposed_type.size())
//		throw std::runtime_error("");
//	for (int i = 0; i < iterators.size(); ++i) {
//		const auto& iterator = iterators.at(i);
//		auto opt_error = std::visit(
//			overload(overload_default_error,
//				[&](const NodeStructs::VariableDeclaration& it) -> std::optional<error> {
//					NodeStructs::MetaType iterator_type = type_of_typename(state, it.type).value();
//					if (decomposed_type.at(i) <=> iterator_type != std::weak_ordering::equivalent) {
//						auto t = transpile_typename(state, it.type);
//						return_if_error(t);
//						return error{ "user error","Invalid type of iterator " + t.value() };
//					}
//					throw;
//					//state.state.variables[it.name].push_back({ NodeStructs::Reference{}/*, it.type*/, type_of_typename(state, it.type).value() });
//					return std::nullopt;
//				},
//				[&](const std::string& it) -> std::optional<error> {
//					throw;
//					//state.state.variables[it].push_back({ NodeStructs::Reference{}, decomposed_type.at(i) });
//					return std::nullopt;
//				}
//			),
//			iterator._value
//		);
//		if (opt_error.has_value()) {
//			for (int j = 0; j < i; ++j) {
//				std::visit(
//					overload(overload_default_error,
//						[&](const NodeStructs::VariableDeclaration& it) {
//							state.state.variables[it.name].pop_back();
//						},
//						[&](const std::string& it) {
//							state.state.variables[it].pop_back();
//						}
//					),
//					iterator._value
//				);
//			}
//			return opt_error;
//		}
//	}
//	return std::nullopt;
//}

std::optional<error> add_for_iterator_variables(
	transpilation_state_with_indent state,
	variables_t& variables,
	const std::vector<Variant<NodeStructs::VariableDeclaration, std::string>>& iterators,
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
				variables[it].push_back({ NodeStructs::Reference{}, copy(it_type) });
				return std::nullopt;
			}
		),
		iterators.at(0)._value
	);
}

transpile_t transpile_arg(
	transpilation_state_with_indent state,
	variables_t& variables,
	const NodeStructs::FunctionArgument& arg
) {
	auto expr_info = transpile_expression(state, variables, arg.expr);
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
		), expr_info_ok.value_category._value);
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
		), expr_info_ok.value_category._value, arg.category.value()._value);
	}
}

transpile_t transpile_args(
	transpilation_state_with_indent state,
	variables_t& variables,
	const std::vector<NodeStructs::FunctionArgument>& args
) {
	std::stringstream ss;
	bool first = true;
	for (const auto& arg : args) {
		if (first)
			first = false;
		else
			ss << ", ";

		auto repr = transpile_arg(state, variables, arg);
		return_if_error(repr);
		ss << repr.value();
	}
	return ss.str();
}

transpile_t transpile_expressions(transpilation_state_with_indent state, variables_t& variables, const std::vector<NodeStructs::Expression>& args) {
	std::stringstream ss;
	bool first = true;
	for (const auto& arg : args) {
		if (first)
			first = false;
		else
			ss << ", ";

		auto repr = transpile_expression(state, variables, arg);
		return_if_error(repr);
		if (!std::holds_alternative<non_type_information>(repr.value()))
			throw;
		const auto& repr_ok = std::get<non_type_information>(repr.value());
		ss << repr_ok.representation;
	}
	return ss.str();
}

transpile_t transpile_typenames(transpilation_state_with_indent state, const std::vector<NodeStructs::Typename>& args) {
	if (args.size() == 0)
		return "";
	auto vec = vec_of_expected_to_expected_of_vec(args
		| std::views::transform([&](auto&& T) { return transpile_typename(state, T); })
		| to_vec());
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
		ss << "__" << arg;
	return ss.str();
}

std::string template_name(std::string original_name, const std::vector<NodeStructs::Expression>& arguments) {
	return template_name(original_name, arguments
		| std::views::transform([&](auto&& e) { return expression_for_template(e); })
		| to_vec());
}

bool is_assignable_to(
	transpilation_state_with_indent state_,
	const NodeStructs::MetaType& parameter,
	const NodeStructs::MetaType& argument
) {
	return std::visit(
		overload(
			[&](const auto& e, const auto& u, const auto& state) -> bool {
				throw;
			},
			[&](const NodeStructs::Type& e, const NodeStructs::AggregateType& u, const auto& state) -> bool {
				if (e.member_variables.size() != u.arguments.size())
					return false;
				for (size_t i = 0; i < e.member_variables.size(); ++i) {
					auto mem_t = type_of_typename(state, e.member_variables.at(i).type);
					if (mem_t.has_error())
						throw;
					if (!is_assignable_to(state, mem_t.value(), u.arguments.at(i).second))
						return false;
				}
				return true;
			},
			[&](const NodeStructs::PrimitiveType& e, const NodeStructs::PrimitiveType& u, const auto& state) -> bool {
				return e.value._value.index() == u.value._value.index();
			},
			[&](const NodeStructs::EnumType& e, const NodeStructs::EnumValueType& u, const auto& state) -> bool {
				return e.enum_.get() <=> u.enum_.get() == std::weak_ordering::equivalent;
			},
			[&](const NodeStructs::Builtin& e, const NodeStructs::Builtin& u, const auto& state) -> bool {
				return e.name == u.name;
			},
			[&](const NodeStructs::Builtin& e, const auto& u, const auto& state) -> bool {
				if (e.name == "builtin_filesystem_directory") {
					return is_assignable_to(state, NodeStructs::MetaType{ NodeStructs::PrimitiveType{ { std::string{} } } }, { copy(u) });
				}
				throw;
			},
			[&](const NodeStructs::UnionType& e, const auto& u, const auto& state) {
				if constexpr (std::is_same_v<std::remove_cvref_t<decltype(u)>, NodeStructs::UnionType>) {
					for (const auto& arg : u.arguments) {
						bool ok = false;
						for (const auto& param : e.arguments) {
							if ((arg <=> param == std::weak_ordering::equivalent)
								|| std::visit([&state](const auto& _param, const auto& _arg) -> bool {
									return is_assignable_to(state, { copy(_param) }, { copy(_arg) });
								}, param.type._value, arg.type._value)) {
								ok = true;
								break;
							}
						}
						if (!ok)
							return false;
					}
					return true;
				}
				else {
					for (const auto& vt : e.arguments)
						if (std::visit([&](const auto& t) -> bool { return is_assignable_to(state, { copy(t) }, { copy(u) }); }, vt.type._value))
							return true;
					return false;
				}
			},
			[&](const NodeStructs::VectorType& e, const NodeStructs::VectorType& u, const auto& state) {
				return e.value_type <=> u.value_type == std::weak_ordering::equivalent;
			},
			[&](const NodeStructs::SetType& e, const NodeStructs::SetType& u, const auto& state) {
				return e.value_type <=> u.value_type == std::weak_ordering::equivalent;
			},
			[&](const NodeStructs::InterfaceType& e, const NodeStructs::Type& u, const auto& state) -> bool {
				for (const auto& [type_name, name] : e.interface.get().member_variables) {
					auto t = type_of_typename(state, type_name);
					if (t.has_error())
						throw;
					for (const auto& [source_type_name, source_name] : u.member_variables)
						if (name == source_name) {
							auto t2 = type_of_typename(state, source_type_name);
							if (t2.has_error())
								throw;
							if (t.value() <=> t2.value() != std::weak_ordering::equivalent)
								return false;
						}
				}
				auto& interfacemembers = state.state.interface_symbol_to_members[
					NodeStructs::Typename{ NodeStructs::BaseTypename{ e.interface.get().name } }
				];
				auto new_member = NodeStructs::MetaType{ copy(u) };
				for (const auto& member : interfacemembers)
					if (member <=> new_member == std::weak_ordering::equivalent)
						return true;
				interfacemembers.push_back(std::move(new_member));
				return true;
			},
			[&](const NodeStructs::Type& e, const NodeStructs::Type& u, const auto& state) {
				if (e <=> u == std::weak_ordering::equivalent)
					return true;
				if (e.member_variables.size() == 1) {
					auto member_variable_t = type_of_typename(state, e.member_variables.at(0).type);
					if (member_variable_t.has_error())
						throw;
					return std::visit([&state, &u](const auto& _param) -> bool {
						return is_assignable_to(state, { copy(_param) }, { copy(u) });
					}, member_variable_t.value().type._value);
				}
				return false;
			}
		),
		parameter.type._value, argument.type._value, std::variant<transpilation_state_with_indent>{state_}
	);
}

transpile_t expr_to_printable(transpilation_state_with_indent state, variables_t& variables, const NodeStructs::Expression& expr) {
	auto t = transpile_expression(state, variables, expr);
	return_if_error(t);
	if (!std::holds_alternative<non_type_information>(t.value()))
		throw;
	const auto& t_ok = std::get<non_type_information>(t.value());

	std::stringstream ss;
	if (std::holds_alternative<NodeStructs::Type>(t_ok.type.type.type._value)) {
		const auto& t_ref = std::get<NodeStructs::Type>(t_ok.type.type.type._value);
		ss << t_ref.name << "{";
		for (const auto& [member_typename, member_name] : t_ref.member_variables) {
			auto typename_t = type_of_typename(state, member_typename);
			return_if_error(typename_t);
			auto member = NodeStructs::PropertyAccessExpression{ .operand = copy(expr), .property_name = member_name };
			auto member_repr = expr_to_printable(state, variables, { std::move(member) });
			return_if_error(member_repr);
			ss << member_name << " = \" << " << member_repr.value();
		}
		return ss.str();
	}
	else if (std::holds_alternative<NodeStructs::InterfaceType>(t_ok.type.type.type._value)) {
		const auto& t_ref = std::get<NodeStructs::InterfaceType>(t_ok.type.type.type._value).interface.get();
		std::stringstream ss;
		ss << "\"" << t_ref.name << "{";
		for (const auto& [member_typename, member_name] : t_ref.member_variables) {
			auto typename_t = type_of_typename(state, member_typename);
			return_if_error(typename_t);
			auto member = NodeStructs::PropertyAccessExpression{ .operand = copy(expr), .property_name = member_name };
			auto member_repr = expr_to_printable(state, variables, { std::move(member) });
			return_if_error(member_repr);
			ss << member_name << " = \" << " << member_repr.value();
		}
		ss << " << \"}\"";
		return ss.str();
	}
	else if (std::holds_alternative<NodeStructs::PrimitiveType>(t_ok.type.type.type._value)) {
		auto expr_repr = transpile_expression(state, variables, expr);
		return_if_error(expr_repr);
		if (!std::holds_alternative<non_type_information>(expr_repr.value()))
			throw;
		const auto& expr_repr_ok = std::get<non_type_information>(expr_repr.value());
		ss << "String(\"\\\"\") + " << expr_repr_ok.representation << " + String(\"\\\"\")";
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
	return false;
}

bool uses_auto(const NodeStructs::FunctionParameter& param) {
	return uses_auto(param.typename_);
}

bool uses_auto(const NodeStructs::Statement& statement) {
	if (std::holds_alternative<NodeStructs::VariableDeclarationStatement>(statement.statement._value)) {
		const auto& var_decl = std::get<NodeStructs::VariableDeclarationStatement>(statement.statement._value);
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
	return std::visit([](const auto& t_) { return uses_auto(t_); }, t.value._value);
}

expected<std::optional<NodeStructs::MetaType>> deduce_return_type(
	transpilation_state_with_indent state,
	variables_t& variables,
	const std::vector<NodeStructs::Statement>& statements
);

expected<std::optional<NodeStructs::MetaType>> deduce_return_type(
	transpilation_state_with_indent state,
	variables_t& variables,
	const NodeStructs::Statement& statement
) {
	return std::visit(
		overload(
			overload_default_error,
			[&](const NodeStructs::ReturnStatement& statement) -> expected<std::optional<NodeStructs::MetaType>> {
				if (statement.ifExpr.has_value())
					throw;
				if (statement.returnExpr.size() != 1)
					throw;
				auto res_info = transpile_expression(state, variables, statement.returnExpr.at(0).expr);
				return_if_error(res_info);
				if (!std::holds_alternative<non_type_information>(res_info.value()))
					throw;
				return std::get<non_type_information>(std::move(res_info).value()).type.type;
			},
			[&](const NodeStructs::Expression& statement) -> expected<std::optional<NodeStructs::MetaType>> {
				return std::nullopt;
			},
			[&](const NodeStructs::VariableDeclarationStatement& statement) -> expected<std::optional<NodeStructs::MetaType>> {
				auto t = transpile_statement(
					state,
					variables,
					statement,
					type_of_typename(state, NodeStructs::Typename{ NodeStructs::BaseTypename{ "Void" } }).value()
				);
				return_if_error(t);
			},
			[&](const NodeStructs::IfStatement& statement) -> expected<std::optional<NodeStructs::MetaType>> {
				auto if_res_t = deduce_return_type(state, variables, statement.ifStatements);
				return_if_error(if_res_t);

				if (!statement.elseExprStatements.has_value())
					return std::move(if_res_t);

				auto else_res_t = std::visit(
					overload(
						[&](const std::vector<NodeStructs::Statement>& statements) -> expected<std::optional<NodeStructs::MetaType>> {
							return deduce_return_type(state, variables, statements);
						},
						[&](const NonCopyableBox<NodeStructs::IfStatement>& inner_if) -> expected<std::optional<NodeStructs::MetaType>> {
							return deduce_return_type(state, variables, NodeStructs::Statement{ copy(inner_if.get()) });
						}
					),
					statement.elseExprStatements.value()._value
				);
				return_if_error(else_res_t);

				if (if_res_t.value().has_value() && else_res_t.value().has_value())
					if (cmp(if_res_t.value().value(), else_res_t.value().value()) != std::weak_ordering::equivalent)
						throw;
					else
						return std::move(if_res_t);
				if (if_res_t.value().has_value())
					return std::move(if_res_t);
				return std::move(else_res_t);
			},
			[&](const NodeStructs::IForStatement& statement) -> expected<std::optional<NodeStructs::MetaType>> {
				throw;
			},
			[&](const NodeStructs::ForStatement& statement) -> expected<std::optional<NodeStructs::MetaType>> {
				auto coll_type_or_e = transpile_expression(state, variables, statement.collection);
				return_if_error(coll_type_or_e);
				if (!std::holds_alternative<non_type_information>(coll_type_or_e.value()))
					throw;
				const auto& coll_type_or_e_ok = std::get<non_type_information>(coll_type_or_e.value());
				auto it_type = iterator_type(state, coll_type_or_e_ok.type.type);
				if (statement.iterators.size() > 1)
					throw;
				auto opt_e = add_for_iterator_variables(state, variables, statement.iterators, it_type);
				if (opt_e.has_value())
					return opt_e.value();
				return deduce_return_type(state, variables, statement.statements);
			},
			[&](const NodeStructs::WhileStatement& statement) -> expected<std::optional<NodeStructs::MetaType>> {
				return deduce_return_type(state, variables, statement.statements);
			},
			[&](const NodeStructs::BreakStatement& statement) -> expected<std::optional<NodeStructs::MetaType>> {
				return std::nullopt;
			},
			[&](const NodeStructs::BlockStatement& statement) -> expected<std::optional<NodeStructs::MetaType>> {
				throw;
			},
			[&](const NodeStructs::MatchStatement& statement) -> expected<std::optional<NodeStructs::MetaType>> {
				// for each match case copy variables, add the match variables, note the return type from the case, compare with stored
				if (statement.expressions.size() != 1)
					throw;
				if (statement.cases.size() == 0)
					throw;
				/*auto expr_info = transpile_expression(state, variables, statement.expressions.at(0));
				return_if_error(expr_info);
				if (!std::holds_alternative<non_type_information>(expr_info.value()))
					throw;
				const auto& expr_ok = std::get<non_type_information>(expr_info.value());
				auto tn = typename_of_type(state, expr_ok.type.type);
				return_if_error(tn);
				auto tn_repr = transpile_typename(state, tn.value());
				return_if_error(tn_repr);*/

				std::optional<NodeStructs::MetaType> res = std::nullopt;
				for (const NodeStructs::MatchCase& match_case : statement.cases) {
					if (match_case.variable_declarations.size() != 1)
						throw;
					auto tn = transpile_typename(state, match_case.variable_declarations.at(0).first);
					return_if_error(tn);
					const auto& varname = match_case.variable_declarations.at(0).second;
					variables[varname]
						.push_back(variable_info{
							.value_category = NodeStructs::Reference{},
							.type = type_of_typename(state, match_case.variable_declarations.at(0).first).value()
							});
					auto deduced = deduce_return_type(state, variables, match_case.statements);
					variables[varname].pop_back();
					return_if_error(deduced);

					if (deduced.value().has_value())
						if (res.has_value())
							if (cmp(res.value(), deduced.value().value()) != std::weak_ordering::equivalent)
								throw;
							else
								continue;
						else
							res.emplace(std::move(deduced).value().value());
				}
				return res;
				// here we would actually know at compile time that this cant be hit so we wouldnt actually insert a throw, it will be removed eventually
			},
			[&](const NodeStructs::SwitchStatement& statement) -> expected<std::optional<NodeStructs::MetaType>> {
				throw;
			},
			[&](const NodeStructs::EqualStatement& statement) -> expected<std::optional<NodeStructs::MetaType>> {
				return std::nullopt;
			}
		),
		statement.statement._value
	);
}

expected<std::optional<NodeStructs::MetaType>> deduce_return_type(
	transpilation_state_with_indent state,
	variables_t& variables,
	const std::vector<NodeStructs::Statement>& statements
) {
	std::optional<NodeStructs::MetaType> res = std::nullopt;
	for (const auto& statement : statements) {
		expected<std::optional<NodeStructs::MetaType>> e = deduce_return_type(state, variables, statement);
		return_if_error(e);

		if (e.value().has_value())
			if (res.has_value())
				if (cmp(res.value(), e.value().value()) != std::weak_ordering::equivalent)
					throw;
				else
					continue;
			else
				res.emplace(std::move(e).value().value());
	}
	return res;
}

expected<NodeStructs::Function> realise_function_using_auto(
	transpilation_state_with_indent state,
	const NodeStructs::Function& fn_using_auto,
	const std::vector<NodeStructs::MetaType>& arg_types
) {
	if (fn_using_auto.parameters.size() != arg_types.size())
		throw;

	NodeStructs::Function realised = copy(fn_using_auto);

	for (const auto& [index, param] : enumerate(fn_using_auto.parameters))
		if (uses_auto(param)) {
			if (param.typename_ <=> NodeStructs::Typename{ NodeStructs::BaseTypename{ "auto" } } == std::weak_ordering::equivalent) {
				auto tn = typename_of_type(state, arg_types.at(index));
				return_if_error(tn);
				auto del = std::move(realised.parameters.at(index).typename_);
				new (&realised.parameters.at(index).typename_) NodeStructs::Typename(std::move(tn).value());
			}
			else
				throw;
		}

	for (auto&& statement : realised.statements)
		if (uses_auto(statement))
			throw;

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

	auto stack_params_opt_error = stack(state, variables, realised.parameters);
	if (stack_params_opt_error.has_value())
		return stack_params_opt_error.value();

	auto return_tn = deduce_return_type(state, variables, realised.statements);
	return_if_error(return_tn);

	if (return_tn.value().has_value())
		return NodeStructs::Function{
			.name = realised.name,
			.name_space = std::nullopt,
			.returnType = typename_of_type(state, std::move(return_tn).value().value()).value(),
			.parameters = std::move(realised.parameters),
			.statements = std::move(realised.statements)
		};
	else
		return NodeStructs::Function{
			.name = realised.name,
			.name_space = std::nullopt,
			.returnType = NodeStructs::Typename{ NodeStructs::BaseTypename{ "Void" } },
			.parameters = std::move(realised.parameters),
			.statements = std::move(realised.statements)
		};
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
		},
		[&](const NodeStructs::void_t&) {
			return NodeStructs::Typename{ NodeStructs::BaseTypename{ "Void" } };
		},
		[&](const char&) {
			return NodeStructs::Typename{ NodeStructs::BaseTypename{ "Char" } };
		}
	), primitive_t.value._value);
}

static bool has_consecutive_variadic_parameters(const NodeStructs::Template& tmpl) {
	for (size_t i = 0; i + 1 < tmpl.parameters.size(); ++i)
		if (std::holds_alternative<NodeStructs::VariadicTemplateParameter>(tmpl.parameters.at(i)._value)
			&& std::holds_alternative<NodeStructs::VariadicTemplateParameter>(tmpl.parameters.at(i + 1)._value))
			return true;
	return false;
}

static bool has_two_variadic_parameters_around_non_specialized_argument(const NodeStructs::Template& tmpl) {
	for (size_t i = 0; i + 2 < tmpl.parameters.size(); ++i)
		if (std::holds_alternative<NodeStructs::VariadicTemplateParameter>(tmpl.parameters.at(i)._value)
			&& std::holds_alternative<NodeStructs::TemplateParameter>(tmpl.parameters.at(i + 1)._value)
			&& std::holds_alternative<NodeStructs::VariadicTemplateParameter>(tmpl.parameters.at(i + 2)._value))
			return true;
	return false;
}

std::optional<error> validate_templates(const std::vector<NodeStructs::Template>& templates) {
	for (const NodeStructs::Template& tmpl : templates) {
		if (has_consecutive_variadic_parameters(tmpl))
			return error{
				"user error",
				"template has consecutive variadic parameters"
		};
		if (has_two_variadic_parameters_around_non_specialized_argument(tmpl))
			return error{
				"user error",
				"template has two variadic parameters around a non specialized argument"
		};
	}
	return std::nullopt;
}

static std::vector<const NodeStructs::Template*> matching_size_candidates(
	size_t n_args,
	const std::vector<NodeStructs::Template>& templates
) {
	std::vector<NodeStructs::Template const*> res;
	for (const auto& tmpl : templates) {
		bool has_variadic = false;
		unsigned n_non_variadic_parameters = 0;
		for (const auto& param_variant : tmpl.parameters) {
			std::visit(overload(
				[&](const NodeStructs::VariadicTemplateParameter& variadic) {
					has_variadic = true;
				},
				[&](const NodeStructs::TemplateParameter& non_variadic) {
					n_non_variadic_parameters += 1;
				},
				[&](const NodeStructs::TemplateParameterWithDefaultValue& non_variadic) {
					n_non_variadic_parameters += 1;
				}
			), param_variant._value);
		}
		if ((has_variadic && n_non_variadic_parameters <= n_args) || (!has_variadic && n_non_variadic_parameters == n_args))
			res.push_back(&tmpl);
	}

	return res;
}

template <typename T>
static std::vector<Arrangement> arrangements(
	const std::vector<T>& args,
	Arrangement current,
	size_t arg_index,
	size_t param_index
) {
	if (arg_index == args.size()) // all args are placed
		return { current };
	else if (param_index == current.tmpl.get().parameters.size()) // not all args are placed but there are no more parameters
		return {};
	else // not all args are placed and there are more parameters
		return std::visit(overload(
			[&](const NodeStructs::VariadicTemplateParameter& variadic) -> std::vector<Arrangement> {
				std::vector<Arrangement> res = arrangements(args, current, arg_index, param_index + 1); // skipping variadic param
				// try to match all args to the variadic, each time aggregating the results
				// do not match and skip, as the next call attempts to skip anyway
				while (arg_index < args.size()) {
					current.arg_placements.push_back(param_index);
					++arg_index;
					auto _arrangements = arrangements(args, current, arg_index, param_index);
					res.insert(res.end(), _arrangements.begin(), _arrangements.end());
				}
				return res;
			},
			[&](const NodeStructs::TemplateParameter& non_variadic) -> std::vector<Arrangement> {
				current.arg_placements.push_back(param_index);
				return arrangements(args, std::move(current), arg_index + 1, param_index + 1);
			},
			[&](const NodeStructs::TemplateParameterWithDefaultValue& non_variadic) -> std::vector<Arrangement> {
				if constexpr (std::is_same_v<T, NodeStructs::Expression>) {
					if (non_variadic.value <=> args.at(arg_index) == std::weak_ordering::equivalent) {
						current.arg_placements.push_back(param_index);
						return arrangements(args, std::move(current), arg_index + 1, param_index + 1);
					}
					else
						return {};
				}
				else {
					if (expression_for_template(non_variadic.value) == typename_for_template(args.at(arg_index))) {
						current.arg_placements.push_back(param_index);
						return arrangements(args, std::move(current), arg_index + 1, param_index + 1);
					}
					else
						return {};
				}
			}
		), current.tmpl.get().parameters.at(param_index)._value);
}

template <typename T>
static std::vector<Arrangement> arrangements(const std::vector<T>& args, const NodeStructs::Template& tmpl) {
	return arrangements(args, Arrangement{ tmpl, {} }, 0, 0);
}

template <typename T>
static std::vector<Arrangement> arrangements(const std::vector<T>& args, const std::vector<const NodeStructs::Template*>& templates) {
	std::vector<Arrangement> res;
	for (const NodeStructs::Template* tmpl : templates) {
		auto _arrangements = arrangements(args, *tmpl);
		res.insert(res.end(), _arrangements.begin(), _arrangements.end());
	}
	return res;
}

bool first_is_preferable(
	const Variant<NodeStructs::TemplateParameter, NodeStructs::TemplateParameterWithDefaultValue, NodeStructs::VariadicTemplateParameter>& first,
	const Variant<NodeStructs::TemplateParameter, NodeStructs::TemplateParameterWithDefaultValue, NodeStructs::VariadicTemplateParameter>& second
) {
	auto score = [](const auto& e) {
		return std::visit(overload(
			[&](const NodeStructs::TemplateParameterWithDefaultValue&) {
				return 2;
			},
			[&](const NodeStructs::TemplateParameter&) {
				return 1;
			},
			[&](const NodeStructs::VariadicTemplateParameter&) {
				return 0;
			}
		), e._value);
	};
	return score(first) > score(second);
}

template <typename T>
bool first_arrangement_has_at_least_one_reason_to_be_preferred_over_second(
	const std::vector<T>& args,
	const Arrangement& first,
	const Arrangement& second
) {
	for (size_t i = 0; i < args.size(); ++i)
		if (first_is_preferable(
			first.tmpl.get().parameters.at(first.arg_placements.at(i)),
			second.tmpl.get().parameters.at(second.arg_placements.at(i))
		))
			return true;
	return false;
}

bool has_empty_variadic(
	const Arrangement& ar
) {
	for (size_t i = 0; i < ar.tmpl.get().parameters.size(); ++i)
		if (std::find(ar.arg_placements.begin(), ar.arg_placements.end(), i) == ar.arg_placements.end())
			return true;
	return false;
}

template <typename T>
bool first_arrangement_preferred_over_second(
	const std::vector<T>& args,
	const Arrangement& first,
	const Arrangement& second
) {
	if (first_arrangement_has_at_least_one_reason_to_be_preferred_over_second(args, second, first))
		return false;
	if (!first_arrangement_has_at_least_one_reason_to_be_preferred_over_second(args, first, second))
		return has_empty_variadic(second) && !has_empty_variadic(first);
}

template <typename T>
bool nth_arrangement_has_reason_to_be_picked_over_all_others(
	const std::vector<T>& args,
	const std::vector<Arrangement>& arrangements,
	size_t i
) {
	for (size_t j = 0; j < arrangements.size(); ++j)
		if (i != j)
			if (!first_arrangement_preferred_over_second(args, arrangements.at(i), arrangements.at(j)))
				return false;
	return true;
}

template <typename T>
expected<Arrangement> _find_best_template(
	const std::vector<NodeStructs::Template>& templates,
	const std::vector<T>& args
) {
	std::vector<const NodeStructs::Template*> candidates = matching_size_candidates(args.size(), templates);
	if (candidates.size() == 0)
		throw;
	std::vector<Arrangement> candidate_arrangements = arrangements(args, candidates);
	
	std::sort(
		candidate_arrangements.begin(),
		candidate_arrangements.end(),
		[](const Arrangement& l, const Arrangement& r) {
			return &l.tmpl.get() < &r.tmpl.get() || &l.tmpl.get() == &r.tmpl.get() && cmp(l.arg_placements, r.arg_placements) == std::weak_ordering::less;
		}
	);
	candidate_arrangements.erase(
		std::unique(
			candidate_arrangements.begin(),
			candidate_arrangements.end(),
			[](const Arrangement& l, const Arrangement& r) {
				return &l.tmpl.get() == &r.tmpl.get() && cmp(l.arg_placements, r.arg_placements) == std::weak_ordering::equivalent;
			}
		),
		candidate_arrangements.end()
	);

	if (candidate_arrangements.size() == 1)
		return std::move(candidate_arrangements.at(0));
	for (size_t i = 0; i < candidate_arrangements.size(); ++i)
		if (nth_arrangement_has_reason_to_be_picked_over_all_others(args, candidate_arrangements, i))
			return std::move(candidate_arrangements.at(i));

	std::stringstream args_ss;
	bool has_prev_args = false;
	for (const auto& arg : args) {
		if (has_prev_args)
			args_ss << ", ";
		has_prev_args = true;
		if constexpr (std::is_same_v<T, NodeStructs::Expression>) {
			args_ss << expression_for_template(arg);
		}
		else {
			args_ss << typename_for_template(arg);
		}
	}
	std::stringstream templates_ss;
	bool has_prev_templates = false;
	auto name = [](const auto& e) { return std::visit([](const auto& x) { return x.name; }, e._value); };
	for (auto const* a : candidates) {
		if (has_prev_templates)
			templates_ss << ", ";
		has_prev_templates = true;
		std::stringstream parameters_ss;
		bool has_prev_parameters = false;
		for (const auto& parameter : a->parameters) {
			if (has_prev_parameters)
				parameters_ss << ", ";
			has_prev_parameters = true;
			parameters_ss << name(parameter);
			/*if (parameter.second.has_value())
				parameters_ss << " = " << expression_for_template(parameter.second.value());*/
		}
		templates_ss << "template " + templates.at(0).name + "<" + parameters_ss.str() + ">";
	}
	return error{
		"user error",
		"More than one instance of `" + templates.at(0).name + "`"
		" matched the provided arguments [" + args_ss.str() + "]"
		", contenders were [" + templates_ss.str() + "]"
	};
}

expected<Arrangement> find_best_template(
	const std::vector<NodeStructs::Template>& templates,
	const std::vector<NodeStructs::Expression>& args
) {
	return _find_best_template(templates, args);
}

expected<Arrangement> find_best_template(
	const std::vector<NodeStructs::Template>& templates,
	const std::vector<NodeStructs::Typename>& args
) {
	return _find_best_template(templates, args);
}

static expected<std::optional<const NodeStructs::Function*>> find_best_function100(
	transpilation_state_with_indent state,
	const std::vector<NodeStructs::MetaType>& arg_types,
	const std::vector<NodeStructs::Function>& fs
) {
	std::optional<const NodeStructs::Function*> res = std::nullopt;

	for (const auto& fn : fs) {
		size_t size = fn.parameters.size();
		if (size != arg_types.size())
			continue;
		bool failed = false;
		for (size_t i = 0; i < size; ++i) {
			auto f_tn = type_of_typename(state, fn.parameters.at(i).typename_);
			return_if_error(f_tn);
			if (!is_assignable_to(state, f_tn.value(), arg_types.at(i)))
				failed = true;
		}
		if (failed)
			continue;
		else
			res.emplace(&fn);
	}

	if (res.has_value())
		return res.value();
	else
		return std::nullopt;
}

static expected<std::optional<const NodeStructs::Function*>> find_best_function010(
	transpilation_state_with_indent state,
	const std::vector<NodeStructs::MetaType>& arg_types,
	const std::vector<NodeStructs::Function>& auto_fs
) {
	std::optional<const NodeStructs::Function*> res = std::nullopt;
	for (const auto& auto_fn : auto_fs) {
		size_t size = auto_fn.parameters.size();
		if (size != arg_types.size())
			continue;
		auto fn = realise_function_using_auto(
			state,
			auto_fn,
			arg_types
		);
		return_if_error(fn);
		if (uses_auto(fn.value()))
			throw;

		auto fn_ok = std::move(fn).value();
		// TODO VERIFY
		if (!state.state.traversed_functions.contains(fn_ok)) {
			state.state.traversed_functions.insert(copy(fn_ok));
			state.state.functions_to_transpile.insert(copy(fn_ok));
		}

		bool failed = false;
		for (size_t i = 0; i < size; ++i) {
			auto f_tn = type_of_typename(state, fn_ok.parameters.at(i).typename_);
			if (f_tn.has_error()) {
				failed = true;
				continue;
			}
			if (!is_assignable_to(state, f_tn.value(), arg_types.at(i)))
				failed = true;
		}
		if (failed)
			continue;
		else {
			auto it = state.state.traversed_functions.find(fn_ok);
			res.emplace(&*it);
		}
	}
	if (res.has_value())
		return res.value();
	else
		return std::nullopt;
}

static expected<std::optional<const NodeStructs::Function*>> find_best_function001(
	transpilation_state_with_indent state,
	const std::vector<NodeStructs::MetaType>& arg_types,
	const std::vector<NodeStructs::Template>& templates
) {
	throw;
}

static expected<std::optional<const NodeStructs::Function*>> find_best_function110(
	transpilation_state_with_indent state,
	const std::vector<NodeStructs::MetaType>& arg_types,
	const std::vector<NodeStructs::Function>& fs,
	const std::vector<NodeStructs::Function>& auto_fs
) {
	auto e_f = find_best_function100(state, arg_types, fs);
	if (e_f.has_error() || e_f.value().has_value())
		return e_f;
	return find_best_function010(state, arg_types, auto_fs);
}

static expected<std::optional<const NodeStructs::Function*>> find_best_function101(
	transpilation_state_with_indent state,
	const std::vector<NodeStructs::MetaType>& arg_types,
	const std::vector<NodeStructs::Function>& fs,
	const std::vector<NodeStructs::Template>& templates
) {
	throw;
}

static expected<std::optional<const NodeStructs::Function*>> find_best_function011(
	transpilation_state_with_indent state,
	const std::vector<NodeStructs::MetaType>& arg_types,
	const std::vector<NodeStructs::Function>& auto_fs,
	const std::vector<NodeStructs::Template>& templates
) {
	throw;
}

static expected<std::optional<const NodeStructs::Function*>> find_best_function111(
	transpilation_state_with_indent state,
	const std::vector<NodeStructs::MetaType>& arg_types,
	const std::vector<NodeStructs::Function>& fs,
	const std::vector<NodeStructs::Function>& auto_fs,
	const std::vector<NodeStructs::Template>& templates
) {
	throw;
}

expected<std::optional<const NodeStructs::Function*>> find_best_function(
	transpilation_state_with_indent state,
	const std::string& name,
	const Namespace& space,
	const std::vector<NodeStructs::MetaType>& arg_types
	) {
	const auto& fs = space.functions.find(name);
	const auto& auto_fs = space.functions_using_auto.find(name);
	const auto& templates = space.templates.find(name);
	if (fs != space.functions.end())
		if (auto_fs != space.functions_using_auto.end())
			if (templates != space.templates.end())
				return find_best_function111(state, arg_types, fs->second, auto_fs->second, templates->second);
			else
				return find_best_function110(state, arg_types, fs->second, auto_fs->second);
		else
			if (templates != space.templates.end())
				return find_best_function101(state, arg_types, fs->second, templates->second);
			else
				return find_best_function100(state, arg_types, fs->second);
	else
		if (auto_fs != space.functions_using_auto.end())
			if (templates != space.templates.end())
				return find_best_function011(state, arg_types, auto_fs->second, templates->second);
			else
				return find_best_function010(state, arg_types, auto_fs->second);
		else
			if (templates != space.templates.end())
				return find_best_function001(state, arg_types, templates->second);
			else
				throw;
}
