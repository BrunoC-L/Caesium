#include "header.h"
struct Dog {
};

struct Cat {
};

using Animal = std::variant<Cat, Dog>;

Int _redirect_main(const Vector<String>& s) {
    Vector<Animal> animals = {};
    return size(animals);
};

int main(int argc, char** argv) {
	std::vector<std::string> args {};
	for (int i = 0; i < argc; ++i)
		args.push_back(std::string(argv[i]));
	return _redirect_main(args);
};