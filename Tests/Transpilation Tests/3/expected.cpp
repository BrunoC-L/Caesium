#include "defaults.hpp"



Bool bop1(const String& s);
using Vector_String_ = Vector<String>;
Int _redirect_main(const Vector_String_& cli_args);

Bool bop1(const String& s) {
	return True;
};
Int _redirect_main(const Vector_String_& cli_args) {
	for (auto&& arg : cli_args) {
		if (bop1(arg)) {
			return;
		}
	}
};

int main(int argc, char** argv) {
	std::vector<std::string> args {};
	for (int i = 0; i < argc; ++i)
		args.push_back(std::string(argv[i]));
	return _redirect_main(args);
}
