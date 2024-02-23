#include "expected.hpp"
struct CatsAndDogs {
Vector<Cat> cats;
Vector<Dog> dogs;
};

struct Dog {
String name;
};

struct Cat {
String name;
};

Int _redirect_main(const Vector<String>& s) {
	CatsAndDogs c = {};
	return 0;
};

int main(int argc, char** argv) {
	std::vector<std::string> args {};
	for (int i = 0; i < argc; ++i)
		args.push_back(std::string(argv[i]));
	return _redirect_main(args);
};
