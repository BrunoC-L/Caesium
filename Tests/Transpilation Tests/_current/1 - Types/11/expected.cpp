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


using Union_A_or_B_ = Union<A, B>;
using Union_A_or_B_or_C_ = Union<A, B, C>;
using Union_filesystem__directory_or_filesystem__file_ = Union<filesystem__directory, filesystem__file>;
using Vector_String_ = Vector<String>;
Int _redirect_main(const Vector_String_& s);

Int _redirect_main(const Vector_String_& s) {
	Union_A_or_B_ v1 = { A{} }; 
	Union_A_or_B_or_C_ v2 = std::visit([](const auto& auto1) -> Union_A_or_B_or_C_ { return auto1; }, v1);
}

int main(int argc, char** argv) {
	std::vector<std::string> args {};
	for (int i = 0; i < argc; ++i)
		args.push_back(std::string(argv[i]));
	return _redirect_main(args);
}
