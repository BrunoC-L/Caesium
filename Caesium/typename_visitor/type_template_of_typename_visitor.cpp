#include "../core/toCPP.hpp"
#include "../core/structurizer.hpp"
#include "../utility/replace_all.hpp"
#include "../utility/vec_of_expected_to_expected_of_vec.hpp"

using T = type_template_of_typename_visitor;
using R = T::R;

R T::operator()(const NodeStructs::BaseTypename& t) {
	if (auto it = state.state.global_namespace.builtins.find(t.type); it != state.state.global_namespace.builtins.end()) {
		if (t.type == "Vector") {
			auto x = type_of_typename(state, templated_with.at(0));
			return_if_error(x);
			return NodeStructs::MetaType{ NodeStructs::VectorType{ NonCopyableBox<NodeStructs::MetaType>{ std::move(x).value() } } };
		}
		if (t.type == "Set") {
			auto x = type_of_typename(state, templated_with.at(0));
			return_if_error(x);
			return NodeStructs::MetaType{ NodeStructs::SetType{ NonCopyableBox<NodeStructs::MetaType>{ std::move(x).value() } } };
		}
		if (t.type == "Map") {
			auto k = type_of_typename(state, templated_with.at(0));
			return_if_error(k);
			auto v = type_of_typename(state, templated_with.at(1));
			return_if_error(v);
			return NodeStructs::MetaType{ NodeStructs::MapType{
				NonCopyableBox<NodeStructs::MetaType>{ std::move(k).value() },
				NonCopyableBox<NodeStructs::MetaType>{ std::move(v).value() }
			} };
		}
		if (t.type == "Variant") {
			auto ts = vec_of_expected_to_expected_of_vec(templated_with
				| std::views::transform([&](auto&& tn) { return type_of_typename(state, tn); })
				| to_vec());
			return_if_error(ts);
			return NodeStructs::MetaType{ NodeStructs::UnionType{
				std::move(ts).value()
			} };
		}
		throw;
	}
	if (auto it = state.state.global_namespace.templates.find(t.type); it != state.state.global_namespace.templates.end()) {
		const auto& templates = it->second;

		// todo check if already traversed
		auto t = find_best_template(templates, templated_with);
		return_if_error(t);
		const auto& tmpl = t.value().tmpl.get();
		const auto& arg_placements = t.value().arg_placements;
		auto grab_nth_with_commas = [&](size_t i) {
			std::stringstream ss;
			bool has_previous = false;
			for (size_t j = 0; j < arg_placements.size(); ++j) {
				size_t k = arg_placements.at(j);
				if (k == i) {
					const auto& arg = typename_for_template(templated_with.at(j));
					if (has_previous)
						ss << ", ";
					has_previous = true;
					ss << arg;
				}
			}
			return ss.str();
		};
		auto args = vec_of_expected_to_expected_of_vec(templated_with 
			| std::views::transform([&](auto&& tn) { return transpile_typename(state, tn); })
			| to_vec());
		return_if_error(args);
		const auto& args_ok = args.value();
		std::string tmpl_name = template_name(it->first, args_ok);
		// todo check if exists
		if (auto it = state.state.global_namespace.types.find(tmpl_name); it != state.state.global_namespace.types.end()) {
			const auto& types = it->second;
			if (types.size() != 1)
				throw;
			return NodeStructs::MetaType{ copy(types.at(0)) };
		}
		if (auto it = state.state.global_namespace.functions.find(tmpl_name); it != state.state.global_namespace.functions.end()) {
			const auto& fns = it->second;
			if (fns.size() != 1)
				throw;
			return NodeStructs::MetaType{ NodeStructs::FunctionType{ tmpl_name, state.state.global_namespace } };
		}

		std::string replaced = tmpl.templated;
		for (size_t i = 0; i < tmpl.parameters.size(); ++i) {
			replaced = replace_all(
				std::move(replaced),
				std::visit(overload(
					[](const auto& e) { return "`" + e.name + "`"; },
					[](const NodeStructs::VariadicTemplateParameter& e) { return "`" + e.name + "...`"; }
				), tmpl.parameters.at(i)._value),
				grab_nth_with_commas(i)
			);
		}
		{
			And<IndentToken, grammar::Function, Token<END>> f{ 1 };
			auto tokens = Tokenizer(replaced).read();
			tokens_and_iterator g{ tokens, tokens.begin() };
			if (build(f, g.it)) {
				auto structured_f = getStruct(f.get<grammar::Function>(), std::nullopt);
				structured_f.name = tmpl_name;
				if (uses_auto(structured_f)) {
					state.state.global_namespace.functions_using_auto[tmpl_name].push_back(std::move(structured_f));
					//return NodeStructs::MetaType{ NodeStructs::FunctionType{ tmpl_name } };
					throw;
				}
				else {
					// TODO VERIFY
					state.state.global_namespace.functions[tmpl_name].push_back(copy(structured_f));
					state.state.traversed_functions.insert(copy(structured_f));
					state.state.functions_to_transpile.insert(std::move(structured_f));
					return NodeStructs::MetaType{ NodeStructs::FunctionType{ tmpl_name, state.state.global_namespace } };
				}
			}
		}
		{
			And<IndentToken, grammar::Type> t{ 1 };
			auto tokens = Tokenizer(replaced).read();
			tokens_and_iterator g{ tokens, tokens.begin() };
			auto ok = build(t, g.it);
			while (parse_empty_line(g.it));
			if (ok && (g.it == g.tokens.end() || g.it->first == END)) {
				auto structured_t = getStruct(t.get<grammar::Type>(), std::nullopt);
				structured_t.name = tmpl_name;
				auto templated_with_reprs = vec_of_expected_to_expected_of_vec(templated_with
					| std::views::transform([&](auto&& tn) { return transpile_typename(state, tn); })
					| to_vec());
				return_if_error(templated_with_reprs);
				state.state.global_namespace.types[structured_t.name].push_back(copy(structured_t));
				auto opt_error = traverse_type(state, structured_t);
				if (opt_error.has_value())
					return opt_error.value();
				return NodeStructs::MetaType{ std::move(structured_t) };
			}
		}
		return error{
			"user error",
			"Template expansion does not result in a type or function: |begin|\n" + replaced + "\n|end|"
		};
	}
	else
		return error{
			"user error",
			"Template not found `" + t.type + "`"
		};
}

R T::operator()(const NodeStructs::NamespacedTypename& t) {
	throw;
}

R T::operator()(const NodeStructs::TemplatedTypename& t) {
	throw;
	//const auto& templated = operator()(t.type);
	/*return NodeStructs::TypeCategory{ NodeStructs::TypeTemplateInstanceType{
		::template_type_of_typename_visitor{ {}, state }(type.type),
		type.templated_with
			| std::views::transform([&](const auto& e) { return type_of_typename(state, e); })
			| to_vec()
	} };*/
}

R T::operator()(const NodeStructs::UnionTypename& t) {
	throw;
}
