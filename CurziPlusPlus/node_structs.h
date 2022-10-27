#pragma once
#include <vector>
#include <string>
#include <variant>
#include <optional>

#define NODE_CAST(T, E) std::dynamic_pointer_cast<T>(E)

template<class... Ts> struct overload : Ts... { using Ts::operator()...; };
template<class... Ts> overload(Ts...)->overload<Ts...>;

namespace NodeStructs {
	struct File;
	struct Class;
	struct Constructor;
	struct Function;
	struct MemberVariable;
	struct Alias;
	struct TemplateTypeExtension;
	struct NSTypeExtension;
	struct Typename;
	using TypenameExtension = std::variant<TemplateTypeExtension, NSTypeExtension>;
	struct Statement;

	struct TemplateTypeExtension {
		std::vector<Typename> templateTypes;
	};

	struct Typename {
		std::string type;
		std::vector<TypenameExtension> extensions;
	};

	struct Alias {
		Typename aliasFrom;
		Typename aliasTo;
	};

	struct Constructor {
		std::vector<Typename> parameterTypes;
		std::vector<Statement> statements;
	};

	struct MemberVariable {
		std::string name;
		Typename type;
	};

	struct NSTypeExtension {
		std::string NSTypename;
	};

	struct Statement {
	};

	struct Import {
		std::string location;
		std::vector<std::string> content;
	};

	struct Function {
		std::string name;
		NodeStructs::Typename returnType;
		std::vector<Typename> parameterTypes;
		std::vector<Statement> statements;
	};

	struct File {
		std::vector<Import> imports;
		std::vector<Class> classes;
		std::vector<Function> functions;
	};

	struct Class {
		std::string name;
		//std::optional<templateDeclaration> templated;
		std::vector<Typename> inheritances;
		std::vector<Alias> aliases;
		std::vector<Constructor> constructors;
		std::vector<Function> methods;
		std::vector<MemberVariable> memberVariables;
	};
}
