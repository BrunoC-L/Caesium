#include "core/toCPP.hpp"
#include "expression_visitor/transpile_expression_visitor.hpp"

static caesium_source_location tests_rule_info_stub() {
	return {
		"tests_rule_info_stub",
		{ 0, 0 },
		{ 0, 0 }
	};
}

static caesium_source_location tests_rule_info_simple(const std::string& content) {
	return {
		"tests_rule_info_simple",
		std::move(content),
		{ 0, 0 },
		{ 0, 0 }
	};
}

bool test_assigning_valued_Int_to_Floating() {
	transpilation_state state_(NodeStructs::NameSpace{ .info = tests_rule_info_stub() });
	transpilation_state_with_indent state{ state_, 0 };
	variables_t variables;

	auto param_t_or_e = type_of_typename(state, variables, Floating_tn);
	auto arg_t_or_e = type_of_typename(state, variables, Int_tn);

	if (param_t_or_e.has_error() || !holds<Realised::PrimitiveType>(param_t_or_e.value()))
		return false;
	if (arg_t_or_e.has_error() || !holds<Realised::PrimitiveType>(arg_t_or_e.value()))
		return false;

	Realised::MetaType parameter_type = std::move(param_t_or_e).value();
	Realised::MetaType argument_type = std::move(arg_t_or_e).value();

	{
		auto result = primitives_assignable(get<Realised::PrimitiveType>(parameter_type), get<Realised::PrimitiveType>(argument_type));
		if (result)
			return false;
	}
	{
		auto result = assigned_to(state, variables, parameter_type, argument_type);
		return !holds<directly_assignable>(result);
	}
}

bool test_parse_expression_types() {
	// `1` should be Valued<int>{1}
	{
		std::string program = "1";
		auto tokens = Tokenizer(program).read();
		Iterator it = { .vec = tokens, .index = 0, .line = 0, .col = 0, .file_name = "unit tests builtin" };
		grammar::Expression node(0);
		if (!build(node, it))
			return false;
		NodeStructs::Expression expr = getExpressionStruct("test", tokens, node);

		transpilation_state state_(NodeStructs::NameSpace{ .info = tests_rule_info_stub() });
		transpilation_state_with_indent state{ state_, 0 };
		variables_t variables;

		auto result = transpile_expression(state, variables, expr);
		if (result.has_error())
			return false;
		if (!std::holds_alternative<non_type_information>(result.value()))
			return false;
		const auto& info = std::get<non_type_information>(result.value());
		if (!holds<Realised::PrimitiveType>(info.type))
			return false;
		const auto& pt = get<Realised::PrimitiveType>(info.type);
		if (!holds<Realised::PrimitiveType::Valued<int>>(pt.value))
			return false;
		if (get<Realised::PrimitiveType::Valued<int>>(pt.value).value != 1)
			return false;
	}

	// `1+1` should be Valued<int>{2}
	{
		std::string program = "1+1";
		auto tokens = Tokenizer(program).read();
		Iterator it = { .vec = tokens, .index = 0, .line = 0, .col = 0, .file_name = "unit tests builtin" };
		grammar::Expression node(0);
		if (!build(node, it))
			return false;
		NodeStructs::Expression expr = getExpressionStruct("test", tokens, node);

		transpilation_state state_(NodeStructs::NameSpace{ .info = tests_rule_info_stub() });
		transpilation_state_with_indent state{ state_, 0 };
		variables_t variables;

		auto result = transpile_expression(state, variables, expr);
		if (result.has_error())
			return false;
		if (!std::holds_alternative<non_type_information>(result.value()))
			return false;
		const auto& info = std::get<non_type_information>(result.value());
		if (!holds<Realised::PrimitiveType>(info.type))
			return false;
		const auto& pt = get<Realised::PrimitiveType>(info.type);
		if (!holds<Realised::PrimitiveType::Valued<int>>(pt.value))
			return false;
		if (get<Realised::PrimitiveType::Valued<int>>(pt.value).value != 2)
			return false;
	}

	// `3.141592` should be Valued<double>{3.141592}
	{
		std::string program = "3.141592";
		auto tokens = Tokenizer(program).read();
		Iterator it = { .vec = tokens, .index = 0, .line = 0, .col = 0, .file_name = "unit tests builtin" };
		grammar::Expression node(0);
		if (!build(node, it))
			return false;
		NodeStructs::Expression expr = getExpressionStruct("test", tokens, node);

		transpilation_state state_(NodeStructs::NameSpace{ .info = tests_rule_info_stub() });
		transpilation_state_with_indent state{ state_, 0 };
		variables_t variables;

		auto result = transpile_expression(state, variables, expr);
		if (result.has_error())
			return false;
		if (!std::holds_alternative<non_type_information>(result.value()))
			return false;
		const auto& info = std::get<non_type_information>(result.value());
		if (!holds<Realised::PrimitiveType>(info.type))
			return false;
		const auto& pt = get<Realised::PrimitiveType>(info.type);
		if (!holds<Realised::PrimitiveType::Valued<double>>(pt.value))
			return false;
		if (get<Realised::PrimitiveType::Valued<double>>(pt.value).value != 3.141592)
			return false;
	}

	// `3.141592 - 2.71828` should be Valued<double>{3.141592 - 2.71828}
	{
		std::string program = "3.141592 - 2.71828";
		auto tokens = Tokenizer(program).read();
		Iterator it = { .vec = tokens, .index = 0, .line = 0, .col = 0, .file_name = "unit tests builtin" };
		grammar::Expression node(0);
		if (!build(node, it))
			return false;
		NodeStructs::Expression expr = getExpressionStruct("test", tokens, node);

		transpilation_state state_(NodeStructs::NameSpace{ .info = tests_rule_info_stub() });
		transpilation_state_with_indent state{ state_, 0 };
		variables_t variables;

		auto result = transpile_expression(state, variables, expr);
		if (result.has_error())
			return false;
		if (!std::holds_alternative<non_type_information>(result.value()))
			return false;
		const auto& info = std::get<non_type_information>(result.value());
		if (!holds<Realised::PrimitiveType>(info.type))
			return false;
		const auto& pt = get<Realised::PrimitiveType>(info.type);
		if (!holds<Realised::PrimitiveType::Valued<double>>(pt.value))
			return false;
		if (get<Realised::PrimitiveType::Valued<double>>(pt.value).value != 3.141592 - 2.71828)
			return false;
	}

	// `3.141592 - 1` should be Valued<double>{3.141592 - 1}
	{
		std::string program = "3.141592 - 1";
		auto tokens = Tokenizer(program).read();
		Iterator it = { .vec = tokens, .index = 0, .line = 0, .col = 0, .file_name = "unit tests builtin" };
		grammar::Expression node(0);
		if (!build(node, it))
			return false;
		NodeStructs::Expression expr = getExpressionStruct("test", tokens, node);

		transpilation_state state_(NodeStructs::NameSpace{ .info = tests_rule_info_stub() });
		transpilation_state_with_indent state{ state_, 0 };
		variables_t variables;

		auto result = transpile_expression(state, variables, expr);
		if (result.has_error())
			return false;
		if (!std::holds_alternative<non_type_information>(result.value()))
			return false;
		const auto& info = std::get<non_type_information>(result.value());
		if (!holds<Realised::PrimitiveType>(info.type))
			return false;
		const auto& pt = get<Realised::PrimitiveType>(info.type);
		if (!holds<Realised::PrimitiveType::Valued<double>>(pt.value))
			return false;
		if (get<Realised::PrimitiveType::Valued<double>>(pt.value).value != 3.141592 - 1)
			return false;
	}

	return true;
}

bool unit_tests() {
	bool success = true;
	success &= test_assigning_valued_Int_to_Floating();
	success &= test_parse_expression_types();
	return success;
}