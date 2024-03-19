#include "expected.hpp"
Int _redirect_main(const Vector<String>& args) {
	Vector<String> x = Vector<String>{};
	push(x, String{"Hello, "});
	push(x, String{"World!\n"});
	for (auto&& s : x) {
		(Void)(std::cout << String("\"") + s + String("\""));
	}
	return 0;
};

int main(int argc, char** argv) {
	std::vector<std::string> args {};
	for (int i = 0; i < argc; ++i)
		args.push_back(std::string(argv[i]));
	return _redirect_main(args);
};
