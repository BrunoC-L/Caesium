#pragma once
#include "cppElementVisitor.h"
#include "factory.h"

class CPPelement : public CPPelementVisitor {
protected:
	const JSON& data;
	const CPPelementFactory& factory;
	stream& out;
public:
	CPPelement(stream& out, const JSON& data, const CPPelementFactory& factory) : out(out), data(data), factory(factory) {}
	virtual void accept(CPPelementVisitor* v) = 0;
	virtual void toString(int ntab) = 0;
};

#define ctor(T) \
T(stream& out, const JSON& data, const CPPelementFactory& f) : CPPelement(out, data, f) {}\
virtual void accept(CPPelementVisitor* v) override {v->visit(this);}

#define CPPelementClass(T) \
class T : public CPPelement {\
public:\
	ctor(T);\
	virtual void default_behavior(CPPelement* e) override { e->toString(0); };
