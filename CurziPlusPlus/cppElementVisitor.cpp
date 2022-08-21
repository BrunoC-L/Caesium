#include "cppElementVisitor.h"
#include "cppElements.h"

void CPPelementVisitor::visit(Class* e) { default_behavior(e); }
void CPPelementVisitor::visit(File* e) { default_behavior(e); }
void CPPelementVisitor::visit(Function* e) { default_behavior(e); }
void CPPelementVisitor::visit(Typename* e) { default_behavior(e); }
void CPPelementVisitor::visit(TemplateTypename* e) { default_behavior(e); }
void CPPelementVisitor::visit(NSTypename* e) { default_behavior(e); }
void CPPelementVisitor::visit(ClassElement* e) { default_behavior(e); }
void CPPelementVisitor::visit(Method* e) { default_behavior(e); }
void CPPelementVisitor::visit(MemberVariable* e) { default_behavior(e); }
void CPPelementVisitor::visit(Constructor* e) { default_behavior(e); }
void CPPelementVisitor::visit(ClassMember* e) { default_behavior(e); }
void CPPelementVisitor::visit(ClassInheritance* e) { default_behavior(e); }
void CPPelementVisitor::visit(MultipleInheritance* e) { default_behavior(e); }
void CPPelementVisitor::visit(TypenameList* e) { default_behavior(e); }
void CPPelementVisitor::visit(Indent* e) { default_behavior(e); }
