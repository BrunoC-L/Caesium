#include "toCPP.h"
#include "type_variant_visitor.hpp"

struct transpile_type_visitor : TypeCategoryVisitor<transpile_type_visitor> {
	using TypeCategoryVisitor<transpile_type_visitor>::operator();

	variables_t& variables;
	const Named& named;

	using R = transpile_t;

	R operator()(const NodeStructs::TypeCategory& t) {
		return std::visit(
			[&](const auto& t) {
				return operator()(t);
			},
			t.value
		);
	}
	R operator()(const NodeStructs::Type& t) {
		return t.name;
	}
	R operator()(const NodeStructs::TypeTemplateInstance& t) {
		std::stringstream ss;
		ss << t.type_template.get().templated.name << "<";
		bool has_previous = false;
		for (const auto& e : t.template_arguments) {
			if (has_previous)
				ss << ", ";
			else
				has_previous = true;
			ss << operator()(e).value();
		}
		ss << ">";
		return ss.str();
	}
	R operator()(const NodeStructs::TypeAggregate& t) {
		throw;
	}
	R operator()(const NodeStructs::TypeType& t) {
		throw;
		//return operator()(t.type).transform([](std::string&& rep) { return "Type " + rep; });
	}
	R operator()(const NodeStructs::TypeTemplateType t) {
		throw;
	}
	R operator()(const NodeStructs::FunctionType& t) {
		throw;
	}
	R operator()(const NodeStructs::FunctionTemplateType& t) {
		throw;
	}
	R operator()(const NodeStructs::UnionType& t) {
		throw;
	}
};
