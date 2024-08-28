#include "defaults.hpp"

struct Dog;
struct Cat;

struct Dog {
};

struct Cat {
};


Int f__Animal(const Vector<Union<Cat, Dog>>& vec);
Int _redirect_main(const Vector<String>& s);

Int f__Animal(const Vector<Union<Cat, Dog>>& vec) {
	return 0;
};
Int _redirect_main(const Vector<String>& s) {
	Vector<Union<Cat, Dog>> animals = Vector<Union<Cat, Dog>>{};
	return f__Animal(animals);
};

int main(int argc, char** argv) {
	std::vector<std::string> args {};
	for (int i = 0; i < argc; ++i)
		args.push_back(std::string(argv[i]));
	return _redirect_main(args);
};
