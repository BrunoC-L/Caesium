#pragma once
#include <memory>
#include <utility>
#include <variant>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>


struct A {
	B b;
	C c;
	int x;
	int y;
	A(B b) {
		a.b=c.d.e.e.e.e.e.e;
	}
	A(C c) {
		this.c=c();
	}
	A(B b) {
		a();
	}
	A(C c) {
		a.a;
	}
	A(C c) {
		a.a();
	}
	A(B b) {
		a().a;
	}
	A(B b) {
		a()();
	}
	A(B b) {
		a()=a();
	}
	A a(B b, D d) {
		a=b=c;
	}
	void a() {
		if (a) {
		}
	}
	void a() {
		if (a) {
		}
	}
};

