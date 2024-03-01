#pragma once
#include "../core/node_structs.hpp"

template <typename T>
concept TypeCategoryVisitorConcept = requires(T&& t, const NodeStructs::UniversalType& v) {
	t(v);
	std::visit(t, v.value);
};

template <typename T>
struct TypeCategoryVisitor {
	template <typename Self>
	auto operator()(this Self&& self, const NodeStructs::UniversalType& t) {
		return std::visit(
			[&](const auto& t) {
				return self(t);
			},
			t.value
		);
	}
#define TypeCategoryVisitorDeclarations \
	R operator()(const NodeStructs::Type& t);\
	R operator()(const NodeStructs::TypeType& t);\
	R operator()(const NodeStructs::FunctionType& t);\
	R operator()(const NodeStructs::UnionType& t);\
	R operator()(const NodeStructs::InterfaceType& t);\
	R operator()(const NodeStructs::VectorType& t);\
	R operator()(const NodeStructs::SetType& t);\
	R operator()(const NodeStructs::MapType& t);\
	R operator()(const NodeStructs::Template& t);\
	R operator()(const NodeStructs::BuiltInType& t);\
	R operator()(const std::string& t);\
	R operator()(const double& t);\
	R operator()(const int& t);\
	R operator()(const bool& t);
};

