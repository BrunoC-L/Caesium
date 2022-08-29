#pragma once
#include <fstream>
#include <string>
#include <sstream>

#include "node_structs.h"

class toCPP {
public:
	void transpile(std::ofstream& h, std::ofstream& cpp, const std::unique_ptr<NodeStructs::File>& file) {
		for (const auto& Class : file->classes) {
			h << "class " << Class->name;

			bool firstInheritance = true;
			for (const auto& inheritance : Class->inheritances) {
				if (firstInheritance) {
					firstInheritance = false;
					h << " : ";
				}
				else
					h << ", ";
				h << "public " << transpileType(inheritance);
			}
			h << " {\n";
			for (const auto& member : Class->memberVariables)
				h << transpileType(member->type) << " " << member->name << ";\n";
			for (const auto& method : Class->methods) {
				h << transpileType(method->returnType) << " " << method->name << "(";
				auto first = true;
				for (const auto& t : method->parameterTypes) {
					if (first)
						first = false;
					else
						h << ", ";
					h << transpileType(t);
				}
				h << ");\n";
			}
			h << "};\n";
		}
	}

	std::string transpileType(const std::unique_ptr<NodeStructs::Typename>& type) {
		std::stringstream ss;
		ss << type->type;
		for (const auto& ext : type->extensions)
			if (std::holds_alternative<NodeStructs::NSTypeExtension>(ext))
				ss << "::" << std::get<NodeStructs::NSTypeExtension>(ext).NSTypename;
			else {
				ss << "<";
				bool isFirst = true;
				for (const auto& T : std::get<NodeStructs::TemplateTypeExtension>(ext).templateTypes) {
					if (!isFirst)
						ss << ", ";
					isFirst = false;
					ss << transpileType(T);
				}
				ss << ">";
			}
		return ss.str();
	}
};
