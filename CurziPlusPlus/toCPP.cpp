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
#include "expression_for_template_visitor.hpp"

void insert_all_named_recursive_with_imports(const std::vector<NodeStructs::File>& project, std::map<std::string, Named>& named_by_file, const std::string& filename) {
	for (const NodeStructs::File& file : project)
		if (file.filename == filename) {
			Named& named = named_by_file[filename];

			for (const auto& e : file.types)
				named.types[e.name].push_back(&e);

			for (const auto& e : file.functions)
				named.functions[e.name].push_back(&e);

			for (const auto& e : file.blocks)
				named.blocks[e.name].push_back(&e);

			for (const auto& e : file.templates)
				named.templates[e.name].push_back(&e);

			for (const auto& i : file.imports) {
				insert_all_named_recursive_with_imports(project, named_by_file, i.imported);
				Named& imported_named = named_by_file[i.imported];

				named.types.insert(imported_named.types.begin(), imported_named.types.end());
				named.functions.insert(imported_named.functions.begin(), imported_named.functions.end());
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
				auto t = type_of_typename_visitor{ {}, { state, 0 } }(alias.aliasFrom);
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
				cpp_std stuff_from_cpp{};
				variables_t variables;
				{
					variables["True"].push_back({ NodeStructs::Value{}, { stuff_from_cpp.builtin_bool } });
					variables["False"].push_back({ NodeStructs::Value{}, { stuff_from_cpp.builtin_bool } });
				}

				std::map<std::string, Named> named_by_file;

				for (const auto& file2 : project) {
					Named named_of_file;
					{
						named_of_file.templates["Set"].push_back(&stuff_from_cpp.builtin_set);
						named_of_file.templates["Vector"].push_back(&stuff_from_cpp.builtin_vector);
						named_of_file.templates["Map"].push_back(&stuff_from_cpp.builtin_map);

						named_of_file.types["Int"].push_back(&stuff_from_cpp.builtin_int);
						named_of_file.types["Bool"].push_back(&stuff_from_cpp.builtin_bool);
						named_of_file.types["String"].push_back(&stuff_from_cpp.builtin_string);
						named_of_file.types["Void"].push_back(&stuff_from_cpp.builtin_void);

						named_of_file.functions["println"].push_back(&stuff_from_cpp.println);
						named_of_file.functions["size"].push_back(&stuff_from_cpp.size);
					}
					named_by_file[file2.filename] = named_of_file;
					insert_all_named_recursive_with_imports(project, named_by_file, file2.filename);
				}

				for (const auto& file2 : project) {
					Named& named_of_file = named_by_file[file2.filename];
					insert_aliases_recursive_with_imports(project, named_by_file, file2.filename);
				}

				transpilation_state state{ std::move(variables), Named(named_by_file[file.filename]) };
				state.traversed_functions.insert(stuff_from_cpp.println);
				state.traversed_functions.insert(stuff_from_cpp.size);
				state.traversed_types.insert(stuff_from_cpp.builtin_int);
				state.traversed_types.insert(stuff_from_cpp.builtin_bool);
				state.traversed_types.insert(stuff_from_cpp.builtin_string);
				state.traversed_types.insert(stuff_from_cpp.builtin_void);

				transpile_header_cpp_t k = transpile_main(transpilation_state_with_indent{ state, 0 }, fn);
				if (k.has_error())
					return k;

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
	auto return_type = transpile_typename_visitor{ {}, state }(fn.returnType);
	return_if_error(return_type);
	auto parameters = transpile(state, fn.parameters);
	return_if_error(parameters);
	for (auto&& [type_name, value_cat, name] : fn.parameters)
		state.state.variables[name].push_back(std::pair{ value_cat, type_of_typename_visitor{ {}, state }(type_name).value() });
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
			<< " = " << transpile_typename_visitor{ {}, state }(alias.aliasFrom).value()
			<< ";\n";
		else {
			auto err = "cannot alias to " + transpile_typename_visitor{ {}, state }(alias.aliasTo).value();
			throw std::runtime_error(err);
		}*/
	}

	for (const auto& member : type.memberVariables) {
		auto type = type_of_typename_visitor{ {}, state }(member.type);
		return_if_error(type);
		auto transpiled = transpile_typename_visitor{ {}, state }(member.type);
		return_if_error(transpiled);
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
	transpilation_state_with_indent state,
	const std::vector<std::tuple<NodeStructs::Typename, NodeStructs::ParameterCategory, std::string>>& parameters
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
	transpilation_state_with_indent state,
	const std::vector<NodeStructs::Statement>& statements
) {
	auto transpile_statement = transpile_statement_visitor{ {}, state };
	std::stringstream ss;
	for (const auto& statement : statements) {
		auto k = transpile_statement(statement);
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
					| std::views::transform([&](const auto& e) { return type_of_typename_visitor{ {}, state }(e.type).value(); })
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
					NodeStructs::UniversalType iterator_type = type_of_typename_visitor{ {}, state }(it.type).value();
					if (decomposed_types.at(i) <=> iterator_type != std::weak_ordering::equivalent) {
						auto t = transpile_typename_visitor{ {}, state }(it.type);
						return_if_error(t);
						return error{ "user error","Invalid type of iterator " + t.value() };
					}
					state.state.variables[it.name].push_back({ NodeStructs::Reference{}, type_of_typename_visitor{ {}, state }(it.type).value() });
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
				NodeStructs::UniversalType iterator_type = type_of_typename_visitor{ {}, state }(it.type).value();
				if (it_type <=> iterator_type != std::weak_ordering::equivalent) {
					auto t = transpile_typename_visitor{ {}, state }(it.type);
					return_if_error(t);
					return error{ "user error","Invalid type of iterator " + t.value() };
				}
				state.state.variables[it.name].push_back({ NodeStructs::Reference{}, type_of_typename_visitor{ {}, state }(it.type).value() });
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
	return transpile_expression_visitor{ {}, state }(std::get<NodeStructs::Expression>(arg));
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

		auto repr = transpile_expression_visitor{ {}, state }(arg);
		return_if_error(repr);
		ss << repr.value();
	}
	return ss.str();
}

expected<std::pair<NodeStructs::ParameterCategory, NodeStructs::UniversalType>> type_of_postfix_member(
	transpilation_state_with_indent state,
	const NodeStructs::Type& t,
	const std::string& property_name
) {
	const auto& v = t.memberVariables;
	auto pos = std::find_if(v.begin(), v.end(), [&](const NodeStructs::MemberVariable& m) { return m.name == property_name; });
	if (pos == v.end()) {
		auto u = transpile_type_visitor{ {}, state }(t);
		if (u.has_value())
			return error{ "user error","Error: object of type `" + std::move(u).value() + "` has no member `" + property_name + "`\n"};
		else
			return std::move(u).error();
	}
	else
		return std::pair{ NodeStructs::Value{}, type_of_typename_visitor{ {}, state }(pos->type).value() };
}

std::string _template_name(
	const NodeStructs::Expression& argument
) {
	std::visit(
		overload(overload_default_error,
			[](const NodeStructs::ConditionalExpression&) {
			},
			[](const NodeStructs::OrExpression&) {
			},
			[](const NodeStructs::AndExpression&) {
			},
			[](const NodeStructs::EqualityExpression&) {
			},
			[](const NodeStructs::CompareExpression&) {
			},
			[](const NodeStructs::AdditiveExpression&) {
			},
			[](const NodeStructs::MultiplicativeExpression&) {
			},
			[](const NodeStructs::UnaryExpression&) {
			},
			[](const NodeStructs::CallExpression&) {
			},
			[](const NodeStructs::TemplateExpression&) {
			},
			[](const NodeStructs::ConstructExpression&) {
			},
			[](const NodeStructs::BracketAccessExpression&) {
			},
			[](const NodeStructs::PropertyAccessExpression&) {
			},
			[](const NodeStructs::ParenArguments&) {
			},
			[](const NodeStructs::BraceArguments&) {
			},
			[](const std::string&) {
			},
			[](const Token<FLOATING_POINT_NUMBER>&) {
			},
			[](const Token<INTEGER_NUMBER>&) {
			},
			[](const Token<STRING>&) {
			}
		),
		argument.expression.get()
	);
	return std::string();
}

std::string _template_names(
	const std::vector<NodeStructs::Expression>& arguments
) {
	return std::string();
}

std::string template_name(std::string original_name, const std::vector<std::string>& args) {
	std::stringstream ss;
	ss << original_name;
	for (const auto& arg : args)
		ss << "_" << arg;
	return ss.str();
}

std::string template_name(std::string original_name, const std::vector<NodeStructs::Expression>& arguments) {
	return template_name(original_name, arguments | LIFT_TRANSFORM(expression_for_template_visitor{ {} }) | to_vec());
}
