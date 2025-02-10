#include "defaults.hpp"

struct Dog;
struct Cat;

struct Dog {
};

struct Cat {
};


Int f_Union_Cat_Dog__(const Vector_Union_Cat_Dog__& vec);
using Union_Cat_Dog_ = Union<Cat, Dog>;
using Vector_String_ = Vector<String>;
using Vector_Union_Cat_Dog__ = Vector<Union_Cat_Dog_>;
Int _redirect_main(const Vector_String_& s);

Int f_Union_Cat_Dog__(const Vector_Union_Cat_Dog__& vec) {
	return 0;
};
Int _redirect_main(const Vector_String_& s) {
	Vector_Union_Cat_Dog__ animals = Vector_Union_Cat_Dog__{};
	return f_Union_Cat_Dog__(animals);
};

int main(int argc, char** argv) {
	std::vector<std::string> args {};
	for (int i = 0; i < argc; ++i)
		args.push_back(std::string(argv[i]));
	return _redirect_main(args);
};
