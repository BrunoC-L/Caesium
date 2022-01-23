#include "factory.h"
#include "cppElements.h"

#define TNode(T) #T + std::string("Node")
#define NodeCase(T) if (nodetype == TNode(T)) return std::make_unique<T>(out, data, *this);

std::unique_ptr<CPPelement> CPPelementFactory::create(const JSON& data, stream& out) const {
	const std::string nodetype = data.get("which").asString();
	NodeCase(Class);
	NodeCase(File);
	NodeCase(Function);
	NodeCase(Typename);
	NodeCase(TemplateTypename);
	NodeCase(NSTypename);
	NodeCase(Method);
	NodeCase(MemberVariable);
	NodeCase(ClassElement);
	NodeCase(Constructor);
	NodeCase(ClassMember);
	throw std::exception();
}
