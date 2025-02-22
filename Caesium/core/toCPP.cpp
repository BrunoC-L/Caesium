#include <numeric>

#include "../utility/overload.hpp"
#include "../utility/vec_of_expected_to_expected_of_vec.hpp"
#include "../utility/vec_of_variant_to_optional_vector_single_type.hpp"
#include "../utility/replace_all.hpp"

#include "toCPP.hpp"
#include "../structured/structurizer.hpp"
#include "builtins.hpp"
#include "../utility/default_includes.hpp"

void add_impl(auto& v, auto&& e) {
	if (std::find_if(v.begin(), v.end(), [&](const auto& x) { return cmp(x, e) == std::strong_ordering::equivalent; }) != v.end())
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
		NOT_IMPLEMENTED;
	auto cp = ns.name;
	name_space.namespaces.emplace(std::move(cp), std::move(ns));
}

void add_builtins(Namespace& name_space) {
	builtins _builtins;

	add(name_space, std::move(_builtins.builtin_compile_time_error));
	add(name_space, std::move(_builtins.builtin_type_list));


	add(name_space, std::move(_builtins.builtin_union));

	add(name_space, std::move(_builtins.builtin_vector));
	add(name_space, std::move(_builtins.builtin_set));
	add(name_space, std::move(_builtins.builtin_map));

	add(name_space, std::move(_builtins.builtin_push));
	add(name_space, std::move(_builtins.builtin_insert));
	add(name_space, std::move(_builtins.builtin_size));

	add(name_space, std::move(_builtins.builtin_exit));
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

	for (const auto& e : to_insert.namespaces) {
		Namespace ns{ .info = copy(to_insert.info) };
		ns.name = e.name;
		insert_all_named_recursive_with_imports(ns, e);
		named.namespaces.insert({ e.name, std::move(ns) });
	}

	for (const auto& e : to_insert.enums)
		add(named, copy(e));

	return std::nullopt;
}

template <typename T, typename compare>
auto add(Namespace& named, std::map<std::string, std::vector<T>, compare>& map_of_vec) {
	for (const auto& [key, vec] : map_of_vec)
		for (const T& e : vec)
			add(named, ::copy(e));
}

template <typename compare>
auto add_auto_fns(Namespace& named, std::map<std::string, std::vector<NodeStructs::Function>, compare>& map_of_vec) {
	for (const auto& [key, vec] : map_of_vec)
		for (const NodeStructs::Function& e : vec)
			add_auto_fn(named, copy(e));
}

std::optional<error> insert_all_named_recursive_with_imports(
	const std::vector<NodeStructs::File>& project,
	std::map<std::string, Namespace>& named_by_file,
	const std::string& file_name,
	std::set<std::string>& inserted
) {
	if (inserted.contains(file_name))
		return std::nullopt;
	inserted.insert(file_name);
	for (const NodeStructs::File& file : project)
		if (file.content.name == file_name) {
			if (!named_by_file.contains(file_name))
				named_by_file.insert({ file_name, Namespace{ .info = rule_info{ .file_name = "file:/" + file_name, .content = file_name, } } });
			Namespace& named = named_by_file.at(file_name);
			auto x = insert_all_named_recursive_with_imports(named, file.content);
			if (x.has_value())
				return x.value();
			for (const auto& exists : file.exists) {
				auto x = insert_all_named_recursive_with_imports(named, exists.global_exists);
				if (x.has_value())
					return x.value();
			}

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
		std::string("Invalid import \"") + file_name + "\""
	};
}

std::optional<error> insert_aliases_recursive_with_imports(
	const std::vector<NodeStructs::File>& project,
	std::map<std::string, Namespace>& named_by_file,
	const std::string& file_name,
	std::set<std::string>& inserted
) {
	if (inserted.contains(file_name))
		return std::nullopt;
	inserted.insert(file_name);
	for (const NodeStructs::File& file : project)
		if (file.content.name == file_name) {
			Namespace& named = named_by_file.at(file_name);

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
		"invalid import `" + file_name + "`"
	};
}

void mark_exists_as_traversed(transpilation_state& state, variables_t& variables, const NodeStructs::NameSpace& exists, std::stringstream& ss) {
	for (const auto& e : exists.types) {
		if (e.name_space.has_value()) {
			auto str_or_e = transpile_typename({ state }, variables, e.name_space.value());
			if (str_or_e.has_error())
				NOT_IMPLEMENTED;
			ss << "using " << str_or_e.value() << "__" << e.name << " = ";
			std::string aliases_to = replace_all(std::move(str_or_e).value(), "__", "::");
			ss << aliases_to << "::" << e.name << ";\n";
		}
		state.traversed_types.insert(copy(e));
	}

	for (const auto& e : exists.templates) {
		bool is_type = true;
		if (is_type) {
			if (e.name_space.has_value()) {
				NOT_IMPLEMENTED;
				/*auto str_or_e = transpile_typename({ state }, variables, e.name_space.value());
				if (str_or_e.has_error())
					NOT_IMPLEMENTED;
				ss << "template <typename... Ts> using " << str_or_e.value() << "__" << e.name << " = ";
				std::string aliases_to = replace_all(std::move(str_or_e).value(), "__", "::");
				ss << aliases_to << "::" << e.name << "<Ts...>;\n";*/
			}
		}
		else { // assume function
			if (e.name_space.has_value()) {
				NOT_IMPLEMENTED;
				/*auto str_or_e = transpile_typename({ state }, variables, e.name_space.value());
				if (str_or_e.has_error())
					NOT_IMPLEMENTED;
				ss << "decltype(auto) " << str_or_e.value() << "__" << e.name << "(auto&&... args) { return ";
				std::string aliases_to = replace_all(std::move(str_or_e).value(), "__", "::");
				ss << aliases_to << "::" << e.name << "(std::forward<decltype(args)...>(args...)); }\n";*/
			}
		}
	}

	for (const auto& e : exists.functions)
		state.traversed_functions.insert(copy(e));

	for (const auto& e : exists.functions_using_auto) {
		(void)e;
		NOT_IMPLEMENTED;
	}

	for (const auto& e : exists.interfaces)
		state.traversed_interfaces.insert(copy(e));

	for (const auto& e : exists.blocks) {
		(void)e;
		NOT_IMPLEMENTED;
	}

	for (const auto& e : exists.enums) {
		(void)e;
		NOT_IMPLEMENTED;
	}
	
	for (const auto& e : exists.namespaces)
		mark_exists_as_traversed(state, variables, e, ss);
}

void mark_exists_as_traversed(transpilation_state& state, variables_t& variables, const NodeStructs::Exists& exists, std::stringstream& ss) {
	mark_exists_as_traversed(state, variables, exists.global_exists, ss);
}

expected<std::pair<std::map<std::string, Namespace>, std::set<std::string>>> create_named_by_file(const std::vector<NodeStructs::File>& project) {
	std::map<std::string, Namespace> named_by_file{};
	std::set<std::string> inserted_named = {};

	 // at this point we could also check if some files are never used and provide a warning

	for (const auto& file2 : project) {
		Namespace named_of_file = {
			.info = copy(file2.content.info)
		};
		add_builtins(named_of_file);
		named_by_file.emplace(file2.content.name, std::move(named_of_file));
		if (auto opt_error = insert_all_named_recursive_with_imports(project, named_by_file, file2.content.name, inserted_named); opt_error.has_value())
			return opt_error.value();
	}

	return std::pair{ std::move(named_by_file), std::move(inserted_named) };
}

expected<std::pair<std::set<std::string>, std::map<std::string, Namespace>>> insert_aliases(const std::vector<NodeStructs::File>& project, std::map<std::string, Namespace> named_by_file) {
	std::set<std::string> inserted_aliases = {};
	for (const auto& file2 : project)
		if (auto opt_e = insert_aliases_recursive_with_imports(project, named_by_file, file2.content.name, inserted_aliases); opt_e.has_value())
			return opt_e.value();
	return std::pair{ std::move(inserted_aliases), std::move(named_by_file) };
}

transpile_t transpile(const std::vector<NodeStructs::File>& project) {
	for (const auto& file : project)
		for (const auto& fn : file.content.functions)
			if (fn.name == "main") {

				auto named_or_e = create_named_by_file(project);
				return_if_error(named_or_e);
				auto [named_by_file_, inserted_named] = std::move(named_or_e).value();

				auto aliases_or_e = insert_aliases(project, std::move(named_by_file_));
				return_if_error(aliases_or_e);
				auto [inserted_aliases, named_by_file] = std::move(aliases_or_e).value();

				for (const auto& [_, templates] : named_by_file.at(file.content.name).templates)
					if (auto opt_e = validate_templates(templates); opt_e.has_value())
						return opt_e.value();

				transpilation_state state{ copy(named_by_file.at(file.content.name)) };

				traverse_builtins(state);

				std::stringstream exists_aliases;

				variables_t variables{};

				for (const auto& file : project)
					for (const auto& exists : file.exists)
						mark_exists_as_traversed(state, variables, exists, exists_aliases);

				transpile_declaration_definition_t main_transpilation_result = transpile_main({ state, 0 }, fn);
				return_if_error(main_transpilation_result);

				std::stringstream cpp;
				cpp << "#include \"defaults.hpp\"\n\n" << exists_aliases.str();

				{
					std::stringstream declarations;
					std::stringstream definitions;
					for (const auto& t : state.types_to_transpile) {
						if (!state.traversed_types.contains(t))
							NOT_IMPLEMENTED;
						auto res = transpile_type({ state, 0 }, t);
						return_if_error(res);
						declarations << res.value().first;
						definitions << res.value().second;
					}
					for (const auto& enum_ : state.enums_to_transpile) {
						size_t val = 0;
						std::string enum_prefix = enum_.name + "__";
						for (const auto& enum_val : enum_.values)
							declarations << "static constexpr Int " << enum_prefix << enum_val << " = " << val++ << ";\n";
					}
					for (const auto& i : state.interfaces_to_transpile) {
						if (!state.traversed_interfaces.contains(i))
							NOT_IMPLEMENTED;
						if (i.name_space.has_value())
							NOT_IMPLEMENTED;
						const auto& members = state.interface_symbol_to_members.at(make_typename(NodeStructs::BaseTypename{ i.name }, NodeStructs::Value{}, rule_info_stub_no_throw()));
						if (members.size() == 0)
							NOT_IMPLEMENTED;
						auto unindented = transpilation_state_with_indent{ state, 0 };
						/*auto interface_repr = transpile_typename(unindented, make_typename( NodeStructs::BaseTypename{ i.name } });
						return_if_error(interface_repr);*/
						auto k = members
							| std::views::transform([&](auto&& T) { return typename_of_type(unindented, T); })
							| to_vec();
						auto v = vec_of_expected_to_expected_of_vec(std::move(k));
						return_if_error(v);
						auto members_repr = transpile_typenames(unindented, variables, v.value());
						return_if_error(members_repr);
						declarations << "struct " << i.name << ";\n";
						definitions << "struct " << i.name << " {\n\tUnion<" << members_repr.value() << "> value;\n};\n";
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
							NOT_IMPLEMENTED;
						if (uses_auto(f))
							NOT_IMPLEMENTED;
						auto res = transpile({ state, 0 }, f);
						return_if_error(res);
						declarations << res.value().first;
						definitions << res.value().second;
					}
					if (size != state.functions_to_transpile.size()) {
						puts("size != state.functions_to_transpile.size()");
						for (const auto& f : state.functions_to_transpile)
							if (std::find_if(v.begin(), v.end(), [&](const auto& e) { return cmp(f, e) == std::strong_ordering::equivalent; }) == v.end())
								puts(f.name.c_str());
					}
					{
						for (const std::pair<std::string, std::string>& alias : state.aliases_to_transpile)
							declarations << "using " + alias.first + " = " + alias.second + ";\n";
					}
					declarations << main_transpilation_result.value().first;
					definitions << main_transpilation_result.value().second;
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

transpile_declaration_definition_t transpile_main(
	transpilation_state_with_indent state,
	const NodeStructs::Function& fn
) {
	if (fn.parameters.size() != 1)
		return error{ "user error","\"main\" function requires 1 argument of type `Vector<String> ref`" };

	const auto& [type, name] = fn.parameters.at(0);
	auto vector_str = make_typename(NodeStructs::TemplatedTypename{
		make_typename(NodeStructs::BaseTypename{ "Vector" }, NodeStructs::Value{}, rule_info_language_element("Vector")),
		as_vec(NodeStructs::WordTypenameOrExpression{ "String" })
	}, NodeStructs::Reference{}, rule_info_language_element("Vector<String>"));
	if (cmp(type, vector_str) != std::strong_ordering::equivalent)
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
	for (auto&& [type_name, name] : parameters) {
		auto t = type_of_typename(state, variables, type_name);
		return_if_error(t);
	}
	for (auto&& [type_name, name] : parameters) {
		const auto& cat = type_name.category;
		if (!cat._value.has_value())
			NOT_IMPLEMENTED;
		auto t = type_of_typename(state, variables, type_name);
		if (t.has_error())
			NOT_IMPLEMENTED;
		if (std::holds_alternative<NodeStructs::Value>(cat._value.value()._value))
			variables[name].push_back({ NodeStructs::MutableReference{}, std::move(t).value() });
		else
			variables[name].push_back({ copy(cat._value.value()), std::move(t).value() });
	}
	return std::nullopt;
}

transpile_declaration_definition_t transpile(
	transpilation_state_with_indent state,
	const NodeStructs::Function& fn
) {
	variables_t variables{};
	{
		variables["True"].push_back(
			{
				NodeStructs::Value{},
				{ NodeStructs::PrimitiveType{ NodeStructs::PrimitiveType::Valued{ true } } }
			}
		);
		variables["False"].push_back(
			{
				NodeStructs::Value{},
				{ NodeStructs::PrimitiveType{ NodeStructs::PrimitiveType::Valued{ false } } }
			}
		);
		variables["None"].push_back(
			{
				NodeStructs::Value{},
				{ NodeStructs::PrimitiveType{ NodeStructs::PrimitiveType::Valued{ NodeStructs::empty_optional_t{} } } }
			}
		);
	}
	auto return_typename = transpile_typename(state, variables, fn.returnType);
	return_if_error(return_typename);
	auto return_type = type_of_typename(state, variables, fn.returnType);
	return_if_error(return_type);
	auto parameters = transpile(state, variables, fn.parameters);
	return_if_error(parameters);
	auto stack_params_opt_error = stack(state, variables, fn.parameters);
	if (stack_params_opt_error.has_value())
		return stack_params_opt_error.value();
	auto statements = transpile(state.indented(), variables, fn.statements, return_type.value());
	return_if_error(statements);
	if (fn.name_space.has_value()) {
		std::string common = return_typename.value() + " " + transpile_typename(state, variables, fn.name_space.value()).value() + "__" + fn.name + "(" + parameters.value() + ")";
		std::string declaration = common + ";\n";
		std::string definition = common + " {\n" + statements.value() + "};\n";
		return std::pair{ declaration, definition };
	}
	else {
		std::string common = return_typename.value() + " " + fn.name + "(" + parameters.value() + ")";
		std::string declaration = common + ";\n";
		std::string definition = common + " {\n" + statements.value() + "};\n";
		return std::pair{ declaration, definition };
	}
}

transpile_declaration_definition_t transpile_type(
	transpilation_state_with_indent state,
	const NodeStructs::Type& type
) {
	std::stringstream h, cpp;
	if (type.name_space.has_value()) {
		variables_t variables;
		auto ns_repr_or_e = transpile_typename(state, variables, type.name_space.value());
		return_if_error(ns_repr_or_e);
		h << "struct " << ns_repr_or_e.value() << "__" << type.name << ";\n";
		cpp << "struct " << ns_repr_or_e.value() << "__" << type.name << " {\n";
	}
	else {
		h << "struct " << type.name << ";\n";
		cpp << "struct " << type.name << " {\n";
	}

	if (type.aliases.size() != 0)
		NOT_IMPLEMENTED;

	variables_t variables;
	for (const auto& member : type.member_variables) {
		// todo add member variables to variables, so that you can decltype them in future member variable definitions
		auto type = type_of_typename(state, variables, member.type);
		return_if_error(type);
		auto transpiled = transpile_typename(state, variables, member.type);
		return_if_error(transpiled);
		if (transpiled.value() == "TOKENS")
			NOT_IMPLEMENTED;
		cpp << transpiled.value() << " " << member.name << ";\n";
	}

	cpp << "};\n\n";
	return std::pair{ h.str(), cpp.str() };
}

transpile_declaration_definition_t transpile(
	transpilation_state_with_indent state,
	const NodeStructs::Interface& interface
) {
	return std::pair{ "//generatedtodo const NodeStructs::Interface& interface;", "//generatedtodo const NodeStructs::Interface& interface;"};
}

transpile_t transpile(
	transpilation_state_with_indent state,
	variables_t& variables,
	const std::vector<NodeStructs::FunctionParameter>& parameters
) {
	std::stringstream ss;
	bool first = true;
	for (const auto& [type, name] : parameters) {
		const auto& cat = type.category;
		if (!cat._value.has_value())
			NOT_IMPLEMENTED;
		auto tn = transpile_typename(state, variables, type);
		return_if_error(tn);
		auto s = tn.value();
		if (std::holds_alternative<NodeStructs::Reference>(cat._value.value()._value))
			s = "const " + std::move(s) + "&";
		else if (std::holds_alternative<NodeStructs::MutableReference>(cat._value.value()._value))
			s = std::move(s) + "&";
		else if (std::holds_alternative<NodeStructs::Value>(cat._value.value()._value))
			s = std::move(s) + "&&";
		if (first)
			first = false;
		else
			ss << ", ";
		ss << s << " " << name;
	}
	return ss.str();
}

NodeStructs::MetaType iterator_type(
	transpilation_state_with_indent state,
	const NodeStructs::MetaType& type
) {
	return std::visit(overload(
		[&](const auto& x) -> NodeStructs::MetaType {
			NOT_IMPLEMENTED;
		},
		[&](const NodeStructs::VectorType& vt) -> NodeStructs::MetaType {
			return copy(vt.value_type);
		}
	), type.type.get()._value);
}

std::vector<NodeStructs::MetaType> decompose_type(
	transpilation_state_with_indent state,
	const NodeStructs::MetaType& type
) {
	NOT_IMPLEMENTED;
}

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
				NodeStructs::MetaType iterator_type = type_of_typename(state, variables, it.type).value();
				if (cmp(it_type, iterator_type) != std::strong_ordering::equivalent) {
					auto t = transpile_typename(state, variables, it.type);
					return_if_error(t);
					return error{ "user error","Invalid type of iterator " + t.value() };
				}
				NOT_IMPLEMENTED;
			},
			[&](const std::string& it) -> std::optional<error> {
				variables[it].push_back({ NodeStructs::Reference{}, copy(it_type) });
				return std::nullopt;
			}
		),
		iterators.at(0)._value
	);
}

transpile_expression_information_t transpile_arg(
	transpilation_state_with_indent state,
	variables_t& variables,
	const NodeStructs::FunctionArgument& arg
) {
	auto expr_info = transpile_expression(state, variables, arg.expr);
	return_if_error(expr_info);
	if (!std::holds_alternative<non_type_information>(expr_info.value()))
		NOT_IMPLEMENTED;
	const auto& expr_info_ok = std::get<non_type_information>(expr_info.value());

	if (!arg.category.has_value()) {
		return std::visit(overload(
			[&](const NodeStructs::Reference& e) -> transpile_expression_information_t {
				return copy3(expr_info_ok);
			},
			[&](const NodeStructs::Value& e) -> transpile_expression_information_t {
				return copy3(expr_info_ok);
			},
			[&](const NodeStructs::MutableReference& e) -> transpile_expression_information_t {
				return copy3(expr_info_ok);
			}
		), expr_info_ok.value_category._value);
	}
	else {
		return std::visit(overload(
			[&](const NodeStructs::Reference& expr_cat, const NodeStructs::Reference& arg_cat) -> transpile_expression_information_t {
				return copy3(expr_info_ok);
			},
			[&](const NodeStructs::Value& expr_cat, const NodeStructs::Reference& arg_cat) -> transpile_expression_information_t {
				return error{ "user error", "can't reference a temporary value" };
			},
			[&](const NodeStructs::MutableReference& expr_cat, const NodeStructs::Reference& arg_cat) -> transpile_expression_information_t {
				return copy3(expr_info_ok);
			},
			[&](const NodeStructs::Reference& expr_cat, const NodeStructs::MutableReference& arg_cat) -> transpile_expression_information_t {
				return error{ "user error", "can't mutably reference a reference" };
			},
			[&](const NodeStructs::Value& expr_cat, const NodeStructs::MutableReference& arg_cat) -> transpile_expression_information_t {
				return error{ "user error", "can't mutably reference a temporary value" };
			},
			[&](const NodeStructs::MutableReference& expr_cat, const NodeStructs::MutableReference& arg_cat) -> transpile_expression_information_t {
				return copy3(expr_info_ok);
			},
			[&](const NodeStructs::Reference& expr_cat, const NodeStructs::Move& arg_cat) -> transpile_expression_information_t {
				return error{ "user error", "do not move from a non mutable reference" };
			},
			[&](const NodeStructs::Value& expr_cat, const NodeStructs::Move& arg_cat) -> transpile_expression_information_t {
				return error{ "user error", "do not move temporary values" };
			},
			[&](const NodeStructs::MutableReference& expr_cat, const NodeStructs::Move& arg_cat) -> transpile_expression_information_t {
				return expression_information{
					non_type_information{
						.type = copy(expr_info_ok.type),
						.representation = "std::move(" + expr_info_ok.representation + ")",
						.value_category = NodeStructs::Value{}
					}
				};
			}
		), expr_info_ok.value_category._value, arg.category.value()._value);
	}
}

transpile_t transpile_typenames(
	transpilation_state_with_indent state,
	variables_t& variables,
	const std::vector<NodeStructs::Typename>& args
) {
	if (args.size() == 0)
		return "";
	auto vec = vec_of_expected_to_expected_of_vec(args
		| std::views::transform([&](auto&& T) { return transpile_typename(state, variables, T); })
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

std::string template_name(std::string original_name, const std::vector<NodeStructs::WordTypenameOrExpression>& arguments) {
	NOT_IMPLEMENTED;
	/*return template_name(original_name, arguments
		| std::views::transform([&](auto&& e) { return word_typename_or_expression_for_template(state, variables, e); })
		| to_vec());*/
}

Variant<not_assignable, directly_assignable, requires_conversion> compile_time_assigned_to(
	transpilation_state_with_indent state_,
	variables_t& variables,
	const NodeStructs::MetaType& parameter,
	const NodeStructs::CompileTimeType& arg
) {
	auto res = assigned_to(state_, variables, parameter, arg.type);
	if (holds<not_assignable>(res))
		return not_assignable{};
	else if (holds<directly_assignable>(res)) {
		if (holds<NodeStructs::PrimitiveType>(arg.type)) {
			const auto& x = get<NodeStructs::PrimitiveType>(arg.type);
			return requires_conversion{
				[&](transpilation_state_with_indent, variables_t&, const NodeStructs::Expression& expr) -> transpile_expression_information_t {
					std::string representation = [&]() {
						std::stringstream ss;
						std::visit(overload(
							[&](const auto& val) {
								NOT_IMPLEMENTED;
							},
							[&](const NodeStructs::PrimitiveType::Valued<int>& val) {
								ss << val.value;
							},
							[&](const NodeStructs::void_t& val) { ss << "void_t{}"; },
							[&](const NodeStructs::empty_optional_t& val) { ss << "empty_optional_t{}"; }
						), x.value._value);
						return ss.str();
					}();
					return non_type_information{.type = copy(x), .representation = std::move(representation), .value_category = NodeStructs::Value{} };
				}
			};
		}
		NOT_IMPLEMENTED;
	}
	NOT_IMPLEMENTED;
}

struct no_previous_conversion {};

requires_conversion get_converter(const NodeStructs::MetaType& to, auto previous_converter) {
	return requires_conversion{
		[T = copy(to), previous_converter = std::move(previous_converter)]
		(transpilation_state_with_indent state, variables_t& variables, const NodeStructs::Expression& expr) -> transpile_expression_information_t {
			auto tn = typename_of_type(state, T);
			if (tn.has_error())
				NOT_IMPLEMENTED;

			auto tn_str = transpile_typename(state, variables, tn.value());
			if (tn_str.has_error())
				NOT_IMPLEMENTED;

			transpile_expression_information_t transpiled_expr = [&]() -> transpile_expression_information_t {
				if constexpr (std::is_same_v<std::remove_cvref_t<decltype(previous_converter)>, requires_conversion>)
					return const_cast<requires_conversion&>(previous_converter).converter(state, variables, expr);
				else if constexpr (std::is_same_v<std::remove_cvref_t<decltype(previous_converter)>, no_previous_conversion>)
					return transpile_expression(state, variables, expr);
				else
					NOT_IMPLEMENTED;
			}();
			return_if_error(transpiled_expr);
			if (std::holds_alternative<type_information>(transpiled_expr.value()))
				NOT_IMPLEMENTED;

			return expression_information{ non_type_information{
				.type = { std::move(const_cast<decltype(T)&>(T))},
				.representation = tn_str.value() + "{" + std::get<non_type_information>(std::move(transpiled_expr).value()).representation + "}",
				.value_category = NodeStructs::Value{}
			} };
		}
	};
}

Variant<not_assignable, directly_assignable, requires_conversion> assigned_to(
	transpilation_state_with_indent state_,
	variables_t& variables,
	const NodeStructs::MetaType& parameter,
	const NodeStructs::MetaType& argument
) {
	using R = Variant<not_assignable, directly_assignable, requires_conversion>;

	if (holds<NodeStructs::CompileTimeType>(argument))
		return compile_time_assigned_to(state_, variables, parameter, get<NodeStructs::CompileTimeType>(argument));

	return std::visit(
		overload(
			[&](const auto& param, const auto& arg, const auto& state) -> R {
				NOT_IMPLEMENTED;
			},
			[&](const NodeStructs::OptionalType& param, const auto& arg, const auto& state) -> R {
				return assigned_to(state_, variables, param.value_type, { copy(arg) });
			},
			[&](const NodeStructs::Builtin& param, const auto& arg, const auto& state) -> R {
				if (param.name == "builtin_filesystem_directory") {
					return assigned_to(state, variables, NodeStructs::MetaType{ NodeStructs::PrimitiveType{ NodeStructs::PrimitiveType::NonValued<std::string>{} } }, { copy(arg) });
				}
				NOT_IMPLEMENTED;
			},
			[&](const NodeStructs::UnionType& param, const auto& arg, const auto& state) -> R {
				if constexpr (std::is_same_v<std::remove_cvref_t<decltype(arg)>, NodeStructs::UnionType>) {
					// check each union type of u is assignable into e
					for (const auto& arg : arg.arguments) {
						auto arg_assigned_to = assigned_to(state, variables, parameter, arg);
						if (!std::holds_alternative<directly_assignable>(arg_assigned_to._value))
							return not_assignable{};
					}
					return directly_assignable{};
				}
				else {
					for (const auto& vt : param.arguments) {
						auto union_arg_assignable = assigned_to(state, variables, vt, argument);
						if (std::holds_alternative<directly_assignable>(union_arg_assignable._value))
							return directly_assignable{};
					}
					return not_assignable{};
				}
			},
			[&](const NodeStructs::Type& param, const NodeStructs::AggregateType& arg, const auto& state) -> R {
				if (param.member_variables.size() != arg.arguments.size())
					return not_assignable{};
				std::vector<R> assign_ts;
				assign_ts.reserve(param.member_variables.size());
				bool is_directly_assignable = true;
				for (size_t i = 0; i < param.member_variables.size(); ++i) {
					auto mem_t = type_of_typename(state, variables, param.member_variables.at(i).type);
					if (mem_t.has_error())
						NOT_IMPLEMENTED;
					auto assign_t = assigned_to(state, variables, mem_t.value(), arg.arg_types.at(i))._value;
					if (std::holds_alternative<not_assignable>(assign_t))
						return not_assignable{};
					else if (std::holds_alternative<requires_conversion>(assign_t))
						is_directly_assignable = false;
					assign_ts.push_back(std::move(assign_t));
				}
				if (is_directly_assignable)
					return directly_assignable{};
				else {
					return requires_conversion{
						[T = copy(param), s = param.name, assign_ts = std::move(assign_ts)]
						(transpilation_state_with_indent state, variables_t& variables, const NodeStructs::Expression& expr) -> transpile_expression_information_t {
							if (!holds<NodeStructs::BraceArguments>(expr))
								NOT_IMPLEMENTED;
							const auto& assumed_aggregate = get<NodeStructs::BraceArguments>(expr);
							std::stringstream ss;
							std::vector<R>& non_const_assign_ts = const_cast<std::vector<R>&>(assign_ts);
							for (size_t i = 0; i < assign_ts.size(); ++i) {
								R& assign_t = non_const_assign_ts.at(i);
								const NodeStructs::FunctionArgument& expr = assumed_aggregate.args.at(i);
								std::visit(overload(
									[&](auto& assignable) -> transpile_expression_information_t {
										auto expr_info = [&]() {
											if constexpr (std::is_same_v<std::remove_cvref_t<decltype(assignable)>, requires_conversion>)
												return assignable.converter(state, variables, expr.expr); // todo doesnt use expr.cat
											else
												return transpile_arg(state, variables, expr);
										}();
										return_if_error(expr_info);
										if (!std::holds_alternative<non_type_information>(expr_info.value()))
											NOT_IMPLEMENTED;
										const auto& expr_info_ok = std::get<non_type_information>(expr_info.value());
										ss << expr_info_ok.representation << ", ";
										NOT_IMPLEMENTED;
									},
									[](not_assignable&) -> transpile_expression_information_t {
										NOT_IMPLEMENTED;
									}
								), assign_t._value);
							}
							return expression_information{ non_type_information{
								.type = { std::move(const_cast<decltype(T)&>(T)) },
								.representation = s + "{" + ss.str() + "}",
								.value_category = NodeStructs::Value{}
							} };
						}
					};
				}
			},
			[&](const NodeStructs::Type& param, const NodeStructs::Type& arg, const auto& state) -> R {
				if (cmp(param, arg) == std::strong_ordering::equivalent)
					return directly_assignable{};
				// if the parameter has 1 member, try to assign to that member
				if (param.member_variables.size() == 1) {
					auto member_variable_t = type_of_typename(state, variables, param.member_variables.at(0).type);
					if (member_variable_t.has_error())
						NOT_IMPLEMENTED;
					// in the event where assignment can work, we still need a `requires_conversion`
					R mem_assigned = assigned_to(state, variables, member_variable_t.value(), argument);
					if (std::holds_alternative<directly_assignable>(mem_assigned._value)) {
						return requires_conversion{
							[s = param.name, cp = copy(param)](transpilation_state_with_indent state, variables_t& variables, const NodeStructs::Expression& expr) -> transpile_expression_information_t {
								transpile_expression_information_t expr_info = transpile_expression(state, variables, expr);
								return_if_error(expr_info);
								if (!std::holds_alternative<non_type_information>(expr_info.value()))
									NOT_IMPLEMENTED;
								non_type_information& expr_info_ok = std::get<non_type_information>(expr_info.value());
								return expression_information{ non_type_information{
									.type = NodeStructs::MetaType{ std::move(const_cast<NodeStructs::Type&>(cp)) },
									.representation = s + "{" + expr_info_ok.representation + "}",
									.value_category = std::move(expr_info_ok).value_category
								} };
							}
						};
					}
					else if (std::holds_alternative<requires_conversion>(mem_assigned._value)) {
						return requires_conversion{
							[s = param.name, cp = copy(param), mem_assigned = std::get<requires_conversion>(std::move(mem_assigned)._value)]
							(transpilation_state_with_indent state, variables_t& variables, const NodeStructs::Expression& expr) -> transpile_expression_information_t {
								requires_conversion& rc = const_cast<requires_conversion&>(mem_assigned);
								transpile_expression_information_t expr_info = rc.converter(state, variables, expr);
								return_if_error(expr_info);
								if (!std::holds_alternative<non_type_information>(expr_info.value()))
									NOT_IMPLEMENTED;
								non_type_information& expr_info_ok = std::get<non_type_information>(expr_info.value());
								return expression_information{ non_type_information{
									.type = NodeStructs::MetaType{ std::move(const_cast<NodeStructs::Type&>(cp)) },
									.representation = s + "{" + expr_info_ok.representation + "}",
									.value_category = std::move(expr_info_ok).value_category
								} };
							}
						};
					}
					else
						return not_assignable{};
				}
				return not_assignable{};
			},
			
			[&](const NodeStructs::PrimitiveType& param, const NodeStructs::PrimitiveType& arg, const auto& state) -> R {
				if (holds<NodeStructs::PrimitiveType::NonValued<std::string>>(param.value) && (holds<NodeStructs::PrimitiveType::NonValued<char>>(arg.value) || holds<NodeStructs::PrimitiveType::Valued<char>>(arg.value)))
					return requires_conversion{
						[](transpilation_state_with_indent state, variables_t& variables, const NodeStructs::Expression& expr) -> transpile_expression_information_t {
							transpile_expression_information_t expr_info = transpile_expression(state, variables, expr);
							return_if_error(expr_info);
							if (!std::holds_alternative<non_type_information>(expr_info.value()))
								NOT_IMPLEMENTED;
							non_type_information& expr_info_ok = std::get<non_type_information>(expr_info.value());
							return expression_information{ non_type_information{
								.type = NodeStructs::PrimitiveType{ NodeStructs::PrimitiveType::NonValued<std::string>{} },
								.representation = "String{" + expr_info_ok.representation + "}",
								.value_category = std::move(expr_info_ok).value_category
							} };
						}
					};
				if (param.value._value.index() == arg.value._value.index())
					return directly_assignable{};
				else if (param.value._value.index() + 7 == arg.value._value.index())
					return directly_assignable{};
				else
					return not_assignable{};
			},
			[&](const NodeStructs::OptionalType& param, const NodeStructs::PrimitiveType& arg, const auto& state) -> R {
				if (holds<NodeStructs::PrimitiveType::Valued<NodeStructs::empty_optional_t>>(only(arg)))
					return directly_assignable{};
				else {
					auto inner_assignable = assigned_to(state, variables, only(param), argument);
					if (std::holds_alternative<directly_assignable>(inner_assignable._value))
						return get_converter(only(param), no_previous_conversion{});
					else if (std::holds_alternative<requires_conversion>(inner_assignable._value))
						return get_converter(only(param), std::get<requires_conversion>(std::move(inner_assignable._value)));
					else
						return not_assignable{};
				}
			},
			[&](const NodeStructs::Builtin& param, const NodeStructs::PrimitiveType& arg, const auto& state) -> R {
				if (param.name == "builtin_filesystem_directory") {
					return assigned_to(state, variables, NodeStructs::MetaType{ NodeStructs::PrimitiveType{ NodeStructs::PrimitiveType::NonValued<std::string>{} } }, { copy(arg) });
				}
				NOT_IMPLEMENTED; 
			},
			[&](const NodeStructs::UnionType& param, const NodeStructs::PrimitiveType& arg, const auto& state) -> R { NOT_IMPLEMENTED; },
			[&](const auto& param, const NodeStructs::PrimitiveType& arg, const auto& state) -> R {
				return not_assignable{};
			},
			
			[&](const NodeStructs::EnumType& param, const NodeStructs::EnumValueType& arg, const auto& state) -> R {
				if (cmp(param.enum_.get(), arg.enum_.get()) == std::strong_ordering::equivalent)
					return directly_assignable{};
				else
					return not_assignable{};
			},
			[&](const NodeStructs::Builtin& param, const NodeStructs::Builtin& arg, const auto& state) -> R {
				if (param.name == arg.name)
					return directly_assignable{};
				else
					return not_assignable{};
			},
			[&](const NodeStructs::VectorType& param, const NodeStructs::VectorType& arg, const auto& state) -> R {
				if (cmp(param.value_type, arg.value_type) == std::strong_ordering::equivalent)
					return directly_assignable{};
				else
					return not_assignable{};
			},
			[&](const NodeStructs::SetType& param, const NodeStructs::SetType& arg, const auto& state) -> R {
				if (cmp(param.value_type, arg.value_type) == std::strong_ordering::equivalent)
					return directly_assignable{};
				else
					return not_assignable{};
			},
			[&](const NodeStructs::InterfaceType& param, const NodeStructs::Type& arg, const auto& state) -> R {
				for (const auto& [type_name, name] : param.interface.get().member_variables) {
					auto t = type_of_typename(state, variables, type_name);
					if (t.has_error())
						NOT_IMPLEMENTED;
					for (const auto& [source_type_name, source_name] : arg.member_variables)
						if (name == source_name) {
							auto t2 = type_of_typename(state, variables, source_type_name);
							if (t2.has_error())
								NOT_IMPLEMENTED;
							if (cmp(t.value(), t2.value()) != std::strong_ordering::equivalent)
								return not_assignable{};
						}
				}
				auto& interfacemembers = state.state.interface_symbol_to_members[
					make_typename(NodeStructs::BaseTypename{ param.interface.get().name }, NodeStructs::Value{}, rule_info_stub_no_throw())
				];
				auto new_member = NodeStructs::MetaType{ copy(arg) };
				for (const auto& member : interfacemembers)
					if (cmp(member, new_member) == std::strong_ordering::equivalent)
						return directly_assignable{};
				interfacemembers.push_back(std::move(new_member));
				return directly_assignable{};
			}
		),
		parameter.type.get()._value, argument.type.get()._value, std::variant<transpilation_state_with_indent>{state_}
	);
}

transpile_t expr_to_printable(transpilation_state_with_indent state, variables_t& variables, const NodeStructs::Expression& expr) {
	auto t = transpile_expression(state, variables, expr);
	return_if_error(t);
	if (!std::holds_alternative<non_type_information>(t.value()))
		NOT_IMPLEMENTED;
	const auto& t_ok = std::get<non_type_information>(t.value());

	if (holds<NodeStructs::Type>(t_ok.type)) {
		std::stringstream ss;
		const auto& t_ref = get<NodeStructs::Type>(t_ok.type);
		ss << t_ref.name << "{";
		for (const auto& [member_typename, member_name] : t_ref.member_variables) {
			auto typename_t = type_of_typename(state, variables, member_typename);
			return_if_error(typename_t);
			auto member = NodeStructs::PropertyAccessExpression{ .operand = copy(expr), .property_name = member_name };
			auto member_repr = expr_to_printable(state, variables, make_expression(std::move(member), rule_info_stub_no_throw()));
			return_if_error(member_repr);
			ss << member_name << " = \" << " << member_repr.value();
		}
		return ss.str();
	}
	else if (holds<NodeStructs::InterfaceType>(t_ok.type)) {
		const auto& t_ref = get<NodeStructs::InterfaceType>(t_ok.type).interface.get();
		std::stringstream ss;
		ss << "\"" << t_ref.name << "{";
		for (const auto& [member_typename, member_name] : t_ref.member_variables) {
			auto typename_t = type_of_typename(state, variables, member_typename);
			return_if_error(typename_t);
			auto member = NodeStructs::PropertyAccessExpression{ .operand = copy(expr), .property_name = member_name };
			auto member_repr = expr_to_printable(state, variables, make_expression(std::move(member), rule_info_stub_no_throw()));
			return_if_error(member_repr);
			ss << member_name << " = \" << " << member_repr.value();
		}
		ss << " << \"}\"";
		return ss.str();
	}
	else if (holds<NodeStructs::PrimitiveType>(t_ok.type)) {
		const auto& primitive_type = get<NodeStructs::PrimitiveType>(t_ok.type);
		auto expr_repr = transpile_expression(state, variables, expr);
		return_if_error(expr_repr);
		if (!std::holds_alternative<non_type_information>(expr_repr.value()))
			NOT_IMPLEMENTED;
		const auto& expr_repr_ok = std::get<non_type_information>(expr_repr.value());
		return std::visit(
			overload(
				[&](const NodeStructs::PrimitiveType::NonValued<std::string>&) -> std::string {
					return "('\"' + " + expr_repr_ok.representation + " + '\"')";
				},
				[&](const NodeStructs::PrimitiveType::Valued<std::string>&) -> std::string {
					return expr_repr_ok.representation;
				},
				[&](const NodeStructs::PrimitiveType::NonValued<char>&) -> std::string {
					// not optimal
					return "(String(\"'\") + " + expr_repr_ok.representation + " + String(\"'\"))";
				},
				[&](const NodeStructs::PrimitiveType::NonValued<int>&) -> std::string {
					return expr_repr_ok.representation;
				},
				[&](const NodeStructs::PrimitiveType::NonValued<double>&) -> std::string {
					return expr_repr_ok.representation;
				},
				[&](const NodeStructs::PrimitiveType::NonValued<bool>&) -> std::string {
					return "String{" + expr_repr_ok.representation + " ? \"True\" : \"False\"}";
				},
				[&](const NodeStructs::PrimitiveType::NonValued<NodeStructs::void_t>&) -> std::string {
					return "String(\"Void\")";
				},
				[&](const NodeStructs::PrimitiveType::NonValued<NodeStructs::empty_optional_t>&) -> std::string {
					return "String(\"None\")";
				},
				[&](const auto& e) -> std::string {
					// const auto& f = expr_repr_ok;
					NOT_IMPLEMENTED;
				}
			),
			primitive_type.value._value
		);
	}
	else {
		NOT_IMPLEMENTED;
	}
}

bool uses_auto(const NodeStructs::Function& fn) {
	if (cmp(fn.returnType.value, make_typename(NodeStructs::BaseTypename{ "auto" }, std::nullopt, rule_info_language_element("auto")).value) == std::strong_ordering::equivalent)
		return true;
	for (const auto& param : fn.parameters)
		if (uses_auto(param))
			return true;
	return false;
}

bool uses_auto(const NodeStructs::FunctionParameter& param) {
	return uses_auto(param.typename_);
}

bool uses_auto(const NodeStructs::BaseTypename& t) {
	return t.type == "auto";
}

bool uses_auto(const NodeStructs::NamespacedTypename& t) {
	if (t.name_in_name_space == "auto" || uses_auto(t.name_space))
		NOT_IMPLEMENTED;
	return false;
}

bool uses_auto(const NodeStructs::TemplatedTypename& t) {
	if (uses_auto(t.type))
		return true;
	for (const auto& param : t.templated_with)
		if (holds<NodeStructs::Typename>(param.value)
			&& uses_auto(get<NodeStructs::Typename>(param.value)))
			return true;
		else if (holds<NodeStructs::Expression>(param.value)
				&& uses_auto(get<NodeStructs::Expression>(param.value)))
			return true;
	return false;
}

bool uses_auto(const NodeStructs::Expression& t) {
	NOT_IMPLEMENTED;
}

bool uses_auto(const NodeStructs::UnionTypename& t) {
	for (const auto& param : t.ors)
		if (uses_auto(param))
			return true;
	return false;
}

bool uses_auto(const NodeStructs::OptionalTypename& t) {
	return uses_auto(t.type);
}

bool uses_auto(const NodeStructs::VariadicExpansionTypename& t) {
	auto res = uses_auto(t.type);
	if (res)
		NOT_IMPLEMENTED;
	return res;
}

bool uses_auto(const NodeStructs::Typename& t) {
	return std::visit([](const auto& t_) { return uses_auto(t_); }, t.value.get()._value);
}

expected<std::optional<NodeStructs::MetaType>> deduce_return_type(
	transpilation_state_with_indent state,
	variables_t& variables,
	const NodeStructs::Expression& statement
) {
	return std::nullopt;
}

template <typename context>
expected<std::optional<NodeStructs::MetaType>> deduce_return_type(
	transpilation_state_with_indent state,
	variables_t& variables,
	const NodeStructs::VariableDeclarationStatement<context>& statement
) {
	NOT_IMPLEMENTED;
}

template <typename context>
expected<std::optional<NodeStructs::MetaType>> deduce_return_type(
	transpilation_state_with_indent state,
	variables_t& variables,
	const NodeStructs::IfStatement<context>& statement
) {
	NOT_IMPLEMENTED;
}

template <typename context>
expected<std::optional<NodeStructs::MetaType>> deduce_return_type(
	transpilation_state_with_indent state,
	variables_t& variables,
	const NodeStructs::ForStatement<context>& statement
) {
	NOT_IMPLEMENTED;
}

template <typename context>
expected<std::optional<NodeStructs::MetaType>> deduce_return_type(
	transpilation_state_with_indent state,
	variables_t& variables,
	const NodeStructs::IForStatement<context>& statement
) {
	NOT_IMPLEMENTED;
}

template <typename context>
expected<std::optional<NodeStructs::MetaType>> deduce_return_type(
	transpilation_state_with_indent state,
	variables_t& variables,
	const NodeStructs::WhileStatement<context>& statement
) {
	NOT_IMPLEMENTED;
}

template <typename context>
expected<std::optional<NodeStructs::MetaType>> deduce_return_type(
	transpilation_state_with_indent state,
	variables_t& variables,
	const NodeStructs::BreakStatement<context>& statement
) {
	NOT_IMPLEMENTED;
}

template <typename context>
expected<std::optional<NodeStructs::MetaType>> deduce_return_type(
	transpilation_state_with_indent state,
	variables_t& variables,
	const NodeStructs::ReturnStatement<context>& statement
) {
	if (statement.ifExpr.has_value())
		NOT_IMPLEMENTED;
	if (statement.returnExpr.size() != 1)
		NOT_IMPLEMENTED;
	if (statement.returnExpr.size() == 0)
		return expected<std::optional<NodeStructs::MetaType>>{
			std::optional<NodeStructs::MetaType>{
				NodeStructs::MetaType{
					NodeStructs::PrimitiveType{
						NodeStructs::PrimitiveType::NonValued<NodeStructs::void_t>{}
					}
				}
			}
		};
	auto arg_info = transpile_arg(state, variables, statement.returnExpr.at(0));
	return_if_error(arg_info);
	if (!std::holds_alternative<non_type_information>(arg_info.value()))
		NOT_IMPLEMENTED;
	return std::get<non_type_information>(std::move(arg_info).value()).type;
}

template <typename context>
expected<std::optional<NodeStructs::MetaType>> deduce_return_type(
	transpilation_state_with_indent state,
	variables_t& variables,
	const NodeStructs::BlockStatement<context>& statement
) {
	NOT_IMPLEMENTED;
}

template <typename context>
expected<std::optional<NodeStructs::MetaType>> deduce_return_type(
	transpilation_state_with_indent state,
	variables_t& variables,
	const NodeStructs::MatchStatement<context>& statement
) {
	NOT_IMPLEMENTED;
}

template <typename context>
expected<std::optional<NodeStructs::MetaType>> deduce_return_type(
	transpilation_state_with_indent state,
	variables_t& variables,
	const NodeStructs::SwitchStatement<context>& statement
) {
	NOT_IMPLEMENTED;
}

template <typename context>
expected<std::optional<NodeStructs::MetaType>> deduce_return_type(
	transpilation_state_with_indent state,
	variables_t& variables,
	const NodeStructs::Assignment<context>& statement
) {
	NOT_IMPLEMENTED;
}

expected<std::optional<NodeStructs::MetaType>> deduce_return_type(
	transpilation_state_with_indent state,
	variables_t& variables,
	const NodeStructs::RunTimeStatement& statement
) {
	return caesium_lib::variant::visit(
		statement,
		[&](const auto& statement) {
			return deduce_return_type(state, variables, statement);
		}
	);
	NOT_IMPLEMENTED;
	//return std::visit(
	//	overload(
	//		overload_default_error,
	//		[&](const NodeStructs::ReturnStatement<context>& statement) -> expected<std::optional<NodeStructs::MetaType>> {
	//			if (statement.ifExpr.has_value())
	//				NOT_IMPLEMENTED;
	//			if (statement.returnExpr.size() != 1)
	//				NOT_IMPLEMENTED;
	//			auto res_info = transpile_expression(state, variables, statement.returnExpr.at(0).expr);
	//			return_if_error(res_info);
	//			if (!std::holds_alternative<non_type_information>(res_info.value()))
	//				NOT_IMPLEMENTED;
	//			return std::get<non_type_information>(std::move(res_info).value()).type;
	//		},
	//		[&](const NodeStructs::Expression& statement) -> expected<std::optional<NodeStructs::MetaType>> {
	//			return std::nullopt;
	//		},
	//		[&](const NodeStructs::VariableDeclarationStatement<context>& statement) -> expected<std::optional<NodeStructs::MetaType>> {
	//			return std::nullopt;
	//		},
	//		[&](const NodeStructs::IfStatement<context>& statement) -> expected<std::optional<NodeStructs::MetaType>> {
	//			auto if_res_t = deduce_return_type(state, variables, statement.ifStatements);
	//			return_if_error(if_res_t);

	//			if (!statement.elseExprStatements.has_value())
	//				return if_res_t;

	//			auto else_res_t = std::visit(
	//				overload(
	//					[&](const std::vector<NodeStructs::Statement<context>>& statements) -> expected<std::optional<NodeStructs::MetaType>> {
	//						return deduce_return_type(state, variables, statements);
	//					},
	//					[&](const NonCopyableBox<NodeStructs::IfStatement<context>>& inner_if) -> expected<std::optional<NodeStructs::MetaType>> {
	//						return deduce_return_type(state, variables, NodeStructs::Statement<context>{ copy(inner_if.get()) });
	//					}
	//				),
	//				statement.elseExprStatements.value()._value
	//			);
	//			return_if_error(else_res_t);

	//			if (if_res_t.value().has_value() && else_res_t.value().has_value())
	//				if (cmp(if_res_t.value().value(), else_res_t.value().value()) != std::strong_ordering::equivalent)
	//					NOT_IMPLEMENTED;
	//				else
	//					return if_res_t;
	//			if (if_res_t.value().has_value())
	//				return if_res_t;
	//			return std::move(else_res_t);
	//		},
	//		[&](const NodeStructs::IForStatement<context>& statement) -> expected<std::optional<NodeStructs::MetaType>> {
	//			NOT_IMPLEMENTED;
	//		},
	//		[&](const NodeStructs::ForStatement<context>& statement) -> expected<std::optional<NodeStructs::MetaType>> {
	//			auto coll_type_or_e = transpile_expression(state, variables, statement.collection);
	//			return_if_error(coll_type_or_e);
	//			if (!std::holds_alternative<non_type_information>(coll_type_or_e.value()))
	//				NOT_IMPLEMENTED;
	//			const auto& coll_type_or_e_ok = std::get<non_type_information>(coll_type_or_e.value());
	//			auto it_type = iterator_type(state, coll_type_or_e_ok.type);
	//			if (statement.iterators.size() > 1)
	//				NOT_IMPLEMENTED;
	//			auto opt_e = add_for_iterator_variables(state, variables, statement.iterators, it_type);
	//			if (opt_e.has_value())
	//				return opt_e.value();
	//			return deduce_return_type(state, variables, statement.statements);
	//		},
	//		[&](const NodeStructs::WhileStatement<context>& statement) -> expected<std::optional<NodeStructs::MetaType>> {
	//			return deduce_return_type(state, variables, statement.statements);
	//		},
	//		[&](const NodeStructs::BreakStatement<context>& statement) -> expected<std::optional<NodeStructs::MetaType>> {
	//			return std::nullopt;
	//		},
	//		[&](const NodeStructs::BlockStatement<context>& statement) -> expected<std::optional<NodeStructs::MetaType>> {
	//			NOT_IMPLEMENTED;
	//		},
	//		[&](const NodeStructs::MatchStatement<context>& statement) -> expected<std::optional<NodeStructs::MetaType>> {
	//			// for each match case copy variables, add the match variables, note the return type from the case, compare with stored
	//			if (statement.expressions.size() != 1)
	//				NOT_IMPLEMENTED;
	//			if (statement.cases.size() == 0)
	//				NOT_IMPLEMENTED;
	//			/*auto expr_info = transpile_expression(state, variables, statement.expressions.at(0));
	//			return_if_error(expr_info);
	//			if (!std::holds_alternative<non_type_information>(expr_info.value()))
	//				NOT_IMPLEMENTED;
	//			const auto& expr_ok = std::get<non_type_information>(expr_info.value());
	//			auto tn = typename_of_type(state, expr_ok.type.type);
	//			return_if_error(tn);
	//			auto tn_repr = transpile_typename(state, variables, tn.value());
	//			return_if_error(tn_repr);*/

	//			std::optional<NodeStructs::MetaType> res = std::nullopt;
	//			for (const NodeStructs::MatchCase<context>& match_case : statement.cases) {
	//				if (match_case.variable_declarations.size() != 1)
	//					NOT_IMPLEMENTED;
	//				auto tn = transpile_typename(state, variables, match_case.variable_declarations.at(0).first);
	//				return_if_error(tn);
	//				const auto& varname = match_case.variable_declarations.at(0).second;
	//				variables[varname]
	//					.push_back(variable_info{
	//						.value_category = NodeStructs::Reference{},
	//						.type = type_of_typename(state, variables, match_case.variable_declarations.at(0).first).value()
	//						});
	//				auto deduced = deduce_return_type(state, variables, match_case.statements);
	//				variables[varname].pop_back();
	//				return_if_error(deduced);

	//				if (deduced.value().has_value())
	//					if (res.has_value())
	//						if (cmp(res.value(), deduced.value().value()) != std::strong_ordering::equivalent)
	//							NOT_IMPLEMENTED;
	//						else
	//							continue;
	//					else
	//						res.emplace(std::move(deduced).value().value());
	//			}
	//			return res;
	//			// here we would actually know at compile time that this cant be hit so we wouldnt actually insert a throw, it will be removed eventually
	//		},
	//		[&](const NodeStructs::SwitchStatement<context>& statement) -> expected<std::optional<NodeStructs::MetaType>> {
	//			NOT_IMPLEMENTED;
	//		},
	//		[&](const NodeStructs::Assignment<context>& statement) -> expected<std::optional<NodeStructs::MetaType>> {
	//			return std::nullopt;
	//		}
	//	),
	//	statement.statement.get()._value
	//);
}

expected<std::optional<NodeStructs::MetaType>> deduce_return_type(
	transpilation_state_with_indent state,
	variables_t& variables,
	const NodeStructs::contextual_options<function_context>& statement
) {
	return caesium_lib::variant::visit(statement, [&](const auto& statement) { return deduce_return_type(state, variables, statement); });
}

template <typename context>
expected<std::optional<NodeStructs::MetaType>> deduce_return_type(
	transpilation_state_with_indent state,
	variables_t& variables,
	const NodeStructs::Statement<context>& statement
) {
	return caesium_lib::variant::visit(statement.statement.get(), overload(
		[&](const NodeStructs::CompileTimeStatement<context>& statement) -> expected<std::optional<NodeStructs::MetaType>> {
			NOT_IMPLEMENTED;
		},
		[&](const NodeStructs::contextual_options<context>& contextual)->expected<std::optional<NodeStructs::MetaType>>{
			return deduce_return_type(state, variables, contextual);
		}
	));
}

template <typename context>
expected<std::optional<NodeStructs::MetaType>> deduce_return_type(
	transpilation_state_with_indent state,
	variables_t& variables,
	const std::vector<NodeStructs::Statement<context>>& statements
) {
	std::optional<NodeStructs::MetaType> res = std::nullopt;
	for (const auto& statement : statements) {
		expected<std::optional<NodeStructs::MetaType>> e = deduce_return_type(state, variables, statement);
		return_if_error(e);

		if (e.value().has_value()) {
			if (res.has_value()) {
				if (cmp(res.value(), e.value().value()) != std::strong_ordering::equivalent) {
					NOT_IMPLEMENTED;
				}
				else {
					continue;
				}
			}
			else {
				res.emplace(std::move(e).value().value());
			}
		}
	}
	return res;
}

expected<NodeStructs::Function> realise_function_using_auto(
	transpilation_state_with_indent state,
	const NodeStructs::Function& fn_using_auto,
	const std::vector<NodeStructs::MetaType>& arg_types
) {
	if (fn_using_auto.parameters.size() != arg_types.size())
		NOT_IMPLEMENTED;

	NodeStructs::Function realised = copy(fn_using_auto);

	for (const auto& [index, param] : enumerate(fn_using_auto.parameters))
		if (uses_auto(param)) {
			if (cmp(param.typename_.value.get(), NodeStructs::Typename::vt{ NodeStructs::BaseTypename{"auto"} }) == std::strong_ordering::equivalent) {
				auto tn = typename_of_type(state, arg_types.at(index));
				return_if_error(tn);
				auto del = std::move(realised.parameters.at(index).typename_);
				new (&realised.parameters.at(index).typename_) NodeStructs::Typename{ std::move(tn).value().value, copy(param.typename_.category), rule_info_stub_no_throw() };
			}
			else
				NOT_IMPLEMENTED;
		}

	for (auto&& statement : realised.statements)
		if (uses_auto(statement))
			NOT_IMPLEMENTED;

	variables_t variables{};
	{
		variables["True"].push_back(
			{
				NodeStructs::Value{},
				{ NodeStructs::PrimitiveType{ NodeStructs::PrimitiveType::Valued{ true } } }
			}
		);
		variables["False"].push_back(
			{
				NodeStructs::Value{},
				{ NodeStructs::PrimitiveType{ NodeStructs::PrimitiveType::Valued{ false } } }
			}
		);
	}

	auto stack_params_opt_error = stack(state, variables, realised.parameters);
	if (stack_params_opt_error.has_value())
		return stack_params_opt_error.value();

	if (cmp(realised.returnType.value, make_typename(NodeStructs::BaseTypename{ "auto" }, std::nullopt, rule_info_stub_no_throw()).value) != std::strong_ordering::equivalent) {
		auto fn_or_error = transpile(state, realised);
		return_if_error(fn_or_error);
		return NodeStructs::Function{
			.name = realised.name,
			.name_space = std::nullopt,
			.returnType = std::move(realised.returnType),
			.parameters = std::move(realised.parameters),
			.statements = std::move(realised.statements)
		};
	}
	else {
		auto return_tn = deduce_return_type(state, variables, realised.statements);
		return_if_error(return_tn);

		NodeStructs::Function res = [&]() {
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
					.returnType = make_typename(NodeStructs::BaseTypename{ "Void" }, std::nullopt, rule_info_stub_no_throw()),
					.parameters = std::move(realised.parameters),
					.statements = std::move(realised.statements)
				};
		}();

		auto fn_or_error = transpile(state, res);
		return_if_error(fn_or_error);
		return res;
	}
}

NodeStructs::Typename typename_of_primitive(const NodeStructs::PrimitiveType& primitive_t) {
	return std::visit(overload(overload_default_error,
		[&](const NodeStructs::PrimitiveType::NonValued<std::string>&) {
			return make_typename(NodeStructs::BaseTypename{ "String" }, NodeStructs::Value{}, rule_info_language_element("String"));
		},
		[&](const NodeStructs::PrimitiveType::NonValued<double>&) {
			return make_typename(NodeStructs::BaseTypename{ "Floating" }, NodeStructs::Value{}, rule_info_language_element("Floating"));
		},
		[&](const NodeStructs::PrimitiveType::NonValued<int>&) {
			return make_typename(NodeStructs::BaseTypename{ "Int" }, NodeStructs::Value{}, rule_info_language_element("Int"));
		},
		[&](const NodeStructs::PrimitiveType::NonValued<bool>&) {
			return make_typename(NodeStructs::BaseTypename{ "Bool" }, NodeStructs::Value{}, rule_info_language_element("Bool"));
		},
		[&](const NodeStructs::PrimitiveType::NonValued<NodeStructs::void_t>&) {
			return make_typename(NodeStructs::BaseTypename{ "Void" }, NodeStructs::Value{}, rule_info_language_element("Void"));
		},
		[&](const NodeStructs::PrimitiveType::NonValued<NodeStructs::empty_optional_t>&) {
			return make_typename(NodeStructs::BaseTypename{ "None" }, NodeStructs::Value{}, rule_info_language_element("None"));
		},
		[&](const NodeStructs::PrimitiveType::NonValued<char>&) {
			return make_typename(NodeStructs::BaseTypename{ "Char" }, NodeStructs::Value{}, rule_info_language_element("Char"));
		},
		[&](const NodeStructs::PrimitiveType::Valued<std::string>&) {
			return make_typename(NodeStructs::BaseTypename{ "String" }, NodeStructs::Value{}, rule_info_language_element("String"));
		},
		[&](const NodeStructs::PrimitiveType::Valued<double>&) {
			return make_typename(NodeStructs::BaseTypename{ "Floating" }, NodeStructs::Value{}, rule_info_language_element("Floating"));
		},
		[&](const NodeStructs::PrimitiveType::Valued<int>&) {
			return make_typename(NodeStructs::BaseTypename{ "Int" }, NodeStructs::Value{}, rule_info_language_element("Int"));
		},
		[&](const NodeStructs::PrimitiveType::Valued<bool>&) {
			return make_typename(NodeStructs::BaseTypename{ "Bool" }, NodeStructs::Value{}, rule_info_language_element("Bool"));
		},
		[&](const NodeStructs::PrimitiveType::Valued<NodeStructs::void_t>&) {
			return make_typename(NodeStructs::BaseTypename{ "Void" }, NodeStructs::Value{}, rule_info_language_element("Void"));
		},
		[&](const NodeStructs::PrimitiveType::Valued<NodeStructs::empty_optional_t>&) {
			return make_typename(NodeStructs::BaseTypename{ "None" }, NodeStructs::Value{}, rule_info_language_element("None"));
		},
		[&](const NodeStructs::PrimitiveType::Valued<char>&) {
			return make_typename(NodeStructs::BaseTypename{ "Char" }, NodeStructs::Value{}, rule_info_language_element("Char"));
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
	transpilation_state_with_indent state,
	variables_t& variables,
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
				std::vector<Arrangement> res = arrangements(state, variables, args, current, arg_index, param_index + 1); // skipping variadic param
				// try to match all args to the variadic, each time aggregating the results
				// do not match and skip, as the next call attempts to skip anyway
				while (arg_index < args.size()) {
					current.arg_placements.push_back(param_index);
					++arg_index;
					auto _arrangements = arrangements(state, variables, args, current, arg_index, param_index);
					res.insert(res.end(), _arrangements.begin(), _arrangements.end());
				}
				return res;
			},
			[&](const NodeStructs::TemplateParameter& non_variadic) -> std::vector<Arrangement> {
				current.arg_placements.push_back(param_index);
				return arrangements(state, variables, args, std::move(current), arg_index + 1, param_index + 1);
			},
			[&](const NodeStructs::TemplateParameterWithDefaultValue& non_variadic) -> std::vector<Arrangement> {
				if constexpr (std::is_same_v<T, NodeStructs::Expression>) {
					if (cmp(non_variadic.value, args.at(arg_index)) == std::strong_ordering::equivalent) {
						current.arg_placements.push_back(param_index);
						return arrangements(args, std::move(current), arg_index + 1, param_index + 1);
					}
					else
						return {};
				}
				else {
					auto a = word_typename_or_expression_for_template(state, variables, non_variadic.value);
					if (a.has_error())
						return {};
					auto b = word_typename_or_expression_for_template(state, variables, args.at(arg_index));
					if (b.has_error())
						return {};
					if (a.value() == b.value()) {
						current.arg_placements.push_back(param_index);
						return arrangements(state, variables, args, std::move(current), arg_index + 1, param_index + 1);
					}
					else
						return {};
				}
			}
		), current.tmpl.get().parameters.at(param_index)._value);
}

template <typename T>
static std::vector<Arrangement> arrangements(
	transpilation_state_with_indent state,
	variables_t& variables,
	const std::vector<T>& args,
	const NodeStructs::Template& tmpl
) {
	return arrangements(state, variables, args, Arrangement{ tmpl, {} }, 0, 0);
}

template <typename T>
static std::vector<Arrangement> arrangements(
	transpilation_state_with_indent state,
	variables_t& variables,
	const std::vector<T>& args,
	const std::vector<const NodeStructs::Template*>& templates
) {
	std::vector<Arrangement> res;
	for (const NodeStructs::Template* tmpl : templates) {
		auto _arrangements = arrangements(state, variables, args, *tmpl);
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
	else if (first_arrangement_has_at_least_one_reason_to_be_preferred_over_second(args, first, second))
		return true;
	else
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

expected<Arrangement> find_best_template(
	transpilation_state_with_indent state,
	variables_t& variables,
	const std::vector<NodeStructs::Template>& templates,
	const std::vector<NodeStructs::WordTypenameOrExpression>& args
) {
	std::vector<const NodeStructs::Template*> candidates = matching_size_candidates(args.size(), templates);
	if (candidates.size() == 0)
		NOT_IMPLEMENTED;
	std::vector<Arrangement> candidate_arrangements = arrangements(state, variables, args, candidates);
	
	std::sort(
		candidate_arrangements.begin(),
		candidate_arrangements.end(),
		[](const Arrangement& l, const Arrangement& r) {
			return (&l.tmpl.get() < &r.tmpl.get())
				|| ((&l.tmpl.get() == &r.tmpl.get())
					&& (cmp(l.arg_placements, r.arg_placements) == std::strong_ordering::less));
		}
	);
	candidate_arrangements.erase(
		std::unique(
			candidate_arrangements.begin(),
			candidate_arrangements.end(),
			[](const Arrangement& l, const Arrangement& r) {
				return &l.tmpl.get() == &r.tmpl.get() && cmp(l.arg_placements, r.arg_placements) == std::strong_ordering::equivalent;
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
		auto a = word_typename_or_expression_for_template(state, variables, arg);
		return_if_error(a);
		args_ss << a.value();
	}
	std::stringstream templates_ss;
	bool has_prev_templates = false;
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
			parameters_ss << std::visit(overload(
				[](const NodeStructs::TemplateParameter& p) -> std::string {
					return p.name;
				},
				[](const NodeStructs::TemplateParameterWithDefaultValue& p) -> std::string {
					return p.name + " = `" + original_representation(p.value) + "`";
				},
				[](const NodeStructs::VariadicTemplateParameter& p) -> std::string {
					return p.name + "...";
				}
			), parameter._value);
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

static expected<std::optional<const NodeStructs::Function*>> find_best_function100(
	transpilation_state_with_indent state,
	variables_t& variables,
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
			auto f_tn = type_of_typename(state, variables, fn.parameters.at(i).typename_);
			return_if_error(f_tn);
			if (!std::holds_alternative<directly_assignable>(assigned_to(state, variables, f_tn.value(), arg_types.at(i))._value))
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
	variables_t& variables,
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
			NOT_IMPLEMENTED;

		auto fn_ok = std::move(fn).value();
		// TODO VERIFY
		if (!state.state.traversed_functions.contains(fn_ok)) {
			state.state.traversed_functions.insert(copy(fn_ok));
			auto transpiled_f = transpile(state.unindented(), fn_ok);
			return_if_error(transpiled_f);
			if (uses_auto(fn_ok))
				NOT_IMPLEMENTED;
			state.state.functions_to_transpile.insert(copy(fn_ok));
		}

		bool failed = false;
		for (size_t i = 0; i < size; ++i) {
			auto f_tn = type_of_typename(state, variables, fn_ok.parameters.at(i).typename_);
			if (f_tn.has_error()) {
				failed = true;
				continue;
			}
			if (!std::holds_alternative<directly_assignable>(assigned_to(state, variables, f_tn.value(), arg_types.at(i))._value))
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
	variables_t& variables,
	const std::vector<NodeStructs::MetaType>& arg_types,
	const std::vector<NodeStructs::Template>& templates
) {
	NOT_IMPLEMENTED;
}

static expected<std::optional<const NodeStructs::Function*>> find_best_function110(
	transpilation_state_with_indent state,
	variables_t& variables,
	const std::vector<NodeStructs::MetaType>& arg_types,
	const std::vector<NodeStructs::Function>& fs,
	const std::vector<NodeStructs::Function>& auto_fs
) {
	auto e_f = find_best_function100(state, variables, arg_types, fs);
	if (e_f.has_error() || e_f.value().has_value())
		return e_f;
	return find_best_function010(state, variables, arg_types, auto_fs);
}

static expected<std::optional<const NodeStructs::Function*>> find_best_function101(
	transpilation_state_with_indent state,
	variables_t& variables,
	const std::vector<NodeStructs::MetaType>& arg_types,
	const std::vector<NodeStructs::Function>& fs,
	const std::vector<NodeStructs::Template>& templates
) {
	NOT_IMPLEMENTED;
}

static expected<std::optional<const NodeStructs::Function*>> find_best_function011(
	transpilation_state_with_indent state,
	variables_t& variables,
	const std::vector<NodeStructs::MetaType>& arg_types,
	const std::vector<NodeStructs::Function>& auto_fs,
	const std::vector<NodeStructs::Template>& templates
) {
	NOT_IMPLEMENTED;
}

static expected<std::optional<const NodeStructs::Function*>> find_best_function111(
	transpilation_state_with_indent state,
	variables_t& variables,
	const std::vector<NodeStructs::MetaType>& arg_types,
	const std::vector<NodeStructs::Function>& fs,
	const std::vector<NodeStructs::Function>& auto_fs,
	const std::vector<NodeStructs::Template>& templates
) {
	NOT_IMPLEMENTED;
}

expected<std::optional<const NodeStructs::Function*>> find_best_function(
	transpilation_state_with_indent state,
	variables_t& variables,
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
				return find_best_function111(state, variables, arg_types, fs->second, auto_fs->second, templates->second);
			else
				return find_best_function110(state, variables, arg_types, fs->second, auto_fs->second);
		else
			if (templates != space.templates.end())
				return find_best_function101(state, variables, arg_types, fs->second, templates->second);
			else
				return find_best_function100(state, variables, arg_types, fs->second);
	else
		if (auto_fs != space.functions_using_auto.end())
			if (templates != space.templates.end())
				return find_best_function011(state, variables, arg_types, auto_fs->second, templates->second);
			else
				return find_best_function010(state, variables, arg_types, auto_fs->second);
		else
			if (templates != space.templates.end())
				return find_best_function001(state, variables, arg_types, templates->second);
			else
				NOT_IMPLEMENTED;
}

std::string replace_for_template(std::string in) {
	return replace_all(std::move(in), "<", "_", ">", "_", " ", "", ",", "_");
}

expected<std::string> word_typename_or_expression_for_template(
	transpilation_state_with_indent state,
	variables_t& variables,
	const NodeStructs::WordTypenameOrExpression& value
) {
	// todo no way this works without recursion, it has to add aliases for inner expressions/typenames
	auto base_or_e = std::visit(overload(
		[&](const std::string& s) -> expected<std::string> {
			auto test = transpile_expression(state, variables, s);
			if (test.has_value()) {
				if (!std::holds_alternative<type_information>(test.value()))
					NOT_IMPLEMENTED;
				return std::get<type_information>(test.value()).representation;
			}
			else {
				auto test2 = transpile_typename(state, variables, NodeStructs::BaseTypename{ s });
				return test2;
			}
		},
		[&](const NodeStructs::Expression& e) -> expected<std::string> {
			if (holds<NodeStructs::TemplateExpression>(e)) {
				const auto& tmpl = get<NodeStructs::TemplateExpression>(e);
				auto operand_or_e = transpile_expression(state, variables, tmpl.operand);
				return_if_error(operand_or_e);
				if (!std::holds_alternative<type_information>(operand_or_e.value()))
					NOT_IMPLEMENTED;
				auto vec_or_e = vec_of_expected_to_expected_of_vec(
					tmpl.args
					| std::views::transform([&](const auto& x) { return word_typename_or_expression_for_template(state, variables, x); })
					| to_vec());
				return_if_error(vec_or_e);
				std::stringstream ss;
				bool requires_alias =
					std::get<type_information>(operand_or_e.value()).representation == "Vector" ||
					std::get<type_information>(operand_or_e.value()).representation == "Map" ||
					std::get<type_information>(operand_or_e.value()).representation == "Set" ||
					std::get<type_information>(operand_or_e.value()).representation == "Union"
					;
				ss << replace_for_template(std::move(std::get<type_information>(operand_or_e.value()).representation)) << "<";
				bool has_prev = false;
				for (const auto& arg : vec_or_e.value()) {
					if (has_prev)
						ss << ", ";
					has_prev = true;
					ss << arg;
				}
				ss << ">";
				auto base = ss.str();
				auto out = replace_for_template(base);
				state.state.global_namespace.aliases.insert({
					out,
					NodeStructs::Typename{
						.value = NodeStructs::BaseTypename{ get<std::string>(tmpl.operand.expression.get()) },
						.category = NodeStructs::Value{},
						.info = rule_info{.file_name = "todo:/", .content = get<std::string>(tmpl.operand.expression.get()) }
					}
					});
				if (requires_alias)
					state.state.aliases_to_transpile.insert({ out, base });
				return out;
			}
			else {
				auto t = transpile_expression(state, variables, e);
				return_if_error(t);
				if (!std::holds_alternative<type_information>(t.value()))
					NOT_IMPLEMENTED;
				return std::get<type_information>(t.value()).representation;
			}
		},
		[&](const NodeStructs::Typename& t) -> expected<std::string> {
			return transpile_typename(state, variables, t).value();
		}
	), value.value._value);
	return_if_error(base_or_e);
	auto out = replace_for_template(base_or_e.value());
	return out;
}

expected<NodeStructs::MetaType> type_of_typename(
	transpilation_state_with_indent state,
	variables_t& variables,
	const NodeStructs::WordTypenameOrExpression& tn_or_expr
) {
	return std::visit(overload(
		[&](const NodeStructs::Expression&) -> expected<NodeStructs::MetaType> {
			NOT_IMPLEMENTED;
		},
		[&](const NodeStructs::Typename& tn) -> expected<NodeStructs::MetaType> {
			auto x = type_of_typename(state, variables, tn);
			return_if_error(x);
			return std::move(x).value();
		},
		[&](const std::string& tn) -> expected<NodeStructs::MetaType> {
			auto x = type_of_typename(state, variables, NodeStructs::BaseTypename{ tn });
			return_if_error(x);
			return std::move(x).value();
		}
	), tn_or_expr.value._value);
}
