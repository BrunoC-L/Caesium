#include "defaults.hpp"



using Union_filesystem__directory_or_filesystem__file_ = Union<filesystem__directory, filesystem__file>;
using Vector_String_ = Vector<String>;
Int f_Int_();
Int _redirect_main(const Vector_String_& args);

Int f_Int_() {
	return 0;
}
Int _redirect_main(const Vector_String_& args) {
	return f_Int_();
}

int main(int argc, char** argv) {
	std::vector<std::string> args {};
	for (int i = 0; i < argc; ++i)
		args.push_back(std::string(argv[i]));
	return _redirect_main(args);
}
