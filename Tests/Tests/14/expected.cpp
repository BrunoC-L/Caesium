#include "defaults.hpp"

struct Dog;
struct Cat;

struct Dog {
};

struct Cat {
};


Int f__Animal(const Vector_Animal_& vec);
using Vector_Animal_ = Vector<Animal>;
using Vector_String_ = Vector<String>;
using Vector_union__Dog_Cat_ = Vector<union__Dog_Cat>;
Int _redirect_main(const Vector_String_& s);

Int f__Animal(const Vector_Animal_& vec) {
	return 0;
};
Int _redirect_main(const Vector_String_& s) {
	Vector_union__Dog_Cat_ animals = Vector_union__Dog_Cat_{};
	return f__Animal(animals);
};

int main(int argc, char** argv) {
	std::vector<std::string> args {};
	for (int i = 0; i < argc; ++i)
		args.push_back(std::string(argv[i]));
	return _redirect_main(args);
};
