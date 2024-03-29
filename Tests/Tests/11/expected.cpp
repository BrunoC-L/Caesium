#include "defaults.hpp"

struct A;
struct B;
struct C;

struct A {
};

struct B {
};

struct C {
};


Int _redirect_main(const Vector<String>& s);

Int _redirect_main(const Vector<String>& s) {
	Variant<A, B> v1 = Variant<A, B>{A{}};
	Variant<A, B, C> v2 = std::visit([](const auto& auto1) -> Variant<A, B, C> { return auto1; }, v1);
};

int main(int argc, char** argv) {
	std::vector<std::string> args {};
	for (int i = 0; i < argc; ++i)
		args.push_back(std::string(argv[i]));
	return _redirect_main(args);
};
