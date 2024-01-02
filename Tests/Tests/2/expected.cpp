#include "header.h"
Int _redirect_main(const Vector<String>& cli_args) {
	for (auto&& arg : cli_args) {
		println(arg);
	}
};

int main(int argc, char** argv) {
	std::vector<std::string> args {};
	for (int i = 0; i < argc; ++i)
		args.push_back(std::string(argv[i]));
	return _redirect_main(args);
};
