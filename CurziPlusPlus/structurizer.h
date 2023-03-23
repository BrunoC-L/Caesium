#pragma once
#include "node_structs.h"
#include "grammar.h"

NodeStructs::Import getStruct(const Import& f) {
	NodeStructs::Import res;
	res.imported = std::visit(
		overload(
			[](const Word& word) {
				return "\"" + word.value + ".h\"";
			},
			[](const String& string) {
				return string.value;
		}), f.value.get<Or<Word, String>>().value.value());
	return res;
}

std::vector<NodeStructs::TemplateDeclaration> getTemplatesFromTemplateTypenameDeclaration(const TemplateTypenameDeclaration& templateTypename);

NodeStructs::TemplateDeclaration getTemplateDeclaration(const TemplateTypenameDeclaration& templateTypename) {
	NodeStructs::TemplateDeclaration res;
	res.name = templateTypename.value.get<Word>().value;
	res.parameters = getTemplatesFromTemplateTypenameDeclaration(templateTypename);
	return res;
}

NodeStructs::TemplateDeclaration getTemplateDeclaration(const Word& word) {
	NodeStructs::TemplateDeclaration res;
	res.name = word.value;
	return res;
}

std::vector<NodeStructs::TemplateDeclaration> getTemplatesFromTemplateTypenameDeclaration(const TemplateTypenameDeclaration& templateTypename) {
	std::vector<NodeStructs::TemplateDeclaration> res;
	for (const auto& t : templateTypename.value.get<CommaPlus<Or<TemplateTypenameDeclaration, Word>>>().get<Or<TemplateTypenameDeclaration, Word>>())
		std::visit([&res](const auto& t) {
			res.push_back(getTemplateDeclaration(t));
		}, t.value.value());
	return res;
}

void extend(std::vector<NodeStructs::TypenameExtension>& vec, const NSTypename& nst) {
	const Typename& t = nst.value.get<Typename>();
	vec.push_back(NodeStructs::NSTypeExtension{ t.value.get<Word>().value });
	if (t.value.get<Opt<Or<NSTypename, TemplateTypename>>>().node.has_value())
		std::visit([&vec](const auto& t) {
			extend(vec, t);
		}, t.value.get<Opt<Or<NSTypename, TemplateTypename>>>().node.value().value.value());
}

NodeStructs::Typename getStruct(const Typename& t);

void extend(std::vector<NodeStructs::TypenameExtension>& vec, const TemplateTypename& tt) {
	NodeStructs::TemplateTypeExtension res;
	for (const Typename& t : tt.value.get<CommaStar<Typename>>().get<Typename>())
		res.templateTypes.push_back(getStruct(t));
	vec.push_back(std::move(res));
	if (tt.value.get<Opt<NSTypename>>().node.has_value())
		extend(vec, tt.value.get<Opt<NSTypename>>().node.value());
	
}

/*
expr    -> Typename                                              -> NodeStructs::Typename
T::T<T> -> typename{T, ns{typename{T, template{[typename{T}]}}}} -> typename{T, [ns{T}, template{T}]}
T<T>::T -> typename{T, template{[typename{T}], ns{typename{T}}}} -> typename{T, [template{T}, ns{T}]}
T<T::T> -> typename{T, template{[typename{T, ns{typename{T}}}]}} -> typename{T, [template{T, [ns{T}]}]}
*/
NodeStructs::Typename getStruct(const Typename& t) {
	NodeStructs::Typename res;
	res.type = t.value.get<Word>().value;
	if (t.value.get<Opt<Or<NSTypename, TemplateTypename>>>().node.has_value())
		std::visit([&res](const auto& t) {
			extend(res.extensions, t);
		}, t.value.get<Opt<Or<NSTypename, TemplateTypename>>>().node.value().value.value());
	return res;
}

NodeStructs::Function getStruct(const Function& f) {
	NodeStructs::Function res;
	res.name = f.value.get<Word>().value;
	res.returnType = getStruct(f.value.get<Typename>());
	return res;
}

NodeStructs::Constructor getStruct(const Constructor& f) {
	NodeStructs::Constructor res;
	return res;
}

NodeStructs::MemberVariable getStruct(const MemberVariable& f) {
	NodeStructs::MemberVariable res;
	res.type = getStruct(f.value.get<Typename>());
	res.name = f.value.get<Word>().value;
	return res;
}

NodeStructs::Alias getStruct(const Alias& f) {
	NodeStructs::Alias res;
	res.aliasFrom = NodeStructs::Typename{ f.value.get<Word>().value, {} };
	res.aliasTo = getStruct(f.value.get<Typename>());
	return res;
}

NodeStructs::Class getStruct(const Class& cl) {
	NodeStructs::Class computedClass = std::visit(overload(
		[](const TemplateTypenameDeclaration& templateTypename) {
			NodeStructs::Class res;
			res.name = templateTypename.value.get<Word>().value;
			std::vector<NodeStructs::TemplateDeclaration> templates = getTemplatesFromTemplateTypenameDeclaration(templateTypename);
			res.templated.emplace(NodeStructs::TemplateDeclaration{ "", std::move(templates) });
			return res;
		},
		[](const Word& word) {
			NodeStructs::Class res;
			res.name = word.value;
			return res;
		}
	), cl.value.get<Or<TemplateTypenameDeclaration, Word>>().value.value());
	for (const ClassElement& ce : cl.value.get<Indent<Star<And<IndentToken, ClassElement>>>>().get<ClassElement>())
		std::visit([&computedClass](const auto& e) {
			computedClass.get<decltype(getStruct(e))>().push_back(getStruct(e));
		}, ce.value.value.value());
	if (cl.value.get<Opt<ClassInheritance>>().node.has_value())
		for (const Typename& t : cl.value
									.get<Opt<ClassInheritance>>().node.value().value
									.get<CommaPlus<Typename>>()
									.get<Typename>())
			computedClass.inheritances.push_back(getStruct(t));
	return computedClass;
}

NodeStructs::File getStruct(const File& f) {
	NodeStructs::File res;
	for (const Import& import : f.value.get<Star<Import>>().get<Import>())
		res.imports.push_back(getStruct(import));
	for (const Class& cl : f.value.get<Star<Or<Class, Function>>>().get<Class>())
		res.classes.push_back(getStruct(cl));
	for (const Function& fun : f.value.get<Star<Or<Class, Function>>>().get<Function>())
		res.functions.push_back(getStruct(fun));
	return res;
}

NodeStructs::Statement getStruct(const Statement& f) {
	NodeStructs::Statement res;
	return res;
}
