#include <numeric>

#include "../utility/overload.hpp"
#include "../utility/vec_of_expected_to_expected_of_vec.hpp"
#include "../utility/vec_of_variant_to_optional_vector_single_type.hpp"
#include "../utility/replace_all.hpp"

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

	add(name_space, std::move(_builtins.builtin_compile_time_error));
	add(name_space, std::move(_builtins.builtin_type_list));


	add(name_space, std::move(_builtins.builtin_tuple));
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
		Namespace ns{};
		insert_all_named_recursive_with_imports(ns, e);
		named.namespaces[e.name] = std::move(ns);
	}

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

void mark_exists_as_traversed(transpilation_state& state, const NodeStructs::NameSpace& exists, std::stringstream& ss) {
	for (const auto& e : exists.types) {
		if (e.name_space.has_value()) {
			auto str_or_e = transpile_typename({ state }, e.name_space.value());
			if (str_or_e.has_error())
				throw;
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
				auto str_or_e = transpile_typename({ state }, e.name_space.value());
				if (str_or_e.has_error())
					throw;
				ss << "using " << str_or_e.value() << "__" << e.name << " = ";
				std::string aliases_to = replace_all(std::move(str_or_e).value(), "__", "::");
				ss << aliases_to << "::" << e.name << "<Ts...>;\n";
			}
		}
		else { // assume function
			if (e.name_space.has_value()) {
				auto str_or_e = transpile_typename({ state }, e.name_space.value());
				if (str_or_e.has_error())
					throw;
				ss << "decltype(auto) " << str_or_e.value() << "__" << e.name << "(auto&&... args) { return ";
				std::string aliases_to = replace_all(std::move(str_or_e).value(), "__", "::");
				ss << aliases_to << "::" << e.name << "(std::forward<decltype(args)...>(args...)); }\n";
			}
		}
	}

	for (const auto& e : exists.functions)
		state.traversed_functions.insert(copy(e));

	for (const auto& e : exists.functions_using_auto)
		throw;

	for (const auto& e : exists.interfaces)
		state.traversed_interfaces.insert(copy(e));

	for (const auto& e : exists.blocks)
		throw;

	for (const auto& e : exists.enums)
		throw;
	
	for (const auto& e : exists.namespaces)
		mark_exists_as_traversed(state, e, ss);
}

void mark_exists_as_traversed(transpilation_state& state, const NodeStructs::Exists& exists, std::stringstream& ss) {
	mark_exists_as_traversed(state, exists.global_exists, ss);
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

				std::stringstream exists_aliases;

				for (const auto& file : project)
					for (const auto& exists : file.exists)
						mark_exists_as_traversed(state, exists, exists_aliases);

				transpile_declaration_definition_t main_transpilation_result = transpile_main({ state, 0 }, fn);
				return_if_error(main_transpilation_result);

				std::stringstream cpp;
				cpp << "#include \"defaults.hpp\"\n\n" << exists_aliases.str();

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
						size_t val = 0;
						std::string enum_prefix = enum_.name + "__";
						for (const auto& enum_val : enum_.values)
							declarations << "static constexpr Int " << enum_prefix << enum_val << " = " << val++ << ";\n";
					}
					for (const auto& i : state.interfaces_to_transpile) {
						if (!state.traversed_interfaces.contains(i))
							throw;
						if (i.name_space.has_value())
							throw;
						const auto& members = state.interface_symbol_to_members.at({ NodeStructs::BaseTypename{ i.name }, NodeStructs::Value{} });
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
							throw;
						if (uses_auto(f))
							throw;
						auto res = transpile({ state, 0 }, f);
						return_if_error(res);
						declarations << res.value().first;
						definitions << res.value().second;
					}
					if (size != state.functions_to_transpile.size()) {
						puts("size != state.functions_to_transpile.size()");
						for (const auto& f : state.functions_to_transpile)
							if (std::find_if(v.begin(), v.end(), [&](const auto& e) { return cmp(f, e) == std::weak_ordering::equivalent; }) == v.end())
								puts(f.name.c_str());
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
	auto vector_str = NodeStructs::Typename{ NodeStructs::TemplatedTypename{
		NodeStructs::Typename{ NodeStructs::BaseTypename{ "Vector" }, NodeStructs::Value{} },
		as_vec(NodeStructs::WordTypenameOrExpression{ "String" })
	}, NodeStructs::Reference{} };
	if (cmp(type, vector_str) != std::weak_ordering::equivalent)
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
		auto t = type_of_typename(state, type_name);
		return_if_error(t);
	}
	for (auto&& [type_name, name] : parameters) {
		const auto& cat = type_name.category;
		if (!cat._value.has_value())
			throw;
		auto t = type_of_typename(state, type_name);
		if (t.has_error())
			throw;
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
				{ NodeStructs::PrimitiveType{ true } }
			}
		);
		variables["False"].push_back(
			{
				NodeStructs::Value{},
				{ NodeStructs::PrimitiveType{ false } }
			}
		);
		variables["None"].push_back(
			{
				NodeStructs::Value{},
				{ NodeStructs::PrimitiveType{ NodeStructs::empty_optional_t{} } }
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
		auto declaration = common + ";\n";
		auto definition = common + " {\n" + statements.value() + "};\n";
		return std::pair{ declaration, definition };
	}
	else {
		auto common = return_typename.value() + " " + fn.name + "(" + parameters.value() + ")";
		auto declaration = common + ";\n";
		auto definition = common + " {\n" + statements.value() + "};\n";
		return std::pair{ declaration, definition };
	}
}

transpile_declaration_definition_t transpile(
	transpilation_state_with_indent state,
	const NodeStructs::Type& type
) {
	std::stringstream h, cpp;
	if (type.name_space.has_value()) {
		auto ns_repr_or_e = transpile_typename(state, type.name_space.value());
		return_if_error(ns_repr_or_e);
		h << "struct " << ns_repr_or_e.value() << "__" << type.name << ";\n";
		cpp << "struct " << ns_repr_or_e.value() << "__" << type.name << " {\n";
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
	return std::pair{ h.str(), cpp.str() };
}

transpile_declaration_definition_t transpile(
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
	for (const auto& [type, name] : parameters) {
		const auto& cat = type.category;
		if (!cat._value.has_value())
			throw;
		auto tn = transpile_typename(state, type);
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

transpile_t transpile(
	transpilation_state_with_indent state,
	variables_t& variables,
	const std::vector<NodeStructs::Statement>& statements,
	const NodeStructs::MetaType& expected_return_type
) {
	std::stringstream ss;
	for (const NodeStructs::Statement& statement : statements) {
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
				if (cmp(it_type, iterator_type) != std::weak_ordering::equivalent) {
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

transpile_expression_information_t transpile_arg(
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

std::string template_name(std::string original_name, const std::vector<NodeStructs::WordTypenameOrExpression>& arguments) {
	return template_name(original_name, arguments
		| std::views::transform([&](auto&& e) { return word_typename_or_expression_for_template(e); })
		| to_vec());
}

Variant<not_assignable, directly_assignable, requires_conversion> assigned_to(
	transpilation_state_with_indent state_,
	const NodeStructs::MetaType& parameter,
	const NodeStructs::MetaType& argument
) {
	using R = Variant<not_assignable, directly_assignable, requires_conversion>;

	if (std::holds_alternative<NodeStructs::CompileTimeType>(argument.type._value)) {
		const auto& arg = std::get<NodeStructs::CompileTimeType>(argument.type._value);
		auto res = assigned_to(state_, parameter, arg.type.get());

		if (std::holds_alternative<NodeStructs::PrimitiveType>(arg.type.get().type._value)) {
			const auto& x = std::get<NodeStructs::PrimitiveType>(arg.type.get().type._value);
			return requires_conversion{
				[&](transpilation_state_with_indent, variables_t&, const NodeStructs::Expression&) -> transpile_expression_information_t {
					std::string representation = [&]() {
						std::stringstream ss;
						std::visit(overload(
							[&](const auto& val) { ss << val; },
							[&](const NodeStructs::void_t& val) { ss << "void_t{}"; },
							[&](const NodeStructs::empty_optional_t& val) { ss << "empty_optional_t{}"; }
						), x.value._value);
						return ss.str();
					}();
					return non_type_information{ .type = copy(x), .representation = std::move(representation), .value_category = NodeStructs::Value{} };
				}
			};
		}
		throw;
	}
	return std::visit(
		overload(
			[&](const auto& param, const auto& arg, const auto& state) -> R {
				throw;
			},
			[&](const NodeStructs::OptionalType& e, const auto& u, const auto& state) -> R {
				return assigned_to(state_, e.value_type, { copy(u) });
			},
			[&](const NodeStructs::OptionalType& e, const NodeStructs::PrimitiveType& u, const auto& state) -> R {
				if (std::holds_alternative<NodeStructs::empty_optional_t>(u.value._value))
					return directly_assignable{};
				return assigned_to(state_, e.value_type, { copy(u) });
			},
			[&](const NodeStructs::TupleType& e, const NodeStructs::TupleType& u, const auto& state) -> R {
				throw;
			},
			[&](const NodeStructs::TupleType& e, const NodeStructs::AggregateType& u, const auto& state) -> R {
				auto tn_or_e = typename_of_type(state, e);
				if (tn_or_e.has_error())
					throw;
				auto name_or_e = transpile_typename(state, tn_or_e.value());
				if (name_or_e.has_error())
					throw;

				if (e.arguments.size() != u.arguments.size())
					return not_assignable{};
				std::vector<R> assign_ts;
				assign_ts.reserve(e.arguments.size());
				bool is_directly_assignable = true;
				for (size_t i = 0; i < e.arguments.size(); ++i) {
					auto assign_t = assigned_to(state, e.arguments.at(i), u.arg_types.at(i))._value;
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
						[T = copy(e), s = name_or_e.value(), assign_ts = std::move(assign_ts)]
						(transpilation_state_with_indent state, variables_t& variables, const NodeStructs::Expression& expr) -> transpile_expression_information_t {
							if (!std::holds_alternative<NodeStructs::BraceArguments>(expr.expression.get()._value))
								throw;
							const auto& assumed_aggregate = std::get<NodeStructs::BraceArguments>(expr.expression.get()._value);
							std::stringstream ss;
							std::vector<R>& non_const_assign_ts = const_cast<std::vector<R>&>(assign_ts);
							for (int i = 0; i < assign_ts.size(); ++i) {
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
											throw;
										const auto& expr_info_ok = std::get<non_type_information>(expr_info.value());
										ss << expr_info_ok.representation << ", ";
									},
									[](not_assignable&) -> transpile_expression_information_t {
										throw;
									}
								), assign_t._value);
							}
							return expression_information{ non_type_information{
								.type = { std::move(const_cast<decltype(T)&>(T))},
								.representation = s + "{" + ss.str() + "}",
								.value_category = NodeStructs::Value{}
							} };
						}
					};
				}
			},
			[&](const NodeStructs::TupleType& e, const NodeStructs::Type& u, const auto& state) -> R {
				throw;
			},
			[&](const NodeStructs::Type& e, const NodeStructs::AggregateType& u, const auto& state) -> R {
				if (e.member_variables.size() != u.arguments.size())
					return not_assignable{};
				std::vector<R> assign_ts;
				assign_ts.reserve(e.member_variables.size());
				bool is_directly_assignable = true;
				for (size_t i = 0; i < e.member_variables.size(); ++i) {
					auto mem_t = type_of_typename(state, e.member_variables.at(i).type);
					if (mem_t.has_error())
						throw;
					auto assign_t = assigned_to(state, mem_t.value(), u.arg_types.at(i))._value;
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
						[T = copy(e), s = e.name, assign_ts = std::move(assign_ts)]
						(transpilation_state_with_indent state, variables_t& variables, const NodeStructs::Expression& expr) -> transpile_expression_information_t {
							if (!std::holds_alternative<NodeStructs::BraceArguments>(expr.expression.get()._value))
								throw;
							const auto& assumed_aggregate = std::get<NodeStructs::BraceArguments>(expr.expression.get()._value);
							std::stringstream ss;
							std::vector<R>& non_const_assign_ts = const_cast<std::vector<R>&>(assign_ts);
							for (int i = 0; i < assign_ts.size(); ++i) {
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
											throw;
										const auto& expr_info_ok = std::get<non_type_information>(expr_info.value());
										ss << expr_info_ok.representation << ", ";
									},
									[](not_assignable&) -> transpile_expression_information_t {
										throw;
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
			[&](const NodeStructs::PrimitiveType& e, const NodeStructs::PrimitiveType& u, const auto& state) -> R {
				if (std::holds_alternative<std::string>(e.value._value) && std::holds_alternative<char>(u.value._value))
				return requires_conversion{
					[](transpilation_state_with_indent state, variables_t& variables, const NodeStructs::Expression& expr) -> transpile_expression_information_t {
						transpile_expression_information_t expr_info = transpile_expression(state, variables, expr);
						return_if_error(expr_info);
						if (!std::holds_alternative<non_type_information>(expr_info.value()))
							throw;
						non_type_information& expr_info_ok = std::get<non_type_information>(expr_info.value());
						return expression_information{ non_type_information{
							.type = NodeStructs::PrimitiveType{ std::string{} },
							.representation = "String{" + expr_info_ok.representation + "}",
							.value_category = std::move(expr_info_ok).value_category
						} };
					}
				};
				if (e.value._value.index() == u.value._value.index())
					return directly_assignable{};
				else
					return not_assignable{};
			},
			[&](const NodeStructs::EnumType& e, const NodeStructs::EnumValueType& u, const auto& state) -> R {
				if (cmp(e.enum_.get(), u.enum_.get()) == std::weak_ordering::equivalent)
					return directly_assignable{};
				else
					return not_assignable{};
			},
			[&](const NodeStructs::Builtin& e, const NodeStructs::Builtin& u, const auto& state) -> R {
				if (e.name == u.name)
					return directly_assignable{};
				else
					return not_assignable{};
			},
			[&](const NodeStructs::Builtin& e, const auto& u, const auto& state) -> R {
				if (e.name == "builtin_filesystem_directory") {
					return assigned_to(state, NodeStructs::MetaType{ NodeStructs::PrimitiveType{ { std::string{} } } }, { copy(u) });
				}
				throw;
			},
			[&](const NodeStructs::UnionType& e, const auto& u, const auto& state) -> R {
				if constexpr (std::is_same_v<std::remove_cvref_t<decltype(u)>, NodeStructs::UnionType>) {
					// check each union type of u is assignable into e
					for (const auto& arg : u.arguments) {
						auto arg_assigned_to = assigned_to(state, parameter, arg);
						if (!std::holds_alternative<directly_assignable>(arg_assigned_to._value))
							return not_assignable{};
					}
					return directly_assignable{};
				}
				else {
					for (const auto& vt : e.arguments) {
						//auto union_arg_assignable = std::visit([&](const auto& t) -> R { return assigned_to(state, { copy(t) }, { copy(u) }); }, vt.type._value);
						auto union_arg_assignable = assigned_to(state, vt, argument);
						if (std::holds_alternative<directly_assignable>(union_arg_assignable._value))
							return directly_assignable{};
					}
					return not_assignable{};
				}
			},
			[&](const NodeStructs::VectorType& e, const NodeStructs::VectorType& u, const auto& state) -> R {
				if (cmp(e.value_type, u.value_type) == std::weak_ordering::equivalent)
					return directly_assignable{};
				else
					return not_assignable{};
			},
			[&](const NodeStructs::SetType& e, const NodeStructs::SetType& u, const auto& state) -> R {
				if (cmp(e.value_type, u.value_type) == std::weak_ordering::equivalent)
					return directly_assignable{};
				else
					return not_assignable{};
			},
			[&](const NodeStructs::InterfaceType& e, const NodeStructs::Type& u, const auto& state) -> R {
				for (const auto& [type_name, name] : e.interface.get().member_variables) {
					auto t = type_of_typename(state, type_name);
					if (t.has_error())
						throw;
					for (const auto& [source_type_name, source_name] : u.member_variables)
						if (name == source_name) {
							auto t2 = type_of_typename(state, source_type_name);
							if (t2.has_error())
								throw;
							if (cmp(t.value(), t2.value()) != std::weak_ordering::equivalent)
								return not_assignable{};
						}
				}
				auto& interfacemembers = state.state.interface_symbol_to_members[
					NodeStructs::Typename{ NodeStructs::BaseTypename{ e.interface.get().name }, NodeStructs::Value{} }
				];
				auto new_member = NodeStructs::MetaType{ copy(u) };
				for (const auto& member : interfacemembers)
					if (cmp(member, new_member) == std::weak_ordering::equivalent)
						return directly_assignable{};
				interfacemembers.push_back(std::move(new_member));
				return directly_assignable{};
			},
			[&](const NodeStructs::Type& e, const NodeStructs::Type& u, const auto& state) -> R {
				if (cmp(e, u) == std::weak_ordering::equivalent)
					return directly_assignable{};
				// if the parameter has 1 member, try to assign to that member
				if (e.member_variables.size() == 1) {
					auto member_variable_t = type_of_typename(state, e.member_variables.at(0).type);
					if (member_variable_t.has_error())
						throw;
					// in the event where assignment can work, we still need a `requires_conversion`
					R mem_assigned = assigned_to(state, member_variable_t.value(), argument);
					if (std::holds_alternative<directly_assignable>(mem_assigned._value)) {
						return requires_conversion{
							[s = e.name](transpilation_state_with_indent state, variables_t& variables, const NodeStructs::Expression& expr) -> transpile_expression_information_t {
								transpile_expression_information_t expr_info = transpile_expression(state, variables, expr);
								return_if_error(expr_info);
								if (!std::holds_alternative<non_type_information>(expr_info.value()))
									throw;
								non_type_information& expr_info_ok = std::get<non_type_information>(expr_info.value());
								return expression_information{ non_type_information{
									.type = NodeStructs::PrimitiveType{ std::string{} },
									.representation = s + "{" + expr_info_ok.representation + "}",
									.value_category = std::move(expr_info_ok).value_category
								} };
							}
						};
					}
					else if (std::holds_alternative<requires_conversion>(mem_assigned._value)) {
						return requires_conversion{
							[s = e.name, mem_assigned = std::get<requires_conversion>(std::move(mem_assigned)._value)]
							(transpilation_state_with_indent state, variables_t& variables, const NodeStructs::Expression& expr) -> transpile_expression_information_t {
								requires_conversion& rc = const_cast<requires_conversion&>(mem_assigned);
								transpile_expression_information_t expr_info = rc.converter(state, variables, expr);
								return_if_error(expr_info);
								if (!std::holds_alternative<non_type_information>(expr_info.value()))
									throw;
								non_type_information& expr_info_ok = std::get<non_type_information>(expr_info.value());
								return expression_information{ non_type_information{
									.type = NodeStructs::PrimitiveType{ std::string{} },
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

	if (std::holds_alternative<NodeStructs::Type>(t_ok.type.type._value)) {
		std::stringstream ss;
		const auto& t_ref = std::get<NodeStructs::Type>(t_ok.type.type._value);
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
	else if (std::holds_alternative<NodeStructs::InterfaceType>(t_ok.type.type._value)) {
		const auto& t_ref = std::get<NodeStructs::InterfaceType>(t_ok.type.type._value).interface.get();
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
	else if (std::holds_alternative<NodeStructs::PrimitiveType>(t_ok.type.type._value)) {
		const auto& primitive_type = std::get<NodeStructs::PrimitiveType>(t_ok.type.type._value);
		auto expr_repr = transpile_expression(state, variables, expr);
		return_if_error(expr_repr);
		if (!std::holds_alternative<non_type_information>(expr_repr.value()))
			throw;
		const auto& expr_repr_ok = std::get<non_type_information>(expr_repr.value());
		return std::visit(
			overload(
				[&](const std::string&) -> std::string {
					return "('\"' + " + expr_repr_ok.representation + " + '\"')";
				},
				[&](const char&) -> std::string {
					// not optimal
					return "(String(\"'\") + " + expr_repr_ok.representation + " + String(\"'\"))";
				},
				[&](const int&) -> std::string {
					return expr_repr_ok.representation;
				},
				[&](const double&) -> std::string {
					return expr_repr_ok.representation;
				},
				[&](const bool&) -> std::string {
					return "String{" + expr_repr_ok.representation + " ? \"True\" : \"False\"}";
				},
				[&](const NodeStructs::void_t&) -> std::string {
					return "String(\"Void\")";
				},
				[&](const NodeStructs::empty_optional_t&) -> std::string {
					return "String(\"None\")";
				}
			),
			primitive_type.value._value
		);
	}
	else {
		throw;
	}
}

bool uses_auto(const NodeStructs::Function& fn) {
	if (cmp(fn.returnType.value, NodeStructs::Typename{ NodeStructs::BaseTypename{ "auto" }, std::nullopt }.value) == std::weak_ordering::equivalent)
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
	return t.type == "auto";
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
		if (std::holds_alternative<NodeStructs::Typename>(param.value._value)
			&& uses_auto(std::get<NodeStructs::Typename>(param.value._value)))
			return true;
		else if (std::holds_alternative<NodeStructs::Expression>(param.value._value)
				&& uses_auto(std::get<NodeStructs::Expression>(param.value._value)))
			return true;
	return false;
}

static bool uses_auto(const NodeStructs::Expression& t) {
	throw;
}

static bool uses_auto(const NodeStructs::UnionTypename& t) {
	for (const auto& param : t.ors)
		if (uses_auto(param))
			return true;
	return false;
}

static bool uses_auto(const NodeStructs::TupleTypename& t) {
	for (const auto& param : t.members)
		if (uses_auto(param))
			return true;
	return false;
}

static bool uses_auto(const NodeStructs::OptionalTypename& t) {
	return uses_auto(t.type.get());
}

static bool uses_auto(const NodeStructs::VariadicExpansionTypename& t) {
	auto res = uses_auto(t.type.get());
	if (res)
		throw;
	return res;
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
	bool is_compile_time = statement.is_compile_time;
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
				return std::get<non_type_information>(std::move(res_info).value()).type;
			},
			[&](const NodeStructs::Expression& statement) -> expected<std::optional<NodeStructs::MetaType>> {
				return std::nullopt;
			},
			[&](const NodeStructs::VariableDeclarationStatement& statement) -> expected<std::optional<NodeStructs::MetaType>> {
				auto t = transpile_statement_visitor{ {},
					state,
					variables,
					NodeStructs::MetaType{ NodeStructs::PrimitiveType{ NodeStructs::void_t{} } },
					is_compile_time
				}(statement);
				return_if_error(t);
				throw;
			},
			[&](const NodeStructs::IfStatement& statement) -> expected<std::optional<NodeStructs::MetaType>> {
				auto if_res_t = deduce_return_type(state, variables, statement.ifStatements);
				return_if_error(if_res_t);

				if (!statement.elseExprStatements.has_value())
					return if_res_t;

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
						return if_res_t;
				if (if_res_t.value().has_value())
					return if_res_t;
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
				auto it_type = iterator_type(state, coll_type_or_e_ok.type);
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
			[&](const NodeStructs::Assignment& statement) -> expected<std::optional<NodeStructs::MetaType>> {
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
			if (cmp(param.typename_.value, NodeStructs::Typename::Variant_{ NodeStructs::BaseTypename{ "auto" } }) == std::weak_ordering::equivalent) {
				auto tn = typename_of_type(state, arg_types.at(index));
				return_if_error(tn);
				auto del = std::move(realised.parameters.at(index).typename_);
				new (&realised.parameters.at(index).typename_) NodeStructs::Typename{ std::move(tn).value().value, copy(param.typename_.category) };
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

	if (cmp(realised.returnType.value, NodeStructs::Typename{ NodeStructs::BaseTypename{ "auto" }, std::nullopt }.value) != std::weak_ordering::equivalent) {
		auto fn_or_error = transpile(state, realised);
		return_if_error(fn_or_error);
		return NodeStructs::Function{
			.name = realised.name,
			.name_space = std::nullopt,
			.returnType = std::move(realised.returnType),
			.parameters = std::move(realised.parameters),
			.statements = std::move(realised.statements)
		};
	} else {
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
					.returnType = NodeStructs::Typename{ NodeStructs::BaseTypename{ "Void" }, std::nullopt },
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
		[&](const std::string&) {
			return NodeStructs::Typename{ NodeStructs::BaseTypename{ "String" }, NodeStructs::Value{} };
		},
		[&](const double&) {
			return NodeStructs::Typename{ NodeStructs::BaseTypename{ "Floating" }, NodeStructs::Value{} };
		},
		[&](const int&) {
			return NodeStructs::Typename{ NodeStructs::BaseTypename{ "Int" }, NodeStructs::Value{} };
		},
		[&](const bool&) {
			return NodeStructs::Typename{ NodeStructs::BaseTypename{ "Bool" }, NodeStructs::Value{} };
		},
		[&](const NodeStructs::void_t&) {
			return NodeStructs::Typename{ NodeStructs::BaseTypename{ "Void" }, NodeStructs::Value{} };
		},
		[&](const NodeStructs::empty_optional_t&) {
			return NodeStructs::Typename{ NodeStructs::BaseTypename{ "None" }, NodeStructs::Value{} };
		},
		[&](const char&) {
			return NodeStructs::Typename{ NodeStructs::BaseTypename{ "Char" }, NodeStructs::Value{} };
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
					if (cmp(non_variadic.value, args.at(arg_index)) == std::weak_ordering::equivalent) {
						current.arg_placements.push_back(param_index);
						return arrangements(args, std::move(current), arg_index + 1, param_index + 1);
					}
					else
						return {};
				}
				else {
					if (word_typename_or_expression_for_template(non_variadic.value) == word_typename_or_expression_for_template(args.at(arg_index))) {
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

expected<Arrangement> find_best_template(
	const std::vector<NodeStructs::Template>& templates,
	const std::vector<NodeStructs::WordTypenameOrExpression>& args
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
		args_ss << word_typename_or_expression_for_template(arg);
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
					return p.name + " = `" + expression_original_representation(p.value) + "`";
				},
				[](const NodeStructs::VariadicTemplateParameter& p) -> std::string {
					return p.name + "...";
				}
			), parameter._value);
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
			if (!std::holds_alternative<directly_assignable>(assigned_to(state, f_tn.value(), arg_types.at(i))._value))
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
			auto transpiled_f = transpile(state.unindented(), fn_ok);
			return_if_error(transpiled_f);
			if (uses_auto(fn_ok))
				throw;
			state.state.functions_to_transpile.insert(copy(fn_ok));
		}

		bool failed = false;
		for (size_t i = 0; i < size; ++i) {
			auto f_tn = type_of_typename(state, fn_ok.parameters.at(i).typename_);
			if (f_tn.has_error()) {
				failed = true;
				continue;
			}
			if (!std::holds_alternative<directly_assignable>(assigned_to(state, f_tn.value(), arg_types.at(i))._value))
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

std::string word_typename_or_expression_for_template(const NodeStructs::WordTypenameOrExpression& value) {
	return std::visit(overload(
		[](const std::string& s) {
			return s;
		},
		[](const NodeStructs::Typename& tn) {
			return typename_for_template(tn);
		},
		[](const NodeStructs::Expression& e) {
			return expression_for_template(e);
		}
	), value.value._value);
}

expected<NodeStructs::MetaType> type_of_typename(
	transpilation_state_with_indent state,
	const NodeStructs::WordTypenameOrExpression& tn_or_expr
) {
	return std::visit(overload(
		[&](const NodeStructs::Expression&) -> expected<NodeStructs::MetaType> {
			throw;
		},
		[&](const NodeStructs::Typename& tn) -> expected<NodeStructs::MetaType> {
			auto x = type_of_typename(state, tn);
			return_if_error(x);
			return std::move(x).value();
		},
		[&](const std::string& tn) -> expected<NodeStructs::MetaType> {
			auto x = type_of_typename(state, NodeStructs::BaseTypename{ tn });
			return_if_error(x);
			return std::move(x).value();
		}
	), tn_or_expr.value._value);
}

std::string expression_original_representation(
	const NodeStructs::WordTypenameOrExpression& tn_or_expr
) {
	return std::visit(overload(
		[&](const NodeStructs::Expression& expr) -> std::string {
			return expression_original_representation(expr);
		},
		[&](const NodeStructs::Typename& tn) -> std::string {
			throw;
		},
		[&](const std::string& str) -> std::string {
			return str;
		}
	), tn_or_expr.value._value);
}
