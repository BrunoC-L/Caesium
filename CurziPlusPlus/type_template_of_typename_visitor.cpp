#include "type_template_of_typename_visitor.hpp"
#include "type_of_typename_visitor.hpp"
#include "transpile_typename_visitor.hpp"
#include "structurizer.hpp"

using T = type_template_of_typename_visitor;
using R = T::R;

// https://stackoverflow.com/a/24315631/10945691
static std::string ReplaceAll(std::string str, const std::string& from, const std::string& to) {
	size_t start_pos = 0;
	while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
		str.replace(start_pos, from.length(), to);
		start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
	}
	return str;
}

R T::operator()(const NodeStructs::BaseTypename& t) {
	if (auto it = state.state.named.templates.find(t.type); it != state.state.named.templates.end()) {
		if (it->second.size() != 1)
			throw; // todo find the best match
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
				auto x = type_of_typename_visitor{ {}, state }(templated_with.at(0));
				return_if_error(x);
				return NodeStructs::UniversalType{ NodeStructs::VectorType{ Box<NodeStructs::UniversalType>{ std::move(x).value() } } };
			}
			if (tmpl.name == "Set") {
				auto x = type_of_typename_visitor{ {}, state }(templated_with.at(0));
				return_if_error(x);
				return NodeStructs::UniversalType{ NodeStructs::SetType{ Box<NodeStructs::UniversalType>{ std::move(x).value() } } };
			}
			if (tmpl.name == "Map") {
				auto x = type_of_typename_visitor{ {}, state }(templated_with.at(0));
				auto y = type_of_typename_visitor{ {}, state }(templated_with.at(1));
				return_if_error(x);
				return_if_error(y);
				return NodeStructs::UniversalType{ NodeStructs::MapType{
					Box<NodeStructs::UniversalType>{ std::move(x).value() },
					Box<NodeStructs::UniversalType>{ std::move(y).value() }
				} };
			}
			throw;
		}

		// todo check if already traversed

		std::string replaced = tmpl.templated;
		for (int i = 0; i < tmpl.parameters.size(); ++i) {
			auto e = transpile_typename_visitor{ {}, state }(templated_with.at(i));
			return_if_error(e);
			replaced = ReplaceAll(std::move(replaced), tmpl.parameters.at(i).first, e.value());
		}

		Type t{ 1 };
		auto tokens = Tokenizer(replaced).read();
		tokens_and_iterator g{ tokens, tokens.begin() };
		bool ok = t.build(g) && g.it == g.tokens.end();
		if (!ok)
			throw;
	}
	auto err = "Template not found `" + t.type + "`";
	throw std::runtime_error(err);
}

R T::operator()(const NodeStructs::NamespacedTypename& t) {
	throw;
}

R T::operator()(const NodeStructs::TemplatedTypename& t) {
	const auto& templated = operator()(t.type);
	throw;
	/*return NodeStructs::TypeCategory{ NodeStructs::TypeTemplateInstanceType{
		::template_type_of_typename_visitor{ {}, state }(type.type.get()),
		type.templated_with
			| std::views::transform([&](const auto& e) { return type_of_typename_visitor{ {}, state }(e); })
			| to_vec()
	} };*/
}

R T::operator()(const NodeStructs::UnionTypename& t) {
	throw;
}
