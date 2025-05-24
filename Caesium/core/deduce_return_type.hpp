#include "toCPP.hpp"

template <typename context>
expected<std::optional<Realised::MetaType>> deduce_return_type(
	transpilation_state_with_indent state,
	variables_t& variables,
	const NodeStructs::VariableDeclarationStatement<context>& statement
) {
	NOT_IMPLEMENTED;
}

template <typename context>
expected<std::optional<Realised::MetaType>> deduce_return_type(
	transpilation_state_with_indent state,
	variables_t& variables,
	const NodeStructs::IfStatement<context>& statement
) {
	NOT_IMPLEMENTED;
}

template <typename context>
expected<std::optional<Realised::MetaType>> deduce_return_type(
	transpilation_state_with_indent state,
	variables_t& variables,
	const NodeStructs::ForStatement<context>& statement
) {
	NOT_IMPLEMENTED;
}

template <typename context>
expected<std::optional<Realised::MetaType>> deduce_return_type(
	transpilation_state_with_indent state,
	variables_t& variables,
	const NodeStructs::IForStatement<context>& statement
) {
	NOT_IMPLEMENTED;
}

template <typename context>
expected<std::optional<Realised::MetaType>> deduce_return_type(
	transpilation_state_with_indent state,
	variables_t& variables,
	const NodeStructs::WhileStatement<context>& statement
) {
	NOT_IMPLEMENTED;
}

template <typename context>
expected<std::optional<Realised::MetaType>> deduce_return_type(
	transpilation_state_with_indent state,
	variables_t& variables,
	const NodeStructs::BreakStatement<context>& statement
) {
	NOT_IMPLEMENTED;
}

template <typename context>
expected<std::optional<Realised::MetaType>> deduce_return_type(
	transpilation_state_with_indent state,
	variables_t& variables,
	const NodeStructs::ReturnStatement<context>& statement
) {
	if (statement.ifExpr.has_value())
		NOT_IMPLEMENTED;
	if (statement.returnExpr.size() != 1)
		NOT_IMPLEMENTED;
	if (statement.returnExpr.size() == 0)
		return expected<std::optional<Realised::MetaType>>{
		std::optional<Realised::MetaType>{
			Realised::MetaType{
				Realised::PrimitiveType{
					Realised::PrimitiveType::NonValued<Realised::void_t>{}
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
expected<std::optional<Realised::MetaType>> deduce_return_type(
	transpilation_state_with_indent state,
	variables_t& variables,
	const NodeStructs::BlockStatement<context>& statement
) {
	NOT_IMPLEMENTED;
}

template <typename context>
expected<std::optional<Realised::MetaType>> deduce_return_type(
	transpilation_state_with_indent state,
	variables_t& variables,
	const NodeStructs::MatchStatement<context>& statement
) {
	NOT_IMPLEMENTED;
}

template <typename context>
expected<std::optional<Realised::MetaType>> deduce_return_type(
	transpilation_state_with_indent state,
	variables_t& variables,
	const NodeStructs::SwitchStatement<context>& statement
) {
	NOT_IMPLEMENTED;
}

template <typename context>
expected<std::optional<Realised::MetaType>> deduce_return_type(
	transpilation_state_with_indent state,
	variables_t& variables,
	const NodeStructs::Assignment<context>& statement
) {
	NOT_IMPLEMENTED;
}

expected<std::optional<Realised::MetaType>> deduce_return_type(
	transpilation_state_with_indent state,
	variables_t& variables,
	const NodeStructs::RunTimeStatement& statement
);

template <typename context>
expected<std::optional<Realised::MetaType>> deduce_return_type(
	transpilation_state_with_indent state,
	variables_t& variables,
	const NodeStructs::Statement<context>& statement
) {
	return caesium_lib::variant::visit(statement.statement.get(), overload(
		[&](const NodeStructs::CompileTimeStatement<context>& statement) -> expected<std::optional<Realised::MetaType>> {
			NOT_IMPLEMENTED;
		},
		[&](const NodeStructs::contextual_options<context>& statement) -> expected<std::optional<Realised::MetaType>> {
			return caesium_lib::variant::visit(statement, [&](const auto& statement) {
				return deduce_return_type(state, variables, statement);
			});
		}
	));
}

template <typename context>
expected<std::optional<Realised::MetaType>> deduce_return_type(
	transpilation_state_with_indent state,
	variables_t& variables,
	const std::vector<NodeStructs::Statement<context>>& statements
) {
	std::optional<Realised::MetaType> res = std::nullopt;
	for (const auto& statement : statements) {
		expected<std::optional<Realised::MetaType>> e = deduce_return_type(state, variables, statement);
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
