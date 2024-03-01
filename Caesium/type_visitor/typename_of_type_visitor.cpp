#include "../core/toCPP.hpp"

using T = typename_of_type_visitor;
using R = T::R;

R T::operator()(const NodeStructs::Type& t) {
	return R::value_type{ NodeStructs::BaseTypename{ t.name } };
}

//R T::operator()(const NodeStructs::AggregateType& t) {
//	throw;
//}

R T::operator()(const NodeStructs::TypeType& t) {
	throw;
}

R T::operator()(const NodeStructs::FunctionType& t) {
	throw;
}

R T::operator()(const NodeStructs::InterfaceType& t) {
	throw;
}

R T::operator()(const NodeStructs::UnionType& t) {
	throw;
}

R T::operator()(const NodeStructs::VectorType& t) {
	throw;
}

R T::operator()(const NodeStructs::SetType& t) {
	throw;
}

R T::operator()(const NodeStructs::MapType& t) {
	throw;
}

R T::operator()(const NodeStructs::Template& t) {
	throw;
}

R T::operator()(const NodeStructs::BuiltInType& t) {
	throw;
}

R T::operator()(const std::string&) {
	throw;
}

R T::operator()(const double&) {
	throw;
}

R T::operator()(const int&) {
	throw;
}

R T::operator()(const bool&) {
	throw;
}
