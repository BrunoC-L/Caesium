#include "expected.hpp"
struct Token___WORD___ {
String value;
Int n_indent;
};

Int _redirect_main(const Vector<String>& args) {
	Token___WORD___ w = Token___WORD___{String{"hi"}, 0};
	return 0;
};

int main(int argc, char** argv) {
	std::vector<std::string> args {};
	for (int i = 0; i < argc; ++i)
		args.push_back(std::string(argv[i]));
	return _redirect_main(args);
};
