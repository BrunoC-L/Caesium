#include "defaults.hpp"

struct A;
struct B;

struct A {
};

struct B {
};


Int _redirect_main(const Vector<String>& s);

Int _redirect_main(const Vector<String>& s) {
	Variant<A, B> var = Variant<A, B>{A{}};
};

int main(int argc, char** argv) {
	std::vector<std::string> args {};
	for (int i = 0; i < argc; ++i)
		args.push_back(std::string(argv[i]));
	return _redirect_main(args);
};
