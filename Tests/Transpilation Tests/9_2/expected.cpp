#include "defaults.hpp"

struct A;
struct B;

struct A {
};

struct B {
};


using Union_A_B_ = Union<A, B>;
using Vector_String_ = Vector<String>;
Int _redirect_main(const Vector_String_& s);

Int _redirect_main(const Vector_String_& s) {
	Union_A_B_ var = Union_A_B_{A{}};
};

int main(int argc, char** argv) {
	std::vector<std::string> args {};
	for (int i = 0; i < argc; ++i)
		args.push_back(std::string(argv[i]));
	return _redirect_main(args);
}
