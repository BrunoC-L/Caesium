#include "header.h"
struct Cat {
String name;
};

struct Dog {
String name;
};

Int _redirect_main(const Vector<String>& s) {
	Vector<Named> vec = {};
	push(vec, Dog{"doggo"});
	push(vec, Cat{"gato"});
	for (auto&& e : vec) {
	}
	return 0;
};

int main(int argc, char** argv) {
	std::vector<std::string> args {};
	for (int i = 0; i < argc; ++i)
		args.push_back(std::string(argv[i]));
	return _redirect_main(args);
};
