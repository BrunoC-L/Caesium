#include "defaults.hpp"



using Vector_String_ = Vector<String>;
Void println(const String& x);
Int _redirect_main(const Vector_String_& cli_args);

Void println(const String& x) {
	return;
}
Int _redirect_main(const Vector_String_& cli_args) {
	for (auto&& arg : cli_args) {
		println(arg);
	}
	return 0;
}

int main(int argc, char** argv) {
	std::vector<std::string> args {};
	for (int i = 0; i < argc; ++i)
		args.push_back(std::string(argv[i]));
	return _redirect_main(args);
}
