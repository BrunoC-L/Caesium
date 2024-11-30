#include "defaults.hpp"



Int f__Int();
using Vector_String_ = Vector<String>;
Int _redirect_main(const Vector_String_& args);

Int f__Int() {
	return 0;
};
Int _redirect_main(const Vector_String_& args) {
	return f__Int();
};

int main(int argc, char** argv) {
	std::vector<std::string> args {};
	for (int i = 0; i < argc; ++i)
		args.push_back(std::string(argv[i]));
	return _redirect_main(args);
};
