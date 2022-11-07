#pragma once
#include <fstream>
#include <string>
#include <sstream>
#include "node_structs.h"

class toCPP {
public:
	void transpile(std::ofstream& h, std::ofstream& cpp, const NodeStructs::File& file) {
		h << "#pragma once\n";
		for (const auto& import : file.imports) {
			h << "#include " << import.imported << "\n";
		}
		h << "\n";

		for (const auto& Class : file.classes) {
			
			if (Class.templated.has_value())
				transpileTypeTemplateDeclaration(Class.templated.value(), h);

			h << "class " << Class.name;

			bool firstInheritance = true;
			for (const auto& inheritance : Class.inheritances) {
				if (firstInheritance) {
					firstInheritance = false;
					h << " : ";
				}
				else
					h << ", ";
				h << "public ";
				transpileType(inheritance, h);
			}
			h << " {\n";
			for (const auto& member : Class.memberVariables) {
				h << "\t";
				transpileType(member.type, h);
				h << " " << member.name << ";\n";
			}
			for (const auto& method : Class.methods) {
				h << "\t";
				transpileType(method.returnType, h);
				h << " " << method.name << "(";
				auto first = true;
				for (const auto& t : method.parameterTypes) {
					if (first)
						first = false;
					else
						h << ", ";
					transpileType(t, h);
				}
				h << ");\n";
			}
			h << "};\n\n";
		}
	}

	template <typename stream>
	void transpileType(const NodeStructs::Typename& type, stream& ss) {
		auto f = overload(
			[&](const NodeStructs::NSTypeExtension& ext) {
				ss << "::" << ext.NSTypename;
			},
			[&](const NodeStructs::TemplateTypeExtension& ext) {
				ss << "<";
				bool isFirst = true;
				for (const auto& T : ext.templateTypes) {
					if (!isFirst)
						ss << ", ";
					isFirst = false;
					transpileType(T, ss);
				}
				ss << ">";
			}
			);

		ss << type.type;
		for (const auto& ext : type.extensions)
			std::visit(f, ext);
	}

/*	<U>   : template <typename U>
	<U, V>: template <typename U, typename V>
	<U<V>>: template <template <typename V> typename U>*/
	template <typename stream>
	void transpileTypeTemplateDeclaration(const NodeStructs::TemplateDeclaration& tmpl, stream& ss, bool printNameAtEnd = false) {
		if (tmpl.parameters.size())
			ss << "template <";
		bool first = true;
		for (const auto& tmpl2 : tmpl.parameters) {
			if (!first)
				ss << ", ";
			first = false;
			transpileTypeTemplateDeclaration(tmpl2, ss, true);
		}
		if (tmpl.parameters.size())
			ss << "> ";
		if (printNameAtEnd)
			ss << "typename " << tmpl.name;
		else
			ss << "\n";
	}
};
