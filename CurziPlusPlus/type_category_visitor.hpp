#pragma once
#include "node_structs.hpp"

template <typename T>
concept TypeCategoryVisitorConcept = requires(T&& t, const NodeStructs::TypeCategory& v) {
	t(v);
	std::visit(t, v.value);
};

template <typename T>
struct TypeCategoryVisitor {
	template <typename Self>
	auto operator()(this Self&& self, const NodeStructs::TypeCategory& t) {
		return std::visit(
			[&](const auto& t) {
				return self(t);
			},
			t.value
		);
	}
	/*
	R operator()(const NodeStructs::Type& t);
	R operator()(const NodeStructs::TypeTemplateInstanceType& t);
	R operator()(const NodeStructs::AggregateType& t);
	R operator()(const NodeStructs::TypeType& t);
	R operator()(const NodeStructs::TypeTemplateType t);
	R operator()(const NodeStructs::FunctionType& t);
	R operator()(const NodeStructs::FunctionTemplateType& t);
	R operator()(const NodeStructs::UnionType& t);
	*/
};

