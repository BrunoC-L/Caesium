#include "cppElements.h"

void Typename::toString(int ntab) {
	out.header << data.get("word").asString();
	//std::cout << data.get("word").asString();
	for (const auto& extension : data.get("typenameExtensions").getChildren())
		factory.create(extension, out)->accept(this);
}

void TemplateTypename::toString(int ntab) {
	out.header << "<";
	const auto& types = data.get("TypenameList").get("types");
	for (int i = 0; i < types.size(); i++) {
		if (i > 0)
			out.header << ", ";
		const auto& type = types.get(i);
		factory.create(type, out)->accept(this);
	}
	out.header << ">";
}

void Class::toString(int ntab) {
	auto className = data.get("word");
	auto inheritance = data.get("opt_inheritance");
	auto elements = data.get("classElements");
	out.header << "class " << className.asString() << " ";
	const unsigned size = inheritance.size();
	if (size) {
		out.header << ": ";
		for (unsigned i = 0; i < size; ++i) {
			const auto& child = inheritance.getChildren().at(i);
			if (i > 0)
				out.header << ", ";
			out.header << "public ";
			factory.create(child, out)->toString(ntab);
		}
	}
	out.header << "{";
	for (const auto& methodOrVariable : elements.getChildren())
		factory.create(methodOrVariable, out)->toString(ntab);
	out.header << "};";
}

void File::toString(int ntab) {
	for (const auto& class_or_function : data.get("file").get("classes_or_functions").getChildren())
		factory.create(class_or_function, out)->accept(this);
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

void NSTypename::toString(int ntab) {
	out.header << "::";
	const auto& type = data.get("type");
	factory.create(type, out)->accept(this);
}

void ClassInheritance::toString(int ntab) {
	const auto& child1 = data.get("MultipleInheritanceNode");
	const auto& child2 = child1.get("typenames");
	const std::vector<JSON>& children = child2.getChildren();
	for (const auto& T : child2.getChildren())
		factory.create(T, out)->toString(ntab);
}

void MultipleInheritance::toString(int ntab) {
	JSON typenames = data.get("typenames");
	for (const JSON& T : typenames.getChildren())
		factory.create(T, out)->accept(this);
}

void TypenameList::toString(int ntab) {
}

void Indent::toString(int ntab) {

}
