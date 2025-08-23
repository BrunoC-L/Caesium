#include "defaults.hpp"

struct Dog;
struct Cat;

struct Dog {
};

struct Cat {
};


using Union_Cat_or_Dog_ = Union<Cat, Dog>;
using Union_filesystem__directory_or_filesystem__file_ = Union<filesystem__directory, filesystem__file>;
using Vector_String_ = Vector<String>;
using Vector_Union_Cat_or_Dog__ = Vector<Union_Cat_or_Dog_>;
Int f_Union_Cat_or_Dog__(const Vector_Union_Cat_or_Dog__& vec);
Int _redirect_main(const Vector_String_& s);

Int f_Union_Cat_or_Dog__(const Vector_Union_Cat_or_Dog__& vec) {
	return 0;
}
Int _redirect_main(const Vector_String_& s) {
	Vector_Union_Cat_or_Dog__ animals = Vector_Union_Cat_or_Dog__{};
	return f_Union_Cat_or_Dog__(animals);
}

int main(int argc, char** argv) {
	std::vector<std::string> args {};
	for (int i = 0; i < argc; ++i)
		args.push_back(std::string(argv[i]));
	return _redirect_main(args);
}
