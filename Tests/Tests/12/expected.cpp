#include "expected.hpp"
struct A {
B b;
};

struct B {
};

Int _redirect_main(const Vector<String>& s) {
	A a = A{};
};

int main(int argc, char** argv) {
	std::vector<std::string> args {};
	for (int i = 0; i < argc; ++i)
		args.push_back(std::string(argv[i]));
	return _redirect_main(args);
};
