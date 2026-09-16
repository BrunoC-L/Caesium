#pragma once
#include "toCPP.hpp"

template <typename T, template <typename> typename CompileTimeStatement>
expected<T> realise_one_compile_time_statement(
	transpilation_state_with_indent state,
	variables_t& variables,
	T type_or_interface,
	const CompileTimeStatement<type_context>& statement
) {
	NOT_IMPLEMENTED;
}

template <typename T>
expected<T> realise_one_compile_time_statement(
	transpilation_state_with_indent state,
	variables_t& variables,
	T type_or_interface,
	const NodeStructs::ForStatement<type_context>& statement
);

template <typename T>
expected<T> realise_one_compile_time_statement(
	transpilation_state_with_indent state,
	variables_t& variables,
	T type_or_interface,
	const NodeStructs::IForStatement<type_context>& statement
);


template <typename T>
expected<T> realise_one_compile_time_statement(
	transpilation_state_with_indent state,
	variables_t& variables,
	T type_or_interface,
	const NodeStructs::IfStatement<type_context>& statement
) {
	NOT_IMPLEMENTED;
	/*auto expr_info_or_e = transpile_expression(state, variables, statement.ifExpr);

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
		return type;*/
}

template <typename T>
expected<T> realise_one_compile_time_statement(
	transpilation_state_with_indent state,
	variables_t& variables,
	T type_or_interface,
	const NodeStructs::CompileTimeStatement<type_context>& statement
) {
	return std::visit([&](const auto& stmt) { return realise_one_compile_time_statement(state, variables, std::move(type_or_interface), stmt); }, statement._value);
}

template <typename T>
expected<T> add_member_to_type(
	T type_or_interface,
	Variant<NodeStructs::Alias, NodeStructs::MemberVariable> member
) {
	std::string new_name = caesium_lib::variant::visit(member, overload(
		[](const NodeStructs::Alias& x) -> std::string { return copy(x.name); },
		[](const NodeStructs::MemberVariable& x) -> std::string { return copy(x.name); }
	));
	for (const auto& existing : type_or_interface.members) {
		if (holds<NodeStructs::contextual_options<type_context>>(existing.statement.get())) {
			const auto& opt = get<NodeStructs::contextual_options<type_context>>(existing.statement.get());
			std::string existing_name = caesium_lib::variant::visit(opt, overload(
				[](const NodeStructs::Alias& a) -> std::string { return copy(a.name); },
				[](const NodeStructs::MemberVariable& mv) -> std::string { return copy(mv.name); }
			));
			if (existing_name == new_name)
				return error{ "user error", "Duplicate member name `" + new_name + "`" };
		}
	}
	caesium_lib::variant::visit(std::move(member), overload(
		[&](NodeStructs::Alias x) {
			auto debug_info_string = "alias name = " + copy(x.name);
			type_or_interface.members.push_back(NodeStructs::Statement<type_context>{
				NodeStructs::contextual_options<type_context>{ std::move(x) }
#ifdef DEBUG
				, std::move(debug_info_string)
#endif
			});
		},
		[&](NodeStructs::MemberVariable x) {
			auto debug_info_string = "variable name = " + copy(x.name);
			type_or_interface.members.push_back(NodeStructs::Statement<type_context>{
				NodeStructs::contextual_options<type_context>{ std::move(x) }
#ifdef DEBUG
				, std::move(debug_info_string)
#endif
			});
		}
	));
	return type_or_interface;
}

template <typename T>
select_t<
	std::is_same_v<T, NodeStructs::Type>,
	expected<NodeStructs::Type>,
	expected<NodeStructs::Interface>
> realise_many_compile_time_statements(
	transpilation_state_with_indent state,
	variables_t& variables,
	T type_or_interface,
	const std::vector<NodeStructs::CompileTimeStatement<type_context>>& statements
) {
	NOT_IMPLEMENTED;
	//if (statements.size() == 0)
	//	return type;

	//// keeping a vector of intermediates is not technically needed,
	//// we could go with a recursive approach, but this way we keep the intermediates
	//// at hand so if it leads to an error we can provide them to the user (or to the dev!)
	//auto intermediates = caesium_lib::vector::push(
	//	caesium_lib::vector::make_with_capacity<NodeStructs::Type>(statements.size() + 1),
	//	std::move(type)
	//);

	//for (const NodeStructs::CompileTimeStatement<type_context>& compile_time_statement : statements) {
	//	variables_t variables = make_base_variables();
	//	expected<NodeStructs::Type> next = caesium_lib::variant::visit(compile_time_statement, [&](const auto& compile_time_statement) {
	//		return realise_one_compile_time_statement(state, variables, copy(intermediates._value.back()), compile_time_statement);
	//	});
	//	return_if_error(next);
	//	intermediates = caesium_lib::vector::push(
	//		std::move(intermediates),
	//		std::move(next).value()
	//	);
	//}

	//const auto& finished = intermediates._value.back();
	//NOT_IMPLEMENTED;
	////return Realised::Type{ finished.name, copy(finished.aliases), copy(finished.member_variables), finished.info };
}

template <typename T>
select_t<
	std::is_same_v<T, NodeStructs::Type>,
	expected<NodeStructs::Type>,
	expected<NodeStructs::Interface>
> realise_many_compile_time_statements(
	transpilation_state_with_indent state,
	variables_t& variables,
	T type_or_interface,
	const std::vector<NodeStructs::Statement<type_context>>& statements
) {
	if (statements.size() == 0)
		return type_or_interface;

	// keeping a vector of intermediates is not technically needed,
	// we could go with a recursive approach, but this way we keep the intermediates
	// at hand so if it leads to an error we can provide them to the user (or to the dev!)
	auto intermediates = caesium_lib::vector::push(
		caesium_lib::vector::make_with_capacity<T>(statements.size() + 1),
		std::move(type_or_interface)
	);

	for (const NodeStructs::Statement<type_context>& compile_time_statement : statements) {
		expected<T> next = caesium_lib::variant::visit(compile_time_statement.statement.get(), overload(
			[&](const Variant<NodeStructs::Alias, NodeStructs::MemberVariable>& member) -> expected<T> {
				return add_member_to_type(copy(intermediates._value.back()), copy(member));
			},
			[&](const NodeStructs::CompileTimeStatement<type_context>& compile_time_statement) -> expected<T> {
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

// Shared utility for compile-time for/ifor over a TypeListType:
// injects the type iterator name into state.state.types for each element,
// optionally injects an index variable (ifor), runs the body statements, then cleans up.
// MemberVariable typenames are resolved eagerly (while the iterator is in scope) and
// stored under unique keys so type_of_typename can find them after the loop.
template <typename T>
expected<T> realise_compile_time_for_over_type_list(
	transpilation_state_with_indent state,
	variables_t& variables,
	T type_or_interface,
	const Realised::TypeListType& tl,
	const std::string& type_iterator_name,
	const std::vector<NodeStructs::Statement<type_context>>& statements,
	std::optional<std::string> index_iterator_name = std::nullopt
) {
	auto intermediates = caesium_lib::vector::push(
		caesium_lib::vector::make_with_capacity<T>(tl.types.size() + 1),
		std::move(type_or_interface)
	);
	int i = 0;
	for (const Realised::MetaType& type : tl.types) {
		state.state.types.erase(type_iterator_name);
		state.state.types.insert({ type_iterator_name, copy(type) });
		if (index_iterator_name.has_value())
			variables[index_iterator_name.value()].push_back(variable_info{
				NodeStructs::ValueCategory{ NodeStructs::Value{} },
				Realised::MetaType{ Realised::PrimitiveType{ Realised::PrimitiveType::Valued<int>{ i } } }
			});

		// Process body statements like realise_many_compile_time_statements, but resolve
		// MemberVariable typenames eagerly while the type iterator is still in scope.
		// Run inside a lambda so return_if_error exits the lambda (not the outer function),
		// allowing cleanup to happen unconditionally after.
		expected<T> body_result = [&]() -> expected<T> {
			auto body_intermediates = caesium_lib::vector::push(
				caesium_lib::vector::make_with_capacity<T>(statements.size() + 1),
				copy(intermediates._value.back())
			);
			for (const NodeStructs::Statement<type_context>& stmt : statements) {
				expected<T> next = caesium_lib::variant::visit(stmt.statement.get(), overload(
					[&](const Variant<NodeStructs::Alias, NodeStructs::MemberVariable>& member) -> expected<T> {
						return caesium_lib::variant::visit(member, overload(
							[&](const NodeStructs::Alias& alias) -> expected<T> {
								return add_member_to_type(copy(body_intermediates._value.back()), copy(member));
							},
							[&](const NodeStructs::MemberVariable& mv) -> expected<T> {
								// Resolve the member type NOW while type_iterator_name is in state.state.types
								auto resolved_or_e = type_of_typename(state, variables, mv.type);
								return_if_error(resolved_or_e);
								// Store under a unique key so type_of_typename can find it after the loop
								std::string uid = "__for_iter_type_" + std::to_string(state.state.current_variable_unique_id++);
								state.state.types.insert({ uid, copy(resolved_or_e.value()) });
								return add_member_to_type(
									copy(body_intermediates._value.back()),
									Variant<NodeStructs::Alias, NodeStructs::MemberVariable>{
										NodeStructs::MemberVariable{
											NodeStructs::Typename{ NodeStructs::BaseTypename{ uid }, std::nullopt, rule_info_stub_no_throw() },
											copy(mv.name)
										}
									}
								);
							}
						));
					},
					[&](const NodeStructs::CompileTimeStatement<type_context>& ct) -> expected<T> {
						return realise_one_compile_time_statement(state, variables, copy(body_intermediates._value.back()), ct);
					}
				));
				return_if_error(next);
				body_intermediates = caesium_lib::vector::push(std::move(body_intermediates), std::move(next).value());
			}
			return std::move(body_intermediates._value.back());
		}();

		state.state.types.erase(type_iterator_name);
		if (index_iterator_name.has_value())
			variables[index_iterator_name.value()].pop_back();

		return_if_error(body_result);
		intermediates = caesium_lib::vector::push(std::move(intermediates), std::move(body_result).value());
		++i;
	}
	return std::move(intermediates._value.back());
}

template <typename T>
expected<T> realise_one_compile_time_statement(
	transpilation_state_with_indent state,
	variables_t& variables,
	T type_or_interface,
	const NodeStructs::ForStatement<type_context>& statement
) {
	transpile_expression_information_t expr_or_error = transpile_expression(state, variables, statement.collection);
	return_if_error(expr_or_error);
	if (!holds<type_information>(expr_or_error.value()))
		return error{ "user error", "For collection must be a type list" };
	const Realised::MetaType& coll_type = get<type_information>(expr_or_error.value()).type;
	if (!holds<Realised::TypeListType>(coll_type))
		NOT_IMPLEMENTED;
	if (statement.iterators.size() != 1 || !std::holds_alternative<std::string>(statement.iterators[0]._value))
		NOT_IMPLEMENTED;
	return realise_compile_time_for_over_type_list(
		state, variables, std::move(type_or_interface),
		get<Realised::TypeListType>(coll_type),
		std::get<std::string>(statement.iterators[0]._value),
		statement.statements,
		std::nullopt
	);
}

template <typename T>
expected<T> realise_one_compile_time_statement(
	transpilation_state_with_indent state,
	variables_t& variables,
	T type_or_interface,
	const NodeStructs::IForStatement<type_context>& statement
) {
	transpile_expression_information_t expr_or_error = transpile_expression(state, variables, statement.for_statement.collection);
	return_if_error(expr_or_error);
	if (!holds<type_information>(expr_or_error.value()))
		return error{ "user error", "IFor collection must be a type list" };
	const Realised::MetaType& coll_type = get<type_information>(expr_or_error.value()).type;
	if (!holds<Realised::TypeListType>(coll_type))
		NOT_IMPLEMENTED;
	if (statement.for_statement.iterators.size() != 1 || !std::holds_alternative<std::string>(statement.for_statement.iterators[0]._value))
		NOT_IMPLEMENTED;
	return realise_compile_time_for_over_type_list(
		state, variables, std::move(type_or_interface),
		get<Realised::TypeListType>(coll_type),
		std::get<std::string>(statement.for_statement.iterators[0]._value),
		statement.for_statement.statements,
		statement.index_iterator
	);
}

template <typename T>
select_t<
	std::is_same_v<T, NodeStructs::Type>,
	expected<std::pair<NodeStructs::Type, Realised::Type>>,
	expected<std::pair<NodeStructs::Interface, Realised::Interface>>
> realise_type_or_interface_unchecked(
	transpilation_state_with_indent state,
	const T& type_or_interface,
	const std::string& name
) {
	using NT = select_t<
		std::is_same_v<T, NodeStructs::Type>,
		NodeStructs::Type,
		NodeStructs::Interface
	>;
	using RT = select_t<
		std::is_same_v<T, NodeStructs::Type>,
		Realised::Type,
		Realised::Interface
	>;
	NT without_members{
		.name = name,
		.name_space = copy(type_or_interface.name_space),
		.members = {},
		.info = copy(type_or_interface.info)
	};

	variables_t variables = make_base_variables();
	state.state.types_traversal.traversing.insert(name);
	expected<NT> rt_or_e = realise_many_compile_time_statements<NT>(state, variables, std::move(without_members), type_or_interface.members);
	state.state.types_traversal.traversing.erase(name);
	return_if_error(rt_or_e);

	NT realised_type = std::move(rt_or_e).value();

	std::vector<std::pair<std::string, expected<Realised::MetaType>>> expected_types = realised_type.members
		| std::views::filter([&](const NodeStructs::Statement<type_context>& member) -> bool {
			return holds<NodeStructs::contextual_options<type_context>>(member);
		})
		| std::views::transform([&](const NodeStructs::Statement<type_context>& member) -> std::reference_wrapper<const NodeStructs::contextual_options<type_context>> {
			return get<NodeStructs::contextual_options<type_context>>(member);
		})
		| std::views::filter([&](std::reference_wrapper<const NodeStructs::contextual_options<type_context>> member) -> bool {
			return holds<NodeStructs::MemberVariable>(member.get());
		})
		| std::views::transform([&](std::reference_wrapper<const NodeStructs::contextual_options<type_context>> member) -> const NodeStructs::MemberVariable& {
			return get<NodeStructs::MemberVariable>(member.get());
		})
		| std::views::transform([&](const NodeStructs::MemberVariable& member) -> std::pair<std::string, expected<Realised::MetaType>> {
			return std::pair{ copy(member.name), type_of_typename(state, variables, member.type) };
		})
		| to_vec();

	for (const std::pair<std::string, expected<Realised::MetaType>>& member : expected_types)
		if (member.second.has_error())
			return member.second.error();

	RT res{
		.name = copy(realised_type.name),
		.member_variables = expected_types
		| std::views::transform([&](std::pair<std::string, expected<Realised::MetaType>>& member) -> Realised::MemberVariable {
			return { std::move(member.first), std::move(member.second).value() };
		})
		| to_vec(),
		.info = copy(realised_type.info)
	};

	return std::pair<NT, RT>{ std::move(realised_type), std::move(res) };
}

template <typename T>
select_t<
	std::is_same_v<T, NodeStructs::Type>,
	expected<std::pair<NodeStructs::Type, Realised::Type>>,
	expected<std::pair<NodeStructs::Interface, Realised::Interface>>
> realise_type_or_interface_unchecked(
	transpilation_state_with_indent state,
	const T& type_or_interface
) {
	transpile_t name_or_e = [&]() -> transpile_t {
		if (type_or_interface.name_space.has_value()) {
			auto ns_or_e = name_of_namespace(type_or_interface.name_space.value());
			return_if_error(ns_or_e);
			return std::move(ns_or_e).value() + "__" + type_or_interface.name;
		}
		else
			return type_or_interface.name;
	}();
	return_if_error(name_or_e);
	return realise_type_or_interface_unchecked(state, type_or_interface, name_or_e.value());
}

template <typename T>
select_t<
	std::is_same_v<NodeStructs::Type, T>,
	expected<Realised::Type>,
	expected<Realised::Interface>
> realise_type_or_interface(
	transpilation_state_with_indent state,
	const T& type_or_interface
) {
	using TrT = select_t<
		std::is_same_v<NodeStructs::Type, T>,
		traversal<Realised::Type>,
		traversal<Realised::Interface>
	>;
	TrT& traversal_ = [&]() -> TrT& {
		if constexpr (std::is_same_v<NodeStructs::Type, T>) {
			return state.state.types_traversal;
		}
		else if constexpr (std::is_same_v<NodeStructs::Interface, T>) {
			return state.state.interfaces_traversal;
		}
		else {
			NOT_IMPLEMENTED;
		}
	}();

	transpile_t name_or_e = [&]() -> transpile_t {
		if (type_or_interface.name_space.has_value()) {
			auto ns_or_e = name_of_namespace(type_or_interface.name_space.value());
			return_if_error(ns_or_e);
			return std::move(ns_or_e).value() + "__" + type_or_interface.name;
		}
		else {
			return type_or_interface.name;
		}
	}();
	return_if_error(name_or_e);
	const std::string& name = name_or_e.value();

	if (traversal_.traversing.contains(name))
		NOT_IMPLEMENTED; // return traversed, how?
	if (traversal_.traversed.count(name))
		return copy(traversal_.traversed.at(name));

	auto types_or_e = realise_type_or_interface_unchecked(state, type_or_interface, name);
	if (types_or_e.has_error())
		return std::move(types_or_e).error();
	auto [nt, rt] = std::move(types_or_e).value();

	auto declaration_definition_or_e = transpile(state, std::move(nt));
	if (declaration_definition_or_e.has_error())
		return std::move(declaration_definition_or_e).error();
	auto [declaration, definition] = declaration_definition_or_e.value();

	traversal_.traversed.insert({ name, copy(rt) });
	traversal_.declarations.push_back(std::move(declaration));
	traversal_.definitions.push_back(std::move(definition));

	return std::move(rt);
}

expected<Realised::Type> get_existing_realised_type(
	transpilation_state_with_indent state,
	const std::string& name,
	const std::optional<NodeStructs::NameSpace>& name_space
);
