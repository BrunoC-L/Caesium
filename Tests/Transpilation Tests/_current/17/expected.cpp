#include "defaults.hpp"

struct Dog;
struct Cat;
struct Named;

struct Dog {
	String name;
};

struct Cat {
	String name;
};

struct Named {
	Union<Dog, Cat> value;
};

using Union_filesystem__directory_or_filesystem__file_ = Union<filesystem__directory, filesystem__file>;
using Vector_Named_ = Vector<Named>;
using Vector_String_ = Vector<String>;
Int _redirect_main(const Vector_String_& s);

Int _redirect_main(const Vector_String_& s) {
	Vector_Named_ vec = Vector_Named_{};
	push(vec, Named{Dog{String{"doggo"}}});
	push(vec, Named{Cat{String{"gato"}}});
	return 0;
}

int main(int argc, char** argv) {
	std::vector<std::string> args {};
	for (int i = 0; i < argc; ++i)
		args.push_back(std::string(argv[i]));
	return _redirect_main(args);
}
