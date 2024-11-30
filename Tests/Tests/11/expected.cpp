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


using Vector_String_ = Vector<String>;
Int _redirect_main(const Vector_String_& s);

Int _redirect_main(const Vector_String_& s) {
	Union<A, B> v1 = Union<A, B>{A{}};
	Union<A, B, C> v2 = std::visit([](const auto& auto1) -> Union<A, B, C> { return auto1; }, v1);
};

int main(int argc, char** argv) {
	std::vector<std::string> args {};
	for (int i = 0; i < argc; ++i)
		args.push_back(std::string(argv[i]));
	return _redirect_main(args);
};
