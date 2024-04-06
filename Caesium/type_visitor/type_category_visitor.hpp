#pragma once
#include "../core/node_structs.hpp"

template <typename T>
concept TypeCategoryVisitorConcept = requires(T&& t, const NodeStructs::MetaType& v) {
	t(v);
	std::visit(t, v.type);
};

template <typename T>
struct TypeCategoryVisitor {
	template <typename Self>
	auto operator()(this Self&& self, const NodeStructs::MetaType& t) {
		return std::visit(
			[&](const auto& t) {
				return self(t);
			},
			t.type
		);
	}
#define TypeCategoryVisitorDeclarations \
	R operator()(const NodeStructs::PrimitiveType& t);\
	R operator()(const NodeStructs::Type& t);\
\
	R operator()(const NodeStructs::FunctionType& t);\
	R operator()(const NodeStructs::InterfaceType& t);\
	R operator()(const NodeStructs::NamespaceType& t);\
	R operator()(const NodeStructs::Builtin& t);\
	R operator()(const NodeStructs::UnionType& t);\
	R operator()(const NodeStructs::TemplateType& t);\
	R operator()(const NodeStructs::EnumType& t);\
	R operator()(const NodeStructs::EnumValueType& t);\
	R operator()(const NodeStructs::AggregateType& t);\
\
	R operator()(const NodeStructs::Vector& t);\
	R operator()(const NodeStructs::VectorType& t);\
	R operator()(const NodeStructs::Set& t);\
	R operator()(const NodeStructs::SetType& t);\
	R operator()(const NodeStructs::Map& t);\
	R operator()(const NodeStructs::MapType& t);
};

