#include "defaults.hpp"



Int _redirect_main(const Vector<String>& s);

Int _redirect_main(const Vector<String>& s) {
	(2 + 2 + 2 * 2 * 2 + 2) > (2 * 2 - 2 - ((2 * 2 - 2)) / 2 + 2);
};

int main(int argc, char** argv) {
	std::vector<std::string> args {};
	for (int i = 0; i < argc; ++i)
		args.push_back(std::string(argv[i]));
	return _redirect_main(args);
};
