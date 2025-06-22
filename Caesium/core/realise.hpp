#pragma once
#include "toCPP.hpp"

template <typename T, template <typename> typename CompileTimeStatement>
expected<T> realise_one_compile_time_statement(
	transpilation_state_with_indent state,
	variables_t& variables,
	T t,
	const CompileTimeStatement<type_context>& statement
) {
	NOT_IMPLEMENTED;
}

template <typename T>
expected<T> realise_one_compile_time_statement(
	transpilation_state_with_indent state,
	variables_t& variables,
	T type,
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
	T t,
	const NodeStructs::CompileTimeStatement<type_context>& statement
) {
	return std::visit([&](const auto& stmt) { return realise_one_compile_time_statement(state, variables, std::move(t), stmt); }, statement._value);
}

template <typename T>
T add_member_to_type(
	T type,
	Variant<NodeStructs::Alias, NodeStructs::MemberVariable> member
) {
	caesium_lib::variant::visit(std::move(member), [&](auto m) {
		type.members.push_back(NodeStructs::Statement<type_context>{ NodeStructs::contextual_options<type_context>{ std::move(m) } });
	});
	return type;
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
		variables_t variables = make_base_variables();
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
