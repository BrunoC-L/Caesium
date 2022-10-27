#pragma once
#include "node_structs.h"
#include "grammar.h"

NodeStructs::Import getStruct(const Import& f) {
	NodeStructs::Import res;
	res.content.push_back(f.value.get<Word, 0>().value);
	res.location = f.value.get<Word, 1>().value;
	return res;
}

NodeStructs::File getStruct(const File& f) {
	NodeStructs::File res;
	for (const Import& import : f.value.get<Star<Import>>().get<Import>())
		res.imports.push_back(getStruct(import));
	return res;
}

NodeStructs::Class getStruct(const Class& f) {
	NodeStructs::Class res;
	return res;
}

NodeStructs::Constructor getStruct(const Constructor& f) {
	NodeStructs::Constructor res;
	return res;
}

NodeStructs::Function getStruct(const Function& f) {
	NodeStructs::Function res;
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
