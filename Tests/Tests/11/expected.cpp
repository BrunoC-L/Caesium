#include "header.h"
struct C {
};

struct B {
};

struct A {
};

Int _redirect_main(const Vector<String>& s) {
	Variant<A, B> v1 = {};
	Variant<A, B, C> v2 = v1;
};

int main(int argc, char** argv) {
	std::vector<std::string> args {};
	for (int i = 0; i < argc; ++i)
		args.push_back(std::string(argv[i]));
	return _redirect_main(args);
};
