#pragma once
#include <exception>

class CPPelement;

class Class;
class File;
class Function;
class Typename;
class TemplateTypename;
class NSTypename;
class ClassElement;
class Method;
class MemberVariable;
class Constructor;
class ClassMember;
class ClassInheritance;
class MultipleInheritance;
class TypenameList;
class Indent;

class CPPelementVisitor {
public:
	virtual void default_behavior(CPPelement* node) { throw std::exception("unimplemented"); }
	virtual void visit(Class* e);
	virtual void visit(File* e);
	virtual void visit(Function* e);
	virtual void visit(Typename* e);
	virtual void visit(TemplateTypename* e);
	virtual void visit(NSTypename* e);
	virtual void visit(ClassElement* e);
	virtual void visit(Method* e);
	virtual void visit(MemberVariable* e);
	virtual void visit(Constructor* e);
	virtual void visit(ClassMember* e);
	virtual void visit(ClassInheritance* e);
	virtual void visit(MultipleInheritance* e);
	virtual void visit(TypenameList* e);
	virtual void visit(Indent* e);
};
