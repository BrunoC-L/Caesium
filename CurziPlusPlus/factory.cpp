#include "factory.h"
#include "cppElements.h"

#define TNode(T) #T + std::string("Node")
#define NodeCase(T) if (nodetype == TNode(T) || nodetype == #T) return std::make_unique<T>(out, data.get(nodetype), *this);

std::unique_ptr<CPPelement> CPPelementFactory::create(const JSON& data, stream& out) const {
	const std::string nodetype = data.getProperties().at(0);
	//std::cout << nodetype << "\n";
	NodeCase(Class)
	NodeCase(File)
	NodeCase(Function)
	NodeCase(Typename)
	NodeCase(TemplateTypename)
	NodeCase(NSTypename)
	NodeCase(Method)
	NodeCase(MemberVariable)
	NodeCase(ClassElement)
	NodeCase(Constructor)
	NodeCase(ClassMember)
	NodeCase(ClassInheritance)
	NodeCase(MultipleInheritance)
	NodeCase(TypenameList)
	NodeCase(Indent)
	throw std::exception();
}
