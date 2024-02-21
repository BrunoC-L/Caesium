#include "expected.hpp"
struct B {
};

struct A {
};

Int _redirect_main(const Vector<String>& s) {
	Variant<A, B> var = {};
};

int main(int argc, char** argv) {
	std::vector<std::string> args {};
	for (int i = 0; i < argc; ++i)
		args.push_back(std::string(argv[i]));
	return _redirect_main(args);
};
