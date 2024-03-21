#include "../core/toCPP.hpp"
#include "../core/structurizer.hpp"
#include "../utility/replace_all.hpp"
#include "../utility/vec_of_expected_to_expected_of_vec.hpp"

using T = type_template_of_typename_visitor;
using R = T::R;

R T::operator()(const NodeStructs::BaseTypename& t) {
	if (auto it = state.state.named.templates.find(t.type); it != state.state.named.templates.end()) {
		if (it->second.size() != 1)
			return error{
			"internal error",
			"template specialization not implemented yet"
		};
		const auto& tmpl = *it->second.back();
		if (tmpl.parameters.size() != templated_with.size()) {
			std::stringstream ss;
			ss  << "invalid number of arguments to template `"
				<< t.type
				<< "`, expected `"
				<< tmpl.parameters.size()
				<< "`, received `"
				<< templated_with.size()
				<< "`";
			return error{ "user error", ss.str() };
		}

		if (tmpl.templated == "BUILTIN") {
			if (tmpl.name == "Vector") {
				auto x = type_of_typename(state, templated_with.at(0));
				return_if_error(x);
				return NodeStructs::MetaType{ NodeStructs::VectorType{ Box<NodeStructs::MetaType>{ std::move(x).value() } } };
			}
			if (tmpl.name == "Set") {
				auto x = type_of_typename(state, templated_with.at(0));
				return_if_error(x);
				return NodeStructs::MetaType{ NodeStructs::SetType{ Box<NodeStructs::MetaType>{ std::move(x).value() } } };
			}
			if (tmpl.name == "Map") {
				auto k = type_of_typename(state, templated_with.at(0));
				return_if_error(k);
				auto v = type_of_typename(state, templated_with.at(1));
				return_if_error(v);
				return NodeStructs::MetaType{ NodeStructs::MapType{
					Box<NodeStructs::MetaType>{ std::move(k).value() },
					Box<NodeStructs::MetaType>{ std::move(v).value() }
				} };
			}
			throw;
		}

		// todo check if already traversed

		std::string replaced = tmpl.templated;
		for (int i = 0; i < tmpl.parameters.size(); ++i) {
			auto e = transpile_typename(state, templated_with.at(i));
			return_if_error(e);
			replaced = replace_all(std::move(replaced), tmpl.parameters.at(i).first, e.value());
		}
		{
			And<IndentToken, grammar::Function, Token<END>> f{ 1 };
			auto tokens = Tokenizer(replaced).read();
			tokens_and_iterator g{ tokens, tokens.begin() };
			if (build(f,g.it)) {
				auto* structured_f = new NodeStructs::Function{ getStruct(f.get<grammar::Function>(), std::nullopt) };
				structured_f->name = tmpl.name; // todo
				state.state.named.functions[structured_f->name].push_back(structured_f);
				state.state.traversed_functions.insert(*structured_f);
				auto transpiled_or_e = transpile(state, *structured_f);
				return_if_error(transpiled_or_e);
				state.state.transpile_in_reverse_order.push_back(std::move(transpiled_or_e).value());
				return NodeStructs::MetaType{ NodeStructs::FunctionType{ *structured_f } };
			}
		}
		{
			And<IndentToken, grammar::Type> t{ 1 };
			auto tokens = Tokenizer(replaced).read();
			tokens_and_iterator g{ tokens, tokens.begin() };
			auto ok = build(t, g.it);
			while (parse_empty_line(g.it));
			if (ok && (g.it == g.tokens.end() || g.it->first == END)) {
				auto* structured_t = new NodeStructs::Type{ getStruct(t.get<grammar::Type>(), std::nullopt) };
				auto templated_with_reprs= vec_of_expected_to_expected_of_vec(templated_with | LIFT_TRANSFORM_X(tn, transpile_typename(state, tn)) | to_vec());
				return_if_error(templated_with_reprs);
				structured_t->name = [&]() {
					std::stringstream ss;
					bool first = true;
					ss << structured_t->name << "___";
					for (const auto& tn : templated_with_reprs.value()) {
						if (!first) {
							ss << "__";
							first = false;
						}
						ss << tn;
					}
					ss << "___";
					return ss.str();
				}();
				state.state.named.types[structured_t->name].push_back(structured_t);
				auto opt_error = traverse_type(state, *structured_t);
				if (opt_error.has_value())
					return opt_error.value();
				return NodeStructs::MetaType{ *structured_t };
			}
		}
		{
			And<IndentToken, grammar::Type> t{ 1 };
			auto tokens = Tokenizer(replaced).read();
			tokens_and_iterator g{ tokens, tokens.begin() };
			if (build(t, g.it))
				throw;
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
	const auto& templated = operator()(t.type);
	throw;
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
