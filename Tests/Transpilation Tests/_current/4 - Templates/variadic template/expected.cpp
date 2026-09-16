#include "defaults.hpp"

struct A_Int_;

struct A_Int_ {
	Int x;
};


using Union_filesystem__directory_or_filesystem__file_ = Union<filesystem__directory, filesystem__file>;
using Vector_String_ = Vector<String>;
Int _redirect_main(const Vector_String_& args);

Int _redirect_main(const Vector_String_& args) {
	A_Int_ a = A_Int_{1};
}

int main(int argc, char** argv) {
	std::vector<std::string> args {};
	for (int i = 0; i < argc; ++i)
		args.push_back(std::string(argv[i]));
	return _redirect_main(args);
}
