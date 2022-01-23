#include "cppElements.h"

void Typename::toString(int ntab) {
	const auto& types = data.get("types");
	out.header << types.get(0).asString();
	for (int i = 1; i < types.size(); ++i)
		factory.create(types.get(i), out)->accept(this);
}

void TemplateTypename::toString(int ntab) {
	out.header << "<";
	const auto& types = data.get("types");
	for (int i = 0; i < types.size(); i++) {
		if (i > 0)
			out.header << ", ";
		const auto& type = types.get(i);
		factory.create(type, out)->accept(this);
	}
	out.header << ">";
}

void Class::toString(int ntab) {
	auto className = data.get("className");
	auto inheritance = data.get("inheritance");
	auto elements = data.get("classElements");
	out.header << "class " << className.asString() << " ";
	const unsigned size = inheritance.size();
	if (size) {
		out.header << ": ";
		for (unsigned i = 0; i < size; ++i) {
			if (i > 0)
				out.header << ", ";
			out.header << "public ";
			factory.create(inheritance.getChildren().at(i), out)->accept(this);
		}
	}
	out.header << "{";
	for (const auto& methodOrVariable : elements.getChildren())
		factory.create(methodOrVariable, out)->accept(this);
	out.header << "};";
}

void File::toString(int ntab) {
	for (const auto& class_or_function : data.get("classes_and_functions").getChildren())
		factory.create(class_or_function, out)->accept(this);
}

void File::visit(Class* e) {
	e->toString(0);
}

void File::visit(Function* e) {
	e->toString(0);
}

void Typename::visit(TemplateTypename* e) {
	e->toString(0);
}

void Typename::visit(NSTypename* e) {
	e->toString(0);
}

void Class::visit(Typename* e) {
	e->toString(0);
}

void Class::visit(ClassMember* e) {
	e->toString(1);
}

void Class::visit(Constructor* e) {
	e->toString(1);
}

void TemplateTypename::visit(Typename* e) {
	e->toString(0);
}

void Method::toString(int ntab) {
	out.header << "\n\t";
	const auto& qualifiers = data.get("qualifiers");
	auto ppp = qualifiers.get("opt_ppp").asString();
	auto staticity = qualifiers.get("opt_static").asString();
	if (ppp != "")
		out.header << ppp << ":";
	if (staticity != "")
		out.header << staticity << " ";
	factory.create(data.get("type"), out)->accept(this);
	out.header << " ";
	out.header << data.get("name").asString();
	out.header << "(";
	out.header << ");";
}

void Method::visit(Typename* e) {
	e->toString(0);
}

void MemberVariable::toString(int ntab) {
	out.header << "member variable";
}

void Function::toString(int ntab) {
	out.header << "function";
}

void ClassElement::toString(int ntab) {
	return;
}

void Constructor::toString(int ntab) {
	out.header << "constructor";
}

void ClassMember::toString(int ntab) {
	factory.create(data.get("member"), out)->accept(this);
}

void ClassMember::visit(Method* e) {
	e->toString(1);
}

void ClassMember::visit(MemberVariable* e) {
	e->toString(1);
}

void NSTypename::toString(int ntab) {
	out.header << "::";
	const auto& type = data.get("type");
	factory.create(type, out)->accept(this);
}

void NSTypename::visit(Typename* e) {
	e->toString(0);
}
