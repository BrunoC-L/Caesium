#pragma once
#include <exception>

class Class;
class File;
class Function;
class Typename;
class TemplateTypename;
class ClassElement;
class ClassMember;
class Method;
class MemberVariable;
class Constructor;
class NSTypename;

class CPPelementVisitor {
public:
	virtual void visit(Class* e) { throw std::exception("unimplemented"); }
	virtual void visit(File* e) { throw std::exception("unimplemented"); }
	virtual void visit(Function* e) { throw std::exception("unimplemented"); }
	virtual void visit(Typename* e) { throw std::exception("unimplemented"); }
	virtual void visit(TemplateTypename* e) { throw std::exception("unimplemented"); }
	virtual void visit(NSTypename* e) { throw std::exception("unimplemented"); }
	virtual void visit(ClassElement* e) { throw std::exception("unimplemented"); }
	virtual void visit(Method* e) { throw std::exception("unimplemented"); }
	virtual void visit(MemberVariable* e) { throw std::exception("unimplemented"); }
	virtual void visit(Constructor* e) { throw std::exception("unimplemented"); }
	virtual void visit(ClassMember* e) { throw std::exception("unimplemented"); }
};
