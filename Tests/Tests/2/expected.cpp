#include "defaults.hpp"



using Vector_String_ = Vector<String>;
Int _redirect_main(const Vector_String_& cli_args);

Int _redirect_main(const Vector_String_& cli_args) {
	for (auto&& arg : cli_args) {
		(Void)(std::cout << ('"' + arg + '"') << "\n");
	}
	return 0;
};

int main(int argc, char** argv) {
	std::vector<std::string> args {};
	for (int i = 0; i < argc; ++i)
		args.push_back(std::string(argv[i]));
	return _redirect_main(args);
};
