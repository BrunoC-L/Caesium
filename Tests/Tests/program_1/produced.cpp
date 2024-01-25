#include "header.h"
struct Cat {
};

struct Dog {
};

Int _redirect_main(const Vector<String>& s) {
	Vector<std::variant<Cat, Dog>> animals = {};
	return size<std::variant<Cat, Dog>>(animals);
};

int main(int argc, char** argv) {
	std::vector<std::string> args {};
	for (int i = 0; i < argc; ++i)
		args.push_back(std::string(argv[i]));
	return _redirect_main(args);
};
