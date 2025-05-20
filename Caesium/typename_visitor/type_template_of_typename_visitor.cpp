#include "../core/toCPP.hpp"
#include "../structured/structurizer.hpp"
#include "../utility/replace_all.hpp"
#include "../utility/vec_of_expected_to_expected_of_vec.hpp"

using T = type_template_of_typename_visitor;
using R = T::R;

R f(
	transpilation_state_with_indent state,
	variables_t& variables,
	const std::vector<NodeStructs::WordTypenameOrExpression>& templated_with, const std::string& name_to_find,
	const NodeStructs::NameSpace& ns
) {
	NOT_IMPLEMENTED;
	//if (auto it = ns.templates.find(name_to_find); it != ns.templates.end()) {
	//	const auto& templates = it->second;

	//	// todo check if already traversed
	//	NOT_IMPLEMENTED;
	//	auto t = find_best_template(state, variables, templates, templated_with);
	//	return_if_error(t);
	//	const auto& tmpl = t.value().tmpl.get();
	//	const auto& arg_placements = t.value().arg_placements;
	//	auto grab_nth_with_commas = [&](size_t i) -> expected<std::string> {
	//		std::stringstream ss;
	//		bool has_previous = false;
	//		for (size_t j = 0; j < arg_placements.size(); ++j) {
	//			size_t k = arg_placements.at(j);
	//			if (k == i) {
	//				const auto& arg = word_typename_or_expression_for_template(state, variables, templated_with.at(j));
	//				return_if_error(arg);
	//				if (has_previous)
	//					ss << ", ";
	//				has_previous = true;
	//				ss << arg.value();
	//			}
	//		}
	//		return ss.str();
	//	};
	//	auto args_or_e = vec_of_expected_to_expected_of_vec(templated_with
	//		| std::views::transform([&](auto&& tn) { return word_typename_or_expression_for_template(state, variables, tn); })
	//		| to_vec());
	//	return_if_error(args_or_e);
	//	const auto& args = args_or_e.value();
	//	auto ex1 = NodeStructs::TemplateExpression{
	//		.operand = NodeStructs::Expression{ .expression = name_to_find, .info = rule_info{.file_name = "todo:/", .content = name_to_find } },
	//		.args = copy(args)
	//		| std::views::transform([&](const std::string& s) { return NodeStructs::WordTypenameOrExpression(s); })
	//		| to_vec()
	//	};
	//	auto ex2 = NodeStructs::WordTypenameOrExpression{ NodeStructs::Expression{
	//		.expression = std::move(ex1),
	//		.info = rule_info{.file_name = "todo:/", .content = name_to_find }
	//	} };
	//	auto tmpl_name_or_e = word_typename_or_expression_for_template(state, variables, std::move(ex2));
	//	return_if_error(tmpl_name_or_e);
	//	const auto& tmpl_name = tmpl_name_or_e.value();
	//	// todo check if exists
	//	if (auto it = state.state.global_namespace.types.find(tmpl_name); it != state.state.global_namespace.types.end()) {
	//		const auto& types = it->second;
	//		if (types.size() != 1)
	//			NOT_IMPLEMENTED;
	//		NOT_IMPLEMENTED;
	//		//return Realised::MetaType{ copy(types.at(0)) };
	//	}
	//	if (auto it = state.state.global_namespace.functions.find(tmpl_name); it != state.state.global_namespace.functions.end()) {
	//		const auto& fns = it->second;
	//		if (fns.size() != 1)
	//			NOT_IMPLEMENTED;
	//		NOT_IMPLEMENTED;
	//		return Realised::MetaType{ Realised::FunctionType{ tmpl_name /*, todo overload set*/}};
	//	}

	//	std::string replaced = tmpl.templated;
	//	for (size_t i = 0; i < tmpl.parameters.size(); ++i) {
	//		auto val = grab_nth_with_commas(i);
	//		return_if_error(val);
	//		replaced = replace_all(
	//			std::move(replaced),
	//			std::visit(overload(
	//				[](const auto& e) { return "`" + e.name + "`"; },
	//				[](const NodeStructs::VariadicTemplateParameter& e) { return "`" + e.name + "...`"; }
	//			), tmpl.parameters.at(i)._value),
	//			val.value()
	//		);
	//	}
	//	{
	//		And<IndentToken, grammar::Function, Token<END>> f{ tmpl.indent };
	//		auto tokens = Tokenizer(replaced).read();
	//		Iterator it = { .vec = tokens, .index = 0 , .line = 0, .col = 0, .file_name = "template:/" + tmpl_name };
	//		if (build(f, it)) {
	//			auto structured_f = getStruct("template:/" + tmpl_name, tokens, f.get<grammar::Function>(), std::nullopt);
	//			structured_f.name = tmpl_name;
	//			if (uses_auto(structured_f)) {
	//				NOT_IMPLEMENTED;
	//				//state.state.global_namespace.functions_using_auto[tmpl_name].push_back(std::move(structured_f));
	//				//return Realised::MetaType{ NodeStructs::FunctionType{ tmpl_name } };
	//			}
	//			else {
	//				state.state.global_namespace.functions[tmpl_name].push_back(copy(structured_f));
	//				NOT_IMPLEMENTED;
	//				/*state.state.functions_traversal.traversed.insert(copy(structured_f));
	//				auto transpiled_f = transpile(state.unindented(), structured_f);
	//				return_if_error(transpiled_f);
	//				if (uses_auto(structured_f))
	//					NOT_IMPLEMENTED;
	//				state.state.functions_to_transpile.insert(std::move(structured_f));

	//				return Realised::MetaType{ NodeStructs::FunctionType{ tmpl_name, state.state.global_namespace } };*/
	//			}
	//		}
	//	}
	//	{
	//		And<IndentToken, grammar::Type> t{ tmpl.indent };
	//		auto tokens = Tokenizer(replaced).read();
	//		Iterator it = { .vec = tokens, .index = 0 , .line = 0, .col = 0, .file_name = "template:/" + tmpl_name };
	//		auto ok = build(t, it);
	//		while (parse_empty_line(it));
	//		if (ok && (it.index == it.vec.size() || it.vec[it.index].first == END)) {
	//			auto structured_t = getStruct("template:/" + tmpl_name, tokens, t.get<grammar::Type>(), std::nullopt);
	//			structured_t.name = tmpl_name;
	//			state.state.global_namespace.types[structured_t.name].push_back(copy(structured_t));
	//			// if exists 
	//			// template <typename T> using `tmplname` = `actual exists name somehow?`<T>;
	//			NOT_IMPLEMENTED;
	//			/*auto opt_error = traverse_type(state, structured_t);
	//			if (opt_error.has_value())
	//				return opt_error.value();
	//			return Realised::MetaType{ std::move(structured_t) };*/
	//		}
	//	}
	//	{
	//		And<IndentToken, grammar::Typename> tn{ tmpl.indent };
	//		auto tokens = Tokenizer(replaced).read();
	//		Iterator it = { .vec = tokens, .index = 0 , .line = 0, .col = 0, .file_name = "template:/" + tmpl_name };
	//		auto ok = build(tn, it);
	//		while (parse_empty_line(it));
	//		if (ok && (it.index == it.vec.size() || it.vec[it.index].first == END)) {
	//			auto structured_tn = getStruct("template:/" + tmpl_name, tokens, tn.get<grammar::Typename>(), tag_allow_value_category_or_empty{});
	//			return type_of_typename(state, variables, structured_tn);
	//		}
	//	}
	//	return error{
	//		"user error",
	//		"Template expansion does not result in a type or function: |begin|\n" + replaced + "\n|end|"
	//	};
	//}
	//else
	//	return error{
	//		"user error",
	//		"Template not found `" + name_to_find + "`"
	//	};
}

R T::operator()(const NodeStructs::BaseTypename& t) {
	if (t.type == "Vector") {
		auto x = type_of_typename(state, variables, templated_with.at(0));
		return_if_error(x);
		return Realised::MetaType{ Realised::VectorType{
			.name = "TODO type_template_of_typename", // todo TODO
			.value_type = NonCopyableBox<Realised::MetaType>{ std::move(x).value() }
		} };
	}
	if (t.type == "Set") {
		auto x = type_of_typename(state, variables, templated_with.at(0));
		return_if_error(x);
		return Realised::MetaType{ Realised::SetType{ "TODO??", NonCopyableBox<Realised::MetaType>{ std::move(x).value() }}};
	}
	if (t.type == "Map") {
		auto k = type_of_typename(state, variables, templated_with.at(0));
		return_if_error(k);
		auto v = type_of_typename(state, variables, templated_with.at(1));
		return_if_error(v);
		NOT_IMPLEMENTED;
		/*return Realised::MetaType{ Realised::MapType{
			NonCopyableBox<Realised::MetaType>{ std::move(k).value() },
			NonCopyableBox<Realised::MetaType>{ std::move(v).value() }
		} };*/
	}
	if (t.type == "Union") {
		auto ts = vec_of_expected_to_expected_of_vec(templated_with
			| std::views::transform([&](auto&& tn) { return type_of_typename(state, variables, tn); })
			| to_vec());
		return_if_error(ts);
		return Realised::MetaType{ Realised::UnionType{
			"TODO??",
			std::move(ts).value()
		} };
	}
	return f(state, variables, templated_with, t.type, state.state.global_namespace);
}

R T::operator()(const NodeStructs::NamespacedTypename& t) {
	auto ns_or_e = type_of_typename(state, variables, t.name_space);
	return_if_error(ns_or_e);
	if (!std::holds_alternative<Realised::NamespaceType>(ns_or_e.value().type.get()._value))
		NOT_IMPLEMENTED;
	return f(state, variables, templated_with, t.name_in_name_space, std::get<Realised::NamespaceType>(ns_or_e.value().type.get()._value).name_space.get());
}

R T::operator()(const NodeStructs::TemplatedTypename& t) {
	NOT_IMPLEMENTED;
	//const auto& templated = operator()(t.type);
	/*return NodeStructs::TypeCategory{ NodeStructs::TypeTemplateInstanceType{
		::template_type_of_typename_visitor{ {}, state }(type.type),
		type.templated_with
			| std::views::transform([&](const auto& e) { return type_of_typename(state, e); })
			| to_vec()
	} };*/
}

R T::operator()(const NodeStructs::OptionalTypename& t) {
	NOT_IMPLEMENTED;
}

R T::operator()(const NodeStructs::UnionTypename& t) {
	NOT_IMPLEMENTED;
}

R T::operator()(const NodeStructs::VariadicExpansionTypename& t) {
	NOT_IMPLEMENTED;
}
