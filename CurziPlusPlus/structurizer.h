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
		}), *f.value.get<Or<Word, String>>().value.get());
	return res;
}

NodeStructs::TemplateDeclaration getTemplateDeclaration(const TemplateTypenameDeclaration& templateTypename) {
	NodeStructs::TemplateDeclaration res;
	res.name = templateTypename.value.get<Word>().value;
	for (const auto* t : templateTypename.value.get<CommaPlus<Or<TemplateTypenameDeclaration, Word>>>().get<Or<TemplateTypenameDeclaration, Word>>())
		std::visit([&res](const auto& t) {
		res.parameters.emplace_back(getTemplateDeclaration(t));
			}, *t->value.get());
	return res;
}

NodeStructs::TemplateDeclaration getTemplateDeclaration(const Word& word) {
	NodeStructs::TemplateDeclaration res;
	res.name = word.value;
	return res;
}

NodeStructs::Class getStruct(const Class& cl) {
	return std::visit(overload(
		[](const TemplateTypenameDeclaration& templateTypename) {
			NodeStructs::Class res;
			res.name = templateTypename.value.get<Word>().value;
			std::vector<NodeStructs::TemplateDeclaration> templates;
			for (const auto* t : templateTypename.value.get<CommaPlus<Or<TemplateTypenameDeclaration, Word>>>().get<Or<TemplateTypenameDeclaration, Word>>())
				std::visit([&templates](const auto& t) {
					templates.emplace_back(getTemplateDeclaration(t));
				}, *t->value.get());
			res.templated.emplace(NodeStructs::TemplateDeclaration{ "", std::move(templates) });
			return res;
		},
		[](const Word& word) {
			NodeStructs::Class res;
			res.name = word.value;
			return res;
		}
	), *cl.value.get<Or<TemplateTypenameDeclaration, Word>>().value.get());
}

NodeStructs::Function getStruct(const Function& f) {
	NodeStructs::Function res;
	return res;
}

NodeStructs::File getStruct(const File& f) {
	NodeStructs::File res;
	for (const Import* import : f.value.get<Star<Import>>().get<Import>())
		res.imports.emplace_back(getStruct(*import));
	for (const Class* cl : f.value.get<Star<Or<Class, Function>>>().get<Class>())
		res.classes.emplace_back(getStruct(*cl));
	for (const Function* fun : f.value.get<Star<Or<Class, Function>>>().get<Function>())
		res.functions.emplace_back(getStruct(*fun));
	return res;
}

NodeStructs::Constructor getStruct(const Constructor& f) {
	NodeStructs::Constructor res;
	return res;
}

NodeStructs::MemberVariable getStruct(const MemberVariable& f) {
	NodeStructs::MemberVariable res;
	return res;
}

NodeStructs::Alias getStruct(const Alias& f) {
	NodeStructs::Alias res;
	return res;
}

NodeStructs::TemplateTypeExtension getStruct(const TemplateTypename& f) {
	NodeStructs::TemplateTypeExtension res;
	return res;
}

NodeStructs::NSTypeExtension getStruct(const NSTypename& f) {
	NodeStructs::NSTypeExtension res;
	return res;
}

NodeStructs::Typename getStruct(const Typename& f) {
	NodeStructs::Typename res;
	return res;
}

NodeStructs::Statement getStruct(const Statement& f) {
	NodeStructs::Statement res;
	return res;
}
