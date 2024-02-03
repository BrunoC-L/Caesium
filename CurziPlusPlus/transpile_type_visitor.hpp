#pragma once
#include "toCPP.hpp"
#include "type_category_visitor.hpp"

struct transpile_type_visitor : TypeCategoryVisitor<transpile_type_visitor> {
	using TypeCategoryVisitor<transpile_type_visitor>::operator();

	transpilation_state_with_indent state;

	using R = transpile_t;

	R operator()(const NodeStructs::Type& t);
	R operator()(const NodeStructs::AggregateType& t);
	R operator()(const NodeStructs::TypeType& t);
	R operator()(const NodeStructs::FunctionType& t);
	R operator()(const NodeStructs::UnionType& t);
	R operator()(const NodeStructs::VectorType& t);
	R operator()(const NodeStructs::SetType& t);
	R operator()(const NodeStructs::MapType& t);
	R operator()(const NodeStructs::Template& t);
	R operator()(const std::string& t);
	R operator()(const double& t);
	R operator()(const int& t);
	R operator()(const bool& t);
};
