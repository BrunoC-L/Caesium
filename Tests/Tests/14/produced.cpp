#include "defaults.hpp"

struct Dog;
struct Cat;

struct Dog {
};

struct Cat {
};


Int f__Animal(const Vector<Animal>& vec);
Int _redirect_main(const Vector<String>& s);

Int f__Animal(const Vector<Animal>& vec) {
	return 0;
};
Int _redirect_main(const Vector<String>& s) {
	Vector<union__Dog_Cat> animals = Vector<union__Dog_Cat>{};
	return f__Animal(animals);
};

int main(int argc, char** argv) {
	std::vector<std::string> args {};
	for (int i = 0; i < argc; ++i)
		args.push_back(std::string(argv[i]));
	return _redirect_main(args);
};
