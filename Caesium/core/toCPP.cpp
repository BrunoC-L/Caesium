#include <numeric>

#include "../utility/overload.hpp"
#include "../utility/vec_of_expected_to_expected_of_vec.hpp"
#include "../utility/vec_of_variant_to_optional_vector_single_type.hpp"
#include "../utility/replace_all.hpp"

#include "../structured/structurizer.hpp"

#include "toCPP.hpp"
#include "builtins.hpp"
#include "deduce_return_type.hpp"

template <typename T>
void add_impl(std::vector<T>& v, T e) {
	if (std::find_if(v.begin(), v.end(), [&](const auto& x) { return cmp(x, e) == std::strong_ordering::equivalent; }) != v.end())
		return;
	v.push_back(std::move(e));
}

void add_builtins(transpilation_state& state) {
	//caesium_standard_library_builtins _builtins;

	//add(name_space, std::move(_builtins.entries_dir));
	//add(name_space, std::move(_builtins.entries_str));



	//add(name_space, Realised::Builtin{ Realised::Builtin::builtin_compile_time_error{} });
	//add(name_space, Realised::Builtin{ Realised::Builtin::builtin_typeof{} });
	//add(name_space, Realised::Builtin{ Realised::Builtin::builtin_type_list{} });
	//add(name_space, Realised::Builtin{ Realised::Builtin::builtin_exit{} });

	//add(name_space, Realised::Builtin{ Realised::Builtin::builtin_vector{} });
	//add(name_space, Realised::Builtin{ Realised::Builtin::builtin_set{} });
	//add(name_space, Realised::Builtin{ Realised::Builtin::builtin_map{} });
	//add(name_space, Realised::Builtin{ Realised::Builtin::builtin_union{} });

	//// these are done wrong
	//add(name_space, Realised::Builtin{ Realised::Builtin::builtin_print{} });
	//add(name_space, Realised::Builtin{ Realised::Builtin::builtin_println{} });
	//add(name_space, Realised::Builtin{ Realised::Builtin::builtin_file{} });
	//add(name_space, Realised::Builtin{ Realised::Builtin::builtin_directory{} });
	//add(name_space, Realised::Builtin{ Realised::Builtin::builtin_push{} });
	//add(name_space, Realised::Builtin{ Realised::Builtin::builtin_size{} });
	//add(name_space, Realised::Builtin{ Realised::Builtin::builtin_insert{} });
}

static void traverse_builtins(transpilation_state& state) {
	caesium_standard_library_builtins _builtins;

	/*state.functions_traversal.traversed.insert({ _builtins.realised_entries_str.name._value, std::move(_builtins.realised_entries_str) });
	state.functions_traversal.traversed.insert({ _builtins.realised_entries_dir.name._value, std::move(_builtins.realised_entries_dir) });*/
}

std::optional<error> insert_all_named_recursive_with_imports(
	NodeStructs::NameSpace& named,
	const NodeStructs::NameSpace& to_insert
) {
	for (const auto& e : to_insert.types)
		named.types.push_back(copy(e));

	for (const auto& e : to_insert.functions)
		named.functions.push_back(copy(e));

	for (const auto& e : to_insert.interfaces)
		named.interfaces.push_back(copy(e));

	for (const auto& e : to_insert.blocks)
		named.blocks.push_back(copy(e));

	for (const auto& e : to_insert.templates)
		named.templates.push_back(copy(e));

	for (const auto& e : to_insert.namespaces)
		named.namespaces.push_back(copy(e));

	for (const auto& e : to_insert.enums)
		named.enums.push_back(copy(e));

	for (const auto& e : to_insert.aliases)
		named.aliases.push_back(copy(e));

	return std::nullopt;
}

std::optional<error> insert_all_named_recursive_with_imports(
	const std::vector<NodeStructs::File>& project,
	std::map<std::string, NodeStructs::NameSpace>& named_by_file,
	const std::string& file_name,
	std::set<std::string>& inserted
) {
	if (inserted.contains(file_name))
		return std::nullopt;
	inserted.insert(file_name);
	for (const NodeStructs::File& file : project)
		if (file.content.name == file_name) {
			if (!named_by_file.contains(file_name))
				named_by_file.insert({ file_name, NodeStructs::NameSpace{.info = caesium_source_location{.file_name = "file:/" + file_name, .content = file_name, } } });
			NodeStructs::NameSpace& named = named_by_file.at(file_name);
			{
				auto opt_error = insert_all_named_recursive_with_imports(named, file.content);
				if (opt_error.has_value())
					return opt_error.value();
				for (const auto& exists : file.exists) {
					auto opt_error = insert_all_named_recursive_with_imports(named, exists.global_exists);
					if (opt_error.has_value())
						return opt_error.value();
				}
			}

			for (const auto& i : file.imports) {
				auto opt_error = insert_all_named_recursive_with_imports(project, named_by_file, i.imported , inserted);
				if (opt_error.has_value())
					return opt_error;
				NodeStructs::NameSpace& imported_named = named_by_file.at(i.imported);

				NOT_IMPLEMENTED;
				/*add(named, imported_named.types);
				add(named, imported_named.functions);
				add(named, imported_named.interfaces);
				add(named, imported_named.blocks);
				add(named, imported_named.templates);
				add(named, imported_named.enums);*/
				/*for (const auto& [k, e] : imported_named.namespaces)
					named.namespaces.emplace(k, copy(e));*/
			}
			return std::nullopt;
		}
	return error{
		"user error",
		std::string("Invalid import \"") + file_name + "\""
	};
}

void mark_exists_as_traversed(transpilation_state& state, variables_t& variables, const NodeStructs::NameSpace& exists, std::stringstream& ss) {
	for (const auto& e : exists.types) {
		auto rt_or_e = realise_type({ state, 0 }, e);
		if (rt_or_e.has_error())
			NOT_IMPLEMENTED;

		state.types_traversal.traversed.insert({ rt_or_e.value().name._value, std::move(rt_or_e).value()});
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

	for (const auto& e : exists.functions) {
		NOT_IMPLEMENTED;
		//state.functions_traversal.traversed.insert({ e.name, copy(e) });
	}

	for (const auto& e : exists.interfaces) {
		NOT_IMPLEMENTED;
		//state.interfaces_traversal.traversed.insert({ e.name, copy(e) });
	}

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

expected<std::pair<std::map<std::string, NodeStructs::NameSpace>, std::set<std::string>>> create_named_by_file(const std::vector<NodeStructs::File>& project) {
	std::map<std::string, NodeStructs::NameSpace> named_by_file{};
	std::set<std::string> inserted_named = {};

	// at this point we could also check if some files are never used and provide a warning

	for (const auto& file2 : project) {
		NodeStructs::NameSpace named_of_file = {
			.info = copy(file2.content.info)
		};
		//add_builtins(named_of_file);
		named_by_file.emplace(file2.content.name, std::move(named_of_file));
		if (auto opt_error = insert_all_named_recursive_with_imports(project, named_by_file, file2.content.name, inserted_named); opt_error.has_value())
			return opt_error.value();
	}

	return std::pair{ std::move(named_by_file), std::move(inserted_named) };
}

// entry
transpile_t transpile(const std::vector<NodeStructs::File>& project) {
	for (const auto& file : project)
		for (const auto& fn : file.content.functions)
			if (fn.name == "main") {

				auto named_or_e = create_named_by_file(project);
				return_if_error(named_or_e);
				auto [named_by_file, inserted_named] = std::move(named_or_e).value();

				if (auto opt_e = validate_templates(named_by_file.at(file.content.name).templates); opt_e.has_value())
					return opt_e.value();

				transpilation_state state{ std::move(named_by_file.at(file.content.name)) };

				traverse_builtins(state);

				std::stringstream exists_aliases;

				variables_t variables = make_base_variables();

				for (const auto& file : project)
					for (const auto& exists : file.exists)
						mark_exists_as_traversed(state, variables, exists, exists_aliases);

				std::optional<error> res = realise_main({ state, 0 }, fn);
				if (res.has_value())
					return res.value();

				std::stringstream cpp;
				cpp << "#include \"defaults.hpp\"\n\n" << exists_aliases.str();

				{
					std::stringstream declarations;
					std::stringstream definitions;

					for (const auto declaration : state.types_traversal.declarations)
						declarations << declaration;
					for (const auto definition : state.types_traversal.definitions)
						definitions << definition;

					for (const auto& enum_ : state.enums_to_transpile) {
						size_t val = 0;
						std::string enum_prefix = enum_.name + "__";
						for (const auto& enum_val : enum_.values)
							declarations << "static constexpr Int " << enum_prefix << enum_val << " = " << val++ << ";\n";
					}
					for (const auto& i : state.interfaces_traversal.traversed) {
					// for (const auto& i : state.interfaces_traversal.definitions) {
						NOT_IMPLEMENTED;
						//if (!state.traversed_interfaces.contains(i))
						//	NOT_IMPLEMENTED;
						//if (i.name_space.has_value())
						//	NOT_IMPLEMENTED;
						//const auto& members = state.interface_symbol_to_members.at(make_typename(NodeStructs::BaseTypename{ i.name }, NodeStructs::Value{}, rule_info_stub_no_throw()));
						//if (members.size() == 0)
						//	NOT_IMPLEMENTED;
						//auto unindented = transpilation_state_with_indent{ state, 0 };
						///*auto interface_repr = transpile_typename(unindented, make_typename( NodeStructs::BaseTypename{ i.name } });
						//return_if_error(interface_repr);*/
						//auto k = members
						//	| std::views::transform([&](auto&& T) { return typename_of_type(unindented, T); })
						//	| to_vec();
						//auto v = vec_of_expected_to_expected_of_vec(std::move(k));
						//return_if_error(v);
						//auto members_repr = transpile_typenames(unindented, variables, v.value());
						//return_if_error(members_repr);
						//declarations << "struct " << i.name << ";\n";
						//definitions << "struct " << i.name << " {\n\tUnion<" << members_repr.value() << "> value;\n};\n";
					}
					cpp << declarations.str() << "\n"
						<< definitions.str() << "\n";
				}
				{
					std::stringstream declarations;
					std::stringstream definitions;

					for (const std::pair<std::string, std::string>& alias : state.aliases_to_transpile)
						declarations << "using " + alias.first + " = " + alias.second + ";\n";
					for (const auto declaration : state.functions_traversal.declarations)
						declarations << declaration;
					for (const auto definition : state.functions_traversal.definitions)
						definitions << definition;
					cpp << declarations.str() << "\n"
						<< definitions.str() << "\n";
				}

				cpp << 
					"int main(int argc, char** argv) {\n"
					"\tstd::vector<std::string> args {};\n"
					"\tfor (int i = 0; i < argc; ++i)\n"
					"\t\targs.push_back(std::string(argv[i]));\n"
					"\treturn _redirect_main(args);\n"
					"}\n";

				return cpp.str();
			}
	return error{ "user error", "Missing \"main\" function" };
}

std::optional<error> realise_main(
	transpilation_state_with_indent state,
	const NodeStructs::Function& fn
) {
	if (fn.parameters.size() != 1)
		return error{ "user error","\"main\" function requires 1 argument of type `Vector<String> ref`" };

	const auto& [type, name] = fn.parameters.at(0);
	auto vector_str = make_typename(NodeStructs::TemplatedTypename{
		make_typename(NodeStructs::BaseTypename{ "Vector" }, NodeStructs::Value{}, rule_info_language_element("Vector")),
		as_vec(NodeStructs::WordTypenameOrExpression{ std::string{ "String" } })
	}, NodeStructs::Reference{}, rule_info_language_element("Vector<String>"));
	if (cmp(type, vector_str) != std::strong_ordering::equivalent)
		return error{ "user error","\"main\" function using 1 argument must be of `Vector<String> ref` type" };

	std::vector<expression_information> args{};
	if (fn.parameters.size())
		args.push_back(
			expression_information{ non_type_information{
				.type = { Realised::VectorType{
					.name = "Vector_String_",
					.value_type = { Realised::PrimitiveType{ Realised::PrimitiveType::String{} } }
				} },
				.representation = "args",
				.value_category = { NodeStructs::Reference{} }
			} }
		);
	auto res = realise_function(
		state,
		variables_t{},
		NodeStructs::Function{
			.name = "_redirect_main",
			.name_space = std::nullopt,
			.returnType = copy(fn.returnType),
			.parameters = copy(fn.parameters),
			.statements = copy(fn.statements),
			.info = copy(fn.info)
		},
		std::move(args)
	);
	if (res.has_error())
		return res.error();
	else
		return std::nullopt;
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

//transpile_declaration_definition_t transpile(
//	transpilation_state_with_indent state,
//	const NodeStructs::Function& fn
//) {
//	variables_t variables = make_base_variables();
//	auto return_typename = transpile_typename(state, variables, fn.returnType);
//	return_if_error(return_typename);
//	auto return_type = type_of_typename(state, variables, fn.returnType);
//	return_if_error(return_type);
//	auto parameters = transpile(state, variables, fn.parameters);
//	return_if_error(parameters);
//	auto stack_params_opt_error = stack(state, variables, fn.parameters);
//	if (stack_params_opt_error.has_value())
//		return stack_params_opt_error.value();
//	auto statements = transpile_statements(state.indented(), variables, fn.statements, return_type.value());
//	return_if_error(statements);
//	if (fn.name_space.has_value()) {
//		std::string common = return_typename.value() + " " + transpile_typename(state, variables, fn.name_space.value()).value() + "__" + fn.name + "(" + parameters.value() + ")";
//		std::string declaration = common + ";\n";
//		std::string definition = common + " {\n" + statements.value() + "};\n";
//		return std::pair{ declaration, definition };
//	}
//	else {
//		std::string common = return_typename.value() + " " + fn.name + "(" + parameters.value() + ")";
//		std::string declaration = common + ";\n";
//		std::string definition = common + " {\n" + statements.value() + "};\n";
//		return std::pair{ declaration, definition };
//	}
//}

transpile_declaration_definition_t transpile(
	transpilation_state_with_indent state,
	const NodeStructs::Type& type
) {
	std::stringstream declaration, definition;
	declaration << "struct " << type.name << ";\n";
	definition << "struct " << type.name << " {\n";

	variables_t variables = make_base_variables();
	for (const auto& member : type.members) {
		std::visit(overload(
			[&](const NodeStructs::Alias& alias) {
				NOT_IMPLEMENTED;
			},
			[&](const NodeStructs::MemberVariable& member) {
				definition << "\t" << transpile_typename(state, variables, member.type).value() << " " << member.name << ";\n";
			}
		), std::get<NodeStructs::contextual_options<type_context>>(member.statement.get()._value)._value);
	}

	definition << "};\n\n";
	return std::pair{ declaration.str(), definition.str() };
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

Realised::MetaType iterator_type(
	transpilation_state_with_indent state,
	const Realised::MetaType& type
) {
	return std::visit(overload(
		[&](const auto& x) -> Realised::MetaType {
			NOT_IMPLEMENTED;
		},
		[&](const Realised::VectorType& vt) -> Realised::MetaType {
			return copy(vt.value_type);
		}
	), type.type.get()._value);
}

std::vector<Realised::MetaType> decompose_type(
	transpilation_state_with_indent state,
	const Realised::MetaType& type
) {
	NOT_IMPLEMENTED;
}

std::optional<error> add_for_iterator_variables(
	transpilation_state_with_indent state,
	variables_t& variables,
	const std::vector<Variant<NodeStructs::VariableDeclaration, std::string>>& iterators,
	const Realised::MetaType& it_type
) {
	if (iterators.size() != 1)
		return error{ "user error","Expected 1 iterator" };
	return std::visit(
		overload(overload_default_error,
			[&](const NodeStructs::VariableDeclaration& it) -> std::optional<error> {
				Realised::MetaType iterator_type = type_of_typename(state, variables, it.type).value();
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
	const Realised::MetaType& parameter,
	const Realised::CompileTimeType& arg
) {
	auto res = assigned_to(state_, variables, parameter, arg.type);
	if (holds<not_assignable>(res))
		return not_assignable{};
	else if (holds<directly_assignable>(res)) {
		if (holds<Realised::PrimitiveType>(arg.type)) {
			const auto& x = get<Realised::PrimitiveType>(arg.type);
			return requires_conversion{
				[&](transpilation_state_with_indent, variables_t&, const NodeStructs::Expression& expr) -> transpile_expression_information_t {
					std::string representation = [&]() {
						std::stringstream ss;
						std::visit(overload(
							[&](const auto& val) {
								NOT_IMPLEMENTED;
							},
							[&](const Realised::PrimitiveType::Valued<int>& val) {
								ss << val.value;
							},
							[&](const Realised::void_t& val) { ss << "void_t{}"; },
							[&](const Realised::empty_optional_t& val) { ss << "empty_optional_t{}"; }
						), x.value._value);
						return ss.str();
					}();
					return non_type_information{ .type = { copy(x) }, .representation = std::move(representation), .value_category = NodeStructs::Value{} };
				}
			};
		}
		NOT_IMPLEMENTED;
	}
	NOT_IMPLEMENTED;
}

struct no_previous_conversion {};

requires_conversion get_converter(const Realised::MetaType& to, auto _previous_converter) {
	return requires_conversion{
		[T = copy(to), previous_converter = std::move(_previous_converter)]
		(transpilation_state_with_indent state, variables_t& variables, const NodeStructs::Expression& expr) -> transpile_expression_information_t {
			(void)previous_converter; // causes unused warnings for some template instantiations
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
	const Realised::MetaType& parameter,
	const Realised::MetaType& argument
) {
	if (cmp(parameter, argument) == std::strong_ordering::equivalent)
		return directly_assignable{};

	using R = Variant<not_assignable, directly_assignable, requires_conversion>;

	if (holds<Realised::CompileTimeType>(argument))
		return compile_time_assigned_to(state_, variables, parameter, get<Realised::CompileTimeType>(argument));

	return std::visit(
		overload(
			[&](const auto& param, const auto& arg, const auto& state) -> R {
				NOT_IMPLEMENTED;
			},
			[&](const Realised::OptionalType& param, const auto& arg, const auto& state) -> R {
				return assigned_to(state_, variables, param.value_type, { copy(arg) });
			},
			[&](const Realised::Builtin& param, const auto& arg, const auto& state) -> R {
				NOT_IMPLEMENTED;
				/*if (param.name == "filesystem__directory") {
					return assigned_to(state, variables, Realised::MetaType{ Realised::PrimitiveType{ Realised::PrimitiveType::NonValued<std::string>{} } }, { copy(arg) });
				}*/
			},
			[&](const Realised::UnionType& param, const auto& arg, const auto& state) -> R {
				if constexpr (std::is_same_v<std::remove_cvref_t<decltype(arg)>, Realised::UnionType>) {
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
			[&](const Realised::Type& param, const Realised::AggregateType& arg, const auto& state) -> R {
				NOT_IMPLEMENTED;
				//if (param.member_variables.size() != arg.arguments.size())
				//	return not_assignable{};
				//std::vector<R> assign_ts;
				//assign_ts.reserve(param.member_variables.size());
				//bool is_directly_assignable = true;
				//for (size_t i = 0; i < param.member_variables.size(); ++i) {
				//	auto mem_t = type_of_typename(state, variables, param.member_variables.at(i).type);
				//	if (mem_t.has_error())
				//		NOT_IMPLEMENTED;
				//	auto assign_t = assigned_to(state, variables, mem_t.value(), arg.arg_types.at(i))._value;
				//	if (std::holds_alternative<not_assignable>(assign_t))
				//		return not_assignable{};
				//	else if (std::holds_alternative<requires_conversion>(assign_t))
				//		is_directly_assignable = false;
				//	assign_ts.push_back(std::move(assign_t));
				//}
				//if (is_directly_assignable)
				//	return directly_assignable{};
				//else {
				//	return requires_conversion{
				//		[T = copy(param), s = param.name, assign_ts = std::move(assign_ts)]
				//		(transpilation_state_with_indent state, variables_t& variables, const NodeStructs::Expression& expr) -> transpile_expression_information_t {
				//			if (!holds<NodeStructs::BraceArguments>(expr))
				//				NOT_IMPLEMENTED;
				//			const auto& assumed_aggregate = get<NodeStructs::BraceArguments>(expr);
				//			std::stringstream ss;
				//			std::vector<R>& non_const_assign_ts = const_cast<std::vector<R>&>(assign_ts);
				//			for (size_t i = 0; i < assign_ts.size(); ++i) {
				//				R& assign_t = non_const_assign_ts.at(i);
				//				const NodeStructs::FunctionArgument& expr = assumed_aggregate.args.at(i);
				//				std::visit(overload(
				//					[&](auto& assignable) -> transpile_expression_information_t {
				//						auto expr_info = [&]() {
				//							if constexpr (std::is_same_v<std::remove_cvref_t<decltype(assignable)>, requires_conversion>)
				//								return assignable.converter(state, variables, expr.expr); // todo doesnt use expr.cat
				//							else
				//								return transpile_arg(state, variables, expr);
				//						}();
				//						return_if_error(expr_info);
				//						if (!std::holds_alternative<non_type_information>(expr_info.value()))
				//							NOT_IMPLEMENTED;
				//						const auto& expr_info_ok = std::get<non_type_information>(expr_info.value());
				//						ss << expr_info_ok.representation << ", ";
				//						NOT_IMPLEMENTED;
				//					},
				//					[](not_assignable&) -> transpile_expression_information_t {
				//						NOT_IMPLEMENTED;
				//					}
				//				), assign_t._value);
				//			}
				//			return expression_information{ non_type_information{
				//				.type = { std::move(const_cast<decltype(T)&>(T)) },
				//				.representation = s + "{" + ss.str() + "}",
				//				.value_category = NodeStructs::Value{}
				//			} };
				//		}
				//	};
				//}
			},
			[&](const Realised::Type& param, const Realised::Type& arg, const auto& state) -> R {
				if (cmp(param, arg) == std::strong_ordering::equivalent)
					return directly_assignable{};
				// if the parameter has 1 member, try to assign to that member
				if (param.member_variables.size() == 1) {
					// in the event where assignment can work, we still need a `requires_conversion`
					R mem_assigned = assigned_to(state, variables, param.member_variables.at(0).type, argument);
					if (std::holds_alternative<directly_assignable>(mem_assigned._value)) {
						return requires_conversion{
							[s = param.name._value, cp = copy(param)](transpilation_state_with_indent state, variables_t& variables, const NodeStructs::Expression& expr) -> transpile_expression_information_t {
								transpile_expression_information_t expr_info = transpile_expression(state, variables, expr);
								return_if_error(expr_info);
								if (!std::holds_alternative<non_type_information>(expr_info.value()))
									NOT_IMPLEMENTED;
								non_type_information& expr_info_ok = std::get<non_type_information>(expr_info.value());
								return expression_information{ non_type_information{
									.type = Realised::MetaType{ std::move(const_cast<Realised::Type&>(cp)) },
									.representation = s + "{" + expr_info_ok.representation + "}",
									.value_category = std::move(expr_info_ok).value_category
								} };
							}
						};
					}
					else if (std::holds_alternative<requires_conversion>(mem_assigned._value)) {
						return requires_conversion{
							[s = param.name._value, cp = copy(param), mem_assigned = std::get<requires_conversion>(std::move(mem_assigned)._value)]
							(transpilation_state_with_indent state, variables_t& variables, const NodeStructs::Expression& expr) -> transpile_expression_information_t {
								requires_conversion& rc = const_cast<requires_conversion&>(mem_assigned);
								transpile_expression_information_t expr_info = rc.converter(state, variables, expr);
								return_if_error(expr_info);
								if (!std::holds_alternative<non_type_information>(expr_info.value()))
									NOT_IMPLEMENTED;
								non_type_information& expr_info_ok = std::get<non_type_information>(expr_info.value());
								return expression_information{ non_type_information{
									.type = Realised::MetaType{ std::move(const_cast<Realised::Type&>(cp)) },
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
			[&](const Realised::PrimitiveType& param, const Realised::PrimitiveType& arg, const auto& state) -> R {
				if (holds<Realised::PrimitiveType::NonValued<std::string>>(param.value) && (holds<Realised::PrimitiveType::NonValued<char>>(arg.value) || holds<Realised::PrimitiveType::Valued<char>>(arg.value)))
					return requires_conversion{
						[](transpilation_state_with_indent state, variables_t& variables, const NodeStructs::Expression& expr) -> transpile_expression_information_t {
							transpile_expression_information_t expr_info = transpile_expression(state, variables, expr);
							return_if_error(expr_info);
							if (!std::holds_alternative<non_type_information>(expr_info.value()))
								NOT_IMPLEMENTED;
							non_type_information& expr_info_ok = std::get<non_type_information>(expr_info.value());
							return expression_information{ non_type_information{
								.type = { Realised::PrimitiveType{ Realised::PrimitiveType::NonValued<std::string>{} } },
								.representation = "String{" + expr_info_ok.representation + "}",
								.value_category = std::move(expr_info_ok).value_category
							} };
						}
					};
				if (primitives_assignable(param, arg))
					return directly_assignable{};
				else
					return not_assignable{};
			},
			[&](const Realised::OptionalType& param, const Realised::PrimitiveType& arg, const auto& state) -> R {
				NOT_IMPLEMENTED;
				//if (holds<Realised::PrimitiveType::Valued<Realised::empty_optional_t>>(only(arg)))
				//	return directly_assignable{};
				//else {
				//	auto inner_assignable = assigned_to(state, variables, only(param)._value, argument);
				//	if (std::holds_alternative<directly_assignable>(inner_assignable._value))
				//		return get_converter(only(param), no_previous_conversion{});
				//	else if (std::holds_alternative<requires_conversion>(inner_assignable._value))
				//		return get_converter(only(param), std::get<requires_conversion>(std::move(inner_assignable._value)));
				//	else
				//		return not_assignable{};
				//}
			},
			[&](const Realised::Builtin& param, const Realised::PrimitiveType& arg, const auto& state) -> R {
				NOT_IMPLEMENTED; 
			},
			[&](const Realised::UnionType& param, const Realised::PrimitiveType& arg, const auto& state) -> R {
				NOT_IMPLEMENTED;
			},
			[&](const auto& param, const Realised::PrimitiveType& arg, const auto& state) -> R {
				return not_assignable{};
			},
			
			[&](const Realised::EnumType& param, const Realised::EnumValueType& arg, const auto& state) -> R {
				if (cmp(param.enum_.get(), arg.enum_.get()) == std::strong_ordering::equivalent)
					return directly_assignable{};
				else
					return not_assignable{};
			},
			[&](const Realised::Builtin& param, const Realised::Builtin& arg, const auto& state) -> R {
				if (param.builtin._value.index() == arg.builtin._value.index())
					return directly_assignable{};
				else
					return not_assignable{};
			},
			[&](const Realised::VectorType& param, const Realised::VectorType& arg, const auto& state) -> R {
				if (cmp(param.value_type, arg.value_type) == std::strong_ordering::equivalent)
					return directly_assignable{};
				else
					return not_assignable{};
			},
			[&](const Realised::SetType& param, const Realised::SetType& arg, const auto& state) -> R {
				if (cmp(param.value_type, arg.value_type) == std::strong_ordering::equivalent)
					return directly_assignable{};
				else
					return not_assignable{};
			},
			[&](const Realised::InterfaceType& param, const Realised::Type& arg, const auto& state) -> R {
				NOT_IMPLEMENTED;
				//for (const auto& [type_name, name] : param.interface.get().member_variables) {
				//	auto t = type_of_typename(state, variables, type_name);
				//	if (t.has_error())
				//		NOT_IMPLEMENTED;
				//	for (const auto& [source_type_name, source_name] : arg.member_variables) {
				//		NOT_IMPLEMENTED;
				//		/*if (name == source_name) {
				//			auto t2 = type_of_typename(state, variables, source_type_name);
				//			if (t2.has_error())
				//				NOT_IMPLEMENTED;
				//			if (cmp(t.value(), t2.value()) != std::strong_ordering::equivalent)
				//				return not_assignable{};
				//		}*/
				//	}
				//}
				NOT_IMPLEMENTED;
				/*auto& interfacemembers = state.state.interface_symbol_to_members[
					make_typename(NodeStructs::BaseTypename{ param.interface.get().name }, NodeStructs::Value{}, rule_info_stub_no_throw())
				];
				auto new_member = Realised::MetaType{ copy(arg) };
				for (const auto& member : interfacemembers)
					if (cmp(member, new_member) == std::strong_ordering::equivalent)
						return directly_assignable{};
				interfacemembers.push_back(std::move(new_member));*/
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

	if (holds<Realised::Type>(t_ok.type)) {
		std::stringstream ss;
		const auto& t_ref = get<Realised::Type>(t_ok.type);
		ss << t_ref.name._value << "{";
		for (const auto& [member_name, member_type] : t_ref.member_variables) {
			NOT_IMPLEMENTED;
			/*auto member = NodeStructs::PropertyAccessExpression{ .operand = copy(expr), .property_name = member_type };
			auto member_repr = expr_to_printable(state, variables, make_expression(std::move(member), rule_info_stub_no_throw()));
			return_if_error(member_repr);
			ss << member_type << " = \" << " << member_repr.value();*/
		}
		return ss.str();
	}
	else if (holds<Realised::InterfaceType>(t_ok.type)) {
		const auto& t_ref = get<Realised::InterfaceType>(t_ok.type).interface.get();
		std::stringstream ss;
		NOT_IMPLEMENTED;
		/*ss << "\"" << t_ref.name << "{";
		for (const auto& [member_typename, member_name] : t_ref.member_variables) {
			auto typename_t = type_of_typename(state, variables, member_typename);
			return_if_error(typename_t);
			auto member = NodeStructs::PropertyAccessExpression{ .operand = copy(expr), .property_name = member_name };
			auto member_repr = expr_to_printable(state, variables, make_expression(std::move(member), rule_info_stub_no_throw()));
			return_if_error(member_repr);
			ss << member_name << " = \" << " << member_repr.value();
		}*/
		ss << " << \"}\"";
		return ss.str();
	}
	else if (holds<Realised::PrimitiveType>(t_ok.type)) {
		const auto& primitive_type = get<Realised::PrimitiveType>(t_ok.type);
		auto expr_repr = transpile_expression(state, variables, expr);
		return_if_error(expr_repr);
		if (!std::holds_alternative<non_type_information>(expr_repr.value()))
			NOT_IMPLEMENTED;
		const auto& expr_repr_ok = std::get<non_type_information>(expr_repr.value());
		return std::visit(
			overload(
				[&](const Realised::PrimitiveType::NonValued<std::string>&) -> std::string {
					return "('\"' + " + expr_repr_ok.representation + " + '\"')";
				},
				[&](const Realised::PrimitiveType::Valued<std::string>&) -> std::string {
					return expr_repr_ok.representation;
				},
				[&](const Realised::PrimitiveType::NonValued<char>&) -> std::string {
					// not optimal
					return "(String(\"'\") + " + expr_repr_ok.representation + " + String(\"'\"))";
				},
				[&](const Realised::PrimitiveType::NonValued<int>&) -> std::string {
					return expr_repr_ok.representation;
				},
				[&](const Realised::PrimitiveType::NonValued<double>&) -> std::string {
					return expr_repr_ok.representation;
				},
				[&](const Realised::PrimitiveType::NonValued<bool>&) -> std::string {
					return "String{" + expr_repr_ok.representation + " ? \"True\" : \"False\"}";
				},
				[&](const Realised::PrimitiveType::NonValued<Realised::void_t>&) -> std::string {
					return "String(\"Void\")";
				},
				[&](const Realised::PrimitiveType::NonValued<Realised::empty_optional_t>&) -> std::string {
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
	if (cmp(fn.returnType, auto_tn) == std::strong_ordering::equivalent)
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
	return t <=> auto_tn_base == std::strong_ordering::equivalent;
}

bool uses_auto(const NodeStructs::NamespacedTypename& t) {
	if (t.name_in_name_space == auto_tn_base.type || uses_auto(t.name_space))
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

NodeStructs::Typename typename_of_primitive(const Realised::PrimitiveType& primitive_t) {
	return std::visit(overload(overload_default_error,
		[&](const Realised::PrimitiveType::NonValued<std::string>&) {
			return copy(String_tn);
		},
		[&](const Realised::PrimitiveType::Valued<std::string>&) {
			return copy(String_tn);
		},
		[&](const Realised::PrimitiveType::NonValued<double>&) {
			return copy(Floating_tn);
		},
		[&](const Realised::PrimitiveType::Valued<double>&) {
			return copy(Floating_tn);
		},
		[&](const Realised::PrimitiveType::NonValued<int>&) {
			return copy(Int_tn);
		},
		[&](const Realised::PrimitiveType::Valued<int>&) {
			return copy(Int_tn);
		},
		[&](const Realised::PrimitiveType::NonValued<bool>&) {
			return copy(Bool_tn);
		},
		[&](const Realised::PrimitiveType::Valued<bool>&) {
			return copy(Bool_tn);
		},
		[&](const Realised::PrimitiveType::NonValued<Realised::void_t>&) {
			return copy(Void_tn);
		},
		[&](const Realised::PrimitiveType::Valued<Realised::void_t>&) {
			return copy(Void_tn);
		},
		[&](const Realised::PrimitiveType::NonValued<Realised::empty_optional_t>&) -> NodeStructs::Typename {
			NOT_IMPLEMENTED;
		},
		[&](const Realised::PrimitiveType::Valued<Realised::empty_optional_t>&) -> NodeStructs::Typename {
			NOT_IMPLEMENTED;
		},
		[&](const Realised::PrimitiveType::NonValued<char>&) {
			return copy(Char_tn);
		},
		[&](const Realised::PrimitiveType::Valued<char>&) {
			return copy(Char_tn);
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
	
	// produce an error message
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

expected<Realised::Function> realise_function(
	transpilation_state_with_indent state,
	variables_t variables, // by value
	const NodeStructs::Function& function,
	const std::vector<expression_information>& args
) {
	if (function.name_space.has_value())
		NOT_IMPLEMENTED;
	if (function.parameters.size() != args.size())
		return error{
			"user_error",
			"wrong number of arguments: " + std::to_string(args.size()) + ", to function : " + function.name
		};

	for (size_t i = 0; i < args.size(); ++i) {
		const auto& arg = args.at(i);
		const auto& param = function.parameters.at(i);
		if (holds<type_information>(arg))
			NOT_IMPLEMENTED_BUT_PROBABLY_CORRECT;
		if (holds<NodeStructs::VariadicExpansionTypename>(param.typename_))
			NOT_IMPLEMENTED_BUT_PROBABLY_CORRECT;

		auto t_or_e = type_of_typename(state, variables, param.typename_);
		return_if_error(t_or_e);

		Variant<not_assignable, directly_assignable, requires_conversion> assigned = 
			assigned_to(state, variables, t_or_e.value(), std::get<non_type_information>(arg).type);

		if (holds<not_assignable>(assigned))
			return error{
				"user_error",
				"One of the function arguments is not assignable to the parameter, function was `" 
				+ function.name + "` parameter was `" + param.name
				+ "` and argument was `" + std::get<non_type_information>(arg).representation + "`"
			};
	}

	std::string fkey = function.name; // todo args embedded in name for uniqueness
	state.state.functions_traversal.traversing.insert(fkey);

	std::stringstream declaration{};
	std::stringstream definition{};

	if (function.returnType <=> auto_tn == std::strong_ordering::equivalent)
		NOT_IMPLEMENTED;
	bool is_void = function.returnType <=> Void_tn == std::strong_ordering::equivalent;

	auto rt_or_e = type_of_typename(state, variables, function.returnType);
	return_if_error(rt_or_e);
	auto tn_or_e = typename_of_type(state, rt_or_e.value());
	return_if_error(tn_or_e);
	auto tn_str_or_e = transpile_typename(state, variables, tn_or_e.value());
	return_if_error(tn_str_or_e);

	declaration << tn_str_or_e.value() << " " << fkey << "(";
	definition << tn_str_or_e.value() << " " << fkey << "(";
	bool is_first = true;

	std::vector<Realised::Parameter> parameters{};
	parameters.reserve(args.size());

	for (const auto& param : function.parameters) {
		auto t_or_e = type_of_typename(state, variables, param.typename_);
		return_if_error(t_or_e);
		auto tn_or_e = typename_of_type(state, t_or_e.value());
		return_if_error(tn_or_e);
		auto tn_str_or_e = transpile_typename(state, variables, tn_or_e.value());
		return_if_error(tn_str_or_e);

		if (is_first)
			is_first = false;
		else {
			declaration << ", ";
			definition  << ", ";
		}

		auto cat = param.typename_.category._value.has_value() 
			? copy(param.typename_.category._value.value())
			: NodeStructs::ParameterCategory{ NodeStructs::Reference{} };
		parameters.push_back(Realised::Parameter{ .type = copy(t_or_e.value()), .category = copy(cat) });
		variables[param.name].push_back(variable_info{
			.value_category = parameter_category_to_value_category(cat),
			.type = copy(t_or_e.value()),
		});
		std::string param_def = [&]() {
			if (holds<NodeStructs::Value>(cat))
				return tn_str_or_e.value() + " " + param.name;
			else if (holds<NodeStructs::Reference>(cat))
				return "const " + tn_str_or_e.value() + "& " + param.name;
			else if (holds<NodeStructs::MutableReference>(cat))
				return tn_str_or_e.value() + "& " + param.name;
			else
				NOT_IMPLEMENTED;
		}();
		declaration << param_def;
		definition  << param_def;
	}

	auto statement_str_or_e = transpile_statements(state.indented(), variables, function.statements, rt_or_e.value());
	return_if_error(statement_str_or_e);

	declaration << ");\n";
	definition << ") {\n" << statement_str_or_e.value() << "}\n";

	Realised::Function realised{
		.name = fkey,
		.returnType = copy(rt_or_e.value()),
		.parameters = std::move(parameters),
		.info = copy(function.info)
	};

	state.state.functions_traversal.traversing.erase(fkey);
	state.state.functions_traversal.traversed.emplace(fkey, copy(realised));
	state.state.functions_traversal.declarations.push_back(declaration.str());
	state.state.functions_traversal.definitions.push_back(definition.str());

	return realised;
}

transpile_expression_information_t produce_call(
	transpilation_state_with_indent state,
	variables_t& variables,
	const Realised::Function& function,
	const std::vector<expression_information>& args
) {
	std::stringstream repr{};
	repr << function.name._value << "(";
	if (function.parameters.size() != 0)
		NOT_IMPLEMENTED;
	repr << ")";
	return expression_information{ non_type_information {
		.type = copy(function.returnType),
		.representation = repr.str(),
		.value_category = NodeStructs::Value{} // todo
	} };
}

transpile_expression_information_t realise_function_and_produce_call(
	transpilation_state_with_indent state,
	variables_t& variables,
	const std::string& name,
	const NodeStructs::NameSpace& space,
	const std::vector<expression_information>& args
) {
	auto fs_it = find_by_name(space.functions, name);
	auto templates_it = find_by_name(space.templates, name);
	if (fs_it != space.functions.end())
		if (templates_it != space.templates.end())
			NOT_IMPLEMENTED;
		else {
			auto f_or_e = realise_function(state.unindented(), copy(variables), *fs_it, args);
			return_if_error(f_or_e);
			return produce_call(state, variables, f_or_e.value(), args);
		}
	else
		if (templates_it != space.templates.end())
			NOT_IMPLEMENTED;
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
				state.state.global_namespace.aliases.push_back({
					out,
					make_typename(
						NodeStructs::BaseTypename{ get<std::string>(tmpl.operand.expression.get()) },
						NodeStructs::Value{},
						caesium_source_location{.file_name = "todo:/", .content = get<std::string>(tmpl.operand.expression.get()) }
					),
					std::nullopt
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

expected<Realised::MetaType> type_of_typename(
	transpilation_state_with_indent state,
	variables_t& variables,
	const NodeStructs::WordTypenameOrExpression& tn_or_expr
) {
	return std::visit(overload(
		[&](const NodeStructs::Expression&) -> expected<Realised::MetaType> {
			NOT_IMPLEMENTED;
		},
		[&](const NodeStructs::Typename& tn) -> expected<Realised::MetaType> {
			auto x = type_of_typename(state, variables, tn);
			return_if_error(x);
			return std::move(x).value();
		},
		[&](const std::string& tn) -> expected<Realised::MetaType> {
			auto x = type_of_typename(state, variables, NodeStructs::BaseTypename{ tn });
			return_if_error(x);
			return std::move(x).value();
		}
	), tn_or_expr.value._value);
}

variables_t make_base_variables() {
	variables_t variables{};
	{
		variables["True"].push_back(
			{
				NodeStructs::Value{},
				{ Realised::PrimitiveType{ Realised::PrimitiveType::Valued{ true } } }
			}
		);
		variables["False"].push_back(
			{
				NodeStructs::Value{},
				{ Realised::PrimitiveType{ Realised::PrimitiveType::Valued{ false } } }
			}
		);
		variables["None"].push_back(
			{
				NodeStructs::Value{},
				{ Realised::PrimitiveType{ Realised::PrimitiveType::Valued{ Realised::empty_optional_t{} } } }
			}
		);
	}
	return variables;
}

// realise

expected<NodeStructs::Type> realise_many_compile_time_statements(
	transpilation_state_with_indent state,
	variables_t& variables,
	NodeStructs::Type type,
	const std::vector<NodeStructs::CompileTimeStatement<type_context>>& statements
);

expected<NodeStructs::Type> realise_many_compile_time_statements(
	transpilation_state_with_indent state,
	variables_t& variables,
	NodeStructs::Type type,
	const std::vector<NodeStructs::Statement<type_context>>& statements
);

template <template <typename> typename CompileTimeStatement>
expected<NodeStructs::Type> realise_one_compile_time_statement(
	transpilation_state_with_indent state,
	variables_t& variables,
	NodeStructs::Type t,
	const CompileTimeStatement<type_context>& statement
) {
	NOT_IMPLEMENTED;
}

expected<NodeStructs::Type> realise_one_compile_time_statement(
	transpilation_state_with_indent state,
	variables_t& variables,
	NodeStructs::Type type,
	const NodeStructs::IfStatement<type_context>& statement
) {
	auto expr_info_or_e = transpile_expression(state, variables, statement.ifExpr);

	return_if_error(expr_info_or_e);
	if (!holds<non_type_information>(expr_info_or_e.value()))
		NOT_IMPLEMENTED_BUT_PROBABLY_ERROR;
	const non_type_information& expr_info = get<non_type_information>(expr_info_or_e.value());

	if (!holds<Realised::PrimitiveType>(expr_info.type))
		NOT_IMPLEMENTED_BUT_PROBABLY_ERROR;
	const Realised::PrimitiveType& primitive = get<Realised::PrimitiveType>(expr_info.type.type.get());

	if (!holds<Realised::PrimitiveType::Valued<bool>>(primitive.value))
		NOT_IMPLEMENTED_BUT_PROBABLY_ERROR;
	const Realised::PrimitiveType::Valued<bool>& boolean_compile_time_condition = get<Realised::PrimitiveType::Valued<bool>>(primitive.value);

	if (boolean_compile_time_condition.value)
		return realise_many_compile_time_statements(state, variables, std::move(type), statement.ifStatements);
	else if (statement.elseExprStatements.has_value())
		NOT_IMPLEMENTED_BUT_PROBABLY_CORRECT;
	else
		return type;
}

expected<NodeStructs::Type> realise_one_compile_time_statement(
	transpilation_state_with_indent state,
	variables_t& variables,
	NodeStructs::Type t,
	const NodeStructs::CompileTimeStatement<type_context>& statement
) {
	return std::visit([&](const auto& stmt) { return realise_one_compile_time_statement(state, variables, std::move(t), stmt); }, statement._value);
}

expected<NodeStructs::Type> realise_many_compile_time_statements(
	transpilation_state_with_indent state,
	variables_t& variables,
	NodeStructs::Type type,
	const std::vector<NodeStructs::CompileTimeStatement<type_context>>& statements
) {
	if (statements.size() == 0)
		return type;

	// keeping a vector of intermediates is not technically needed,
	// we could go with a recursive approach, but this way we keep the intermediates
	// at hand so if it leads to an error we can provide them to the user (or to the dev!)
	auto intermediates = caesium_lib::vector::push(
		caesium_lib::vector::make_with_capacity<NodeStructs::Type>(statements.size() + 1),
		std::move(type)
	);

	for (const NodeStructs::CompileTimeStatement<type_context>& compile_time_statement : statements) {
		variables_t variables = make_base_variables();
		expected<NodeStructs::Type> next = caesium_lib::variant::visit(compile_time_statement, [&](const auto& compile_time_statement) {
			return realise_one_compile_time_statement(state, variables, copy(intermediates._value.back()), compile_time_statement);
		});
		return_if_error(next);
		intermediates = caesium_lib::vector::push(
			std::move(intermediates),
			std::move(next).value()
		);
	}

	const auto& finished = intermediates._value.back();
	NOT_IMPLEMENTED;
	//return Realised::Type{ finished.name, copy(finished.aliases), copy(finished.member_variables), finished.info };
}

NodeStructs::Type add_member_to_type(
	NodeStructs::Type type,
	Variant<NodeStructs::Alias, NodeStructs::MemberVariable> member
) {
	caesium_lib::variant::visit(std::move(member), [&](auto m) {
		type.members.push_back(NodeStructs::Statement<type_context>{ NodeStructs::contextual_options<type_context>{ std::move(m) } });
	});
	return type;
}

expected<NodeStructs::Type> realise_many_compile_time_statements(
	transpilation_state_with_indent state,
	variables_t& variables,
	NodeStructs::Type type,
	const std::vector<NodeStructs::Statement<type_context>>& statements
) {
	if (statements.size() == 0)
		return type;

	// keeping a vector of intermediates is not technically needed,
	// we could go with a recursive approach, but this way we keep the intermediates
	// at hand so if it leads to an error we can provide them to the user (or to the dev!)
	auto intermediates = caesium_lib::vector::push(
		caesium_lib::vector::make_with_capacity<NodeStructs::Type>(statements.size() + 1),
		std::move(type)
	);

	for (const NodeStructs::Statement<type_context>& compile_time_statement : statements) {
		variables_t variables = make_base_variables();
		expected<NodeStructs::Type> next = caesium_lib::variant::visit(compile_time_statement.statement.get(), overload(
			[&](const Variant<NodeStructs::Alias, NodeStructs::MemberVariable>& member) -> expected<NodeStructs::Type> {
				return add_member_to_type(copy(intermediates._value.back()), copy(member));
			},
			[&](const NodeStructs::CompileTimeStatement<type_context>& compile_time_statement) -> expected<NodeStructs::Type> {
				return realise_one_compile_time_statement(state, variables, copy(intermediates._value.back()), compile_time_statement);
			}
		));
		return_if_error(next);
		intermediates = caesium_lib::vector::push(
			std::move(intermediates),
			std::move(next).value()
		);
	}

	return std::move(intermediates._value.back());
}

expected<Realised::Type> realise_type(transpilation_state_with_indent state, const NodeStructs::Type& type) {
	expected<std::string> name_or_e = [&]() -> expected<std::string> {
		if (type.name_space.has_value()) {
			auto variables_temp = make_base_variables();
			auto tn_or_e = transpile_typename(state, variables_temp, type.name_space.value());
			return_if_error(tn_or_e);
			return tn_or_e.value() + "__" + type.name;
		}
		else {
			return type.name;
		}
	}();
	return_if_error(name_or_e);
	const std::string& name = name_or_e.value();

	if (state.state.types_traversal.traversing.contains(name))
		NOT_IMPLEMENTED; // return traversed
	if (state.state.types_traversal.traversed.count(name))
		return copy(state.state.types_traversal.traversed.at(name));

	NodeStructs::Type without_members{
		.name = name,
		.name_space = copy(type.name_space),
		.members = {},
		.info = copy(type.info)
	};

	variables_t variables = make_base_variables();
	state.state.types_traversal.traversing.insert(name);
	expected<NodeStructs::Type> rt_or_e = realise_many_compile_time_statements(state, variables, std::move(without_members), type.members);
	state.state.types_traversal.traversing.erase(name);
	return_if_error(rt_or_e);

	const NodeStructs::Type& realised_type = rt_or_e.value();

	std::vector<std::pair<std::string, expected<Realised::MetaType>>> expected_types = realised_type.members
		| std::views::filter([&](const NodeStructs::Statement<type_context>& member) -> bool {
			return holds<NodeStructs::contextual_options<type_context>>(member);
		})
		| std::views::transform([&](const NodeStructs::Statement<type_context>& member) -> const NodeStructs::contextual_options<type_context>& {
			return get<NodeStructs::contextual_options<type_context>>(member);
		})
		| std::views::filter([&](const NodeStructs::contextual_options<type_context>& member) -> bool {
			return holds<NodeStructs::MemberVariable>(member);
		})
		| std::views::transform([&](const NodeStructs::contextual_options<type_context>& member) -> std::reference_wrapper<const NodeStructs::MemberVariable> {
			return std::reference_wrapper{ get<NodeStructs::MemberVariable>(member) };
		})
		| std::views::transform([&](const std::reference_wrapper<const NodeStructs::MemberVariable>& member) {
			return std::pair{ copy(member.get().name), type_of_typename(state, variables, member.get().type)};
		})
		| to_vec();

	for (const std::pair<std::string, expected<Realised::MetaType>>& member : expected_types)
		if (member.second.has_error())
			return member.second.error();

	auto declaration_definition_or_e = transpile(state, realised_type);
	return_if_error(declaration_definition_or_e);


	Realised::Type res{
		.name = copy(realised_type.name),
		.member_variables = expected_types
		| std::views::transform([&](std::pair<std::string, expected<Realised::MetaType>>& member) -> Realised::MemberVariable {
			return { std::move(member.first), std::move(member.second).value() };
		})
		| to_vec(),
		.info = copy(realised_type.info)
	};

	state.state.types_traversal.traversed.insert({ type.name, copy(res) });
	state.state.types_traversal.declarations.push_back(std::move(declaration_definition_or_e).value().first);
	state.state.types_traversal.definitions.push_back(std::move(declaration_definition_or_e).value().second);
	return res;
}

expected<Realised::Type> get_existing_realised_type(
	transpilation_state_with_indent state,
	const std::string& name,
	const std::optional<NodeStructs::NameSpace>& name_space
) {
	if (name_space.has_value())
		NOT_IMPLEMENTED;
	if (state.state.types_traversal.traversed.count(name))
		return copy(state.state.types_traversal.traversed.at(name));
	return error{ "user error", "Missing \"main\" function" };
}

std::string_view name_of_builtin(const Realised::Builtin& builtin) {
	return caesium_lib::variant::visit(builtin.builtin, [](const auto& builtin) -> std::string_view { return builtin.name; });
}

expected<std::optional<Realised::MetaType>> deduce_return_type(
	transpilation_state_with_indent state,
	variables_t& variables,
	const NodeStructs::RunTimeStatement& statement
) {
	NOT_IMPLEMENTED;
	//return std::visit(
	//	overload(
	//		overload_default_error,
	//		[&](const NodeStructs::ReturnStatement<context>& statement) -> expected<std::optional<Realised::MetaType>> {
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
	//		[&](const NodeStructs::Expression& statement) -> expected<std::optional<Realised::MetaType>> {
	//			return std::nullopt;
	//		},
	//		[&](const NodeStructs::VariableDeclarationStatement<context>& statement) -> expected<std::optional<Realised::MetaType>> {
	//			return std::nullopt;
	//		},
	//		[&](const NodeStructs::IfStatement<context>& statement) -> expected<std::optional<Realised::MetaType>> {
	//			auto if_res_t = deduce_return_type(state, variables, statement.ifStatements);
	//			return_if_error(if_res_t);

	//			if (!statement.elseExprStatements.has_value())
	//				return if_res_t;

	//			auto else_res_t = std::visit(
	//				overload(
	//					[&](const std::vector<NodeStructs::Statement<context>>& statements) -> expected<std::optional<Realised::MetaType>> {
	//						return deduce_return_type(state, variables, statements);
	//					},
	//					[&](const NonCopyableBox<NodeStructs::IfStatement<context>>& inner_if) -> expected<std::optional<Realised::MetaType>> {
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
	//		[&](const NodeStructs::IForStatement<context>& statement) -> expected<std::optional<Realised::MetaType>> {
	//			NOT_IMPLEMENTED;
	//		},
	//		[&](const NodeStructs::ForStatement<context>& statement) -> expected<std::optional<Realised::MetaType>> {
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
	//		[&](const NodeStructs::WhileStatement<context>& statement) -> expected<std::optional<Realised::MetaType>> {
	//			return deduce_return_type(state, variables, statement.statements);
	//		},
	//		[&](const NodeStructs::BreakStatement<context>& statement) -> expected<std::optional<Realised::MetaType>> {
	//			return std::nullopt;
	//		},
	//		[&](const NodeStructs::BlockStatement<context>& statement) -> expected<std::optional<Realised::MetaType>> {
	//			NOT_IMPLEMENTED;
	//		},
	//		[&](const NodeStructs::MatchStatement<context>& statement) -> expected<std::optional<Realised::MetaType>> {
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

	//			std::optional<Realised::MetaType> res = std::nullopt;
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
	//		[&](const NodeStructs::SwitchStatement<context>& statement) -> expected<std::optional<Realised::MetaType>> {
	//			NOT_IMPLEMENTED;
	//		},
	//		[&](const NodeStructs::Assignment<context>& statement) -> expected<std::optional<Realised::MetaType>> {
	//			return std::nullopt;
	//		}
	//	),
	//	statement.statement.get()._value
	//);
}

NodeStructs::ValueCategory parameter_category_to_value_category(
	const NodeStructs::ParameterCategory& cat
) {
	return caesium_lib::variant::visit(cat, overload(overload_default_error,
		[](const NodeStructs::Reference) -> NodeStructs::ValueCategory {
			return NodeStructs::Reference{};
		},
		[](const NodeStructs::MutableReference) -> NodeStructs::ValueCategory {
			return NodeStructs::MutableReference{};
		},
		[](const NodeStructs::Value) -> NodeStructs::ValueCategory {
			return NodeStructs::MutableReference{};
		}
	));
}

NodeStructs::ValueCategory optional_parameter_category_to_value_category(
	const std::optional<NodeStructs::ParameterCategory>& opt_cat
) {
	if (opt_cat.has_value())
		return parameter_category_to_value_category(opt_cat.value());
	else
		return NodeStructs::Reference{};
}

NodeStructs::ValueCategory optional_parameter_category_to_value_category(
	const Optional<NodeStructs::ParameterCategory>& opt_cat
) {
	return optional_parameter_category_to_value_category(opt_cat._value);
}
