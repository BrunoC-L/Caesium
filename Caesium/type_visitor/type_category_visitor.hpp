#pragma once
#include "../structured/node_structs.hpp"

template <typename T>
struct TypeCategoryVisitor {
	template <typename Self>
	auto operator()(this Self&& self, const Realised::MetaType& t) {
		return std::visit(
			[&](const auto& t) {
				return self(t);
			},
			t.type.get()._value
		);
	}
#define TypeCategoryVisitorDeclarations \
	R operator()(const Realised::PrimitiveType& t);\
	R operator()(const Realised::Type& t);\
\
	R operator()(const Realised::FunctionType& t);\
	R operator()(const Realised::Interface& t);\
	R operator()(const Realised::NamespaceType& t);\
	R operator()(const Realised::Builtin& t);\
	R operator()(const Realised::UnionType& t);\
	R operator()(const Realised::TemplateType& t);\
	R operator()(const Realised::EnumType& t);\
	R operator()(const Realised::EnumValueType& t);\
	R operator()(const Realised::AggregateType& t);\
	R operator()(const Realised::OptionalType& t);\
\
	R operator()(const Realised::VectorType& t);\
	R operator()(const Realised::SetType& t);\
	R operator()(const Realised::MapType& t);\
	R operator()(const Realised::TypeListType& t);\
\
	R operator()(const Realised::CompileTimeType& t);\
	R operator()(const Realised::TemplateInstanceType& t);
};

