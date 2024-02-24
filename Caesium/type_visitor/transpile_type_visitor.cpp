#include "../core/toCPP.hpp"

using T = transpile_type_visitor;
using R = T::R;

R T::operator()(const NodeStructs::Type& t) {
	return t.name;
}

//R T::operator()(const NodeStructs::TypeTemplateInstanceType& t) {
//	std::stringstream ss;
//	ss << t.type_template.get().templated.name << "<";
//	bool has_previous = false;
//	for (const auto& e : t.template_arguments) {
//		if (has_previous)
//			ss << ", ";
//		else
//			has_previous = true;
//		ss << operator()(e).value();
//	}
//	ss << ">";
//	return ss.str();
//}

R T::operator()(const NodeStructs::AggregateType& t) {
	throw;
}

R T::operator()(const NodeStructs::TypeType& t) {
	throw;
	//return operator()(t.type).transform([](std::string&& rep) { return "Type " + rep; });
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
