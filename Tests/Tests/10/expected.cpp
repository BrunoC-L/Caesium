#include "defaults.hpp"

struct A;
struct B;

struct A {
};

struct B {
};


Int _redirect_main(const Vector<String>& s);

Int _redirect_main(const Vector<String>& s) {
	Union<A, B> v1 = Union<A, B>{A{}};
	Union<A, B> v2 = Union<A, B>{B{}};
};

int main(int argc, char** argv) {
	std::vector<std::string> args {};
	for (int i = 0; i < argc; ++i)
		args.push_back(std::string(argv[i]));
	return _redirect_main(args);
};
