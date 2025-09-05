#include "defaults.hpp"

struct A;
struct B;

struct A {
};

struct B {
};


using Union_A_or_B_ = Union<A, B>;
using Union_filesystem__directory_or_filesystem__file_ = Union<filesystem__directory, filesystem__file>;
using Vector_String_ = Vector<String>;
Int _redirect_main(const Vector_String_& s);

Int _redirect_main(const Vector_String_& s) {
	Union_A_or_B_ var = Union_A_or_B_{A{}};
}

int main(int argc, char** argv) {
	std::vector<std::string> args {};
	for (int i = 0; i < argc; ++i)
		args.push_back(std::string(argv[i]));
	return _redirect_main(args);
}
