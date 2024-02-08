#include "header.h"
Int f_Animal(const Vector<Variant<Cat, Dog>>& vec) {
		return 0;
};
struct Cat {
};

struct Dog {
};

Int _redirect_main(const Vector<String>& s) {
	Vector<Variant<Cat, Dog>> animals = {};
	return f_Animal(animals);
};

int main(int argc, char** argv) {
	std::vector<std::string> args {};
	for (int i = 0; i < argc; ++i)
		args.push_back(std::string(argv[i]));
	return _redirect_main(args);
};
