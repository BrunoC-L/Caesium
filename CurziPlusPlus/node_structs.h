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
	using ClassElement = std::variant<Constructor, Function, MemberVariable, Alias>;
	struct TemplateTypeExtension;
	struct NSTypeExtension;
	struct Typename;
	using TypenameExtension = std::variant<TemplateTypeExtension, NSTypeExtension>;
	struct templateDeclaration;
	struct Statement;

	struct Function {
		std::string name;
		Typename returnType;
		std::vector<Typename> parameterTypes;
		std::vector<Statement> statements;
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

	struct TemplateTypeExtension {
		std::vector<Typename> templateTypes;
	};

	struct NSTypeExtension {
		std::string NSTypename;
	};

	struct Typename {
		std::string type;
		std::vector<TypenameExtension> extensions;
	};

	struct templateDeclaration {
		std::string type;
		std::vector<templateDeclaration> templated;
	};

	struct Statement {
	};

	struct File {
		std::vector<Class> classes;
		std::vector<Function> functions;
	};

	struct Class {
		std::string name;
		std::optional<templateDeclaration> templated;
		std::vector<Typename> inheritances;
		std::vector<Alias> aliases;
		std::vector<Constructor> constructors;
		std::vector<Function> methods;
		std::vector<MemberVariable> memberVariables;
	};
}