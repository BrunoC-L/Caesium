#include "defaults.hpp"

struct Cat;
struct Dog;

struct Cat {
};

struct Dog {
};


Int f__Animal(const Vector<Variant<Cat, Dog>>& vec);
Int _redirect_main(const Vector<String>& s);

Int f__Animal(const Vector<Variant<Cat, Dog>>& vec) {
	return 0;
};
Int _redirect_main(const Vector<String>& s) {
	Vector<Variant<Cat, Dog>> animals = Vector<Variant<Cat, Dog>>{};
	return f__Animal(animals);
};

int main(int argc, char** argv) {
	std::vector<std::string> args {};
	for (int i = 0; i < argc; ++i)
		args.push_back(std::string(argv[i]));
	return _redirect_main(args);
};
