#include "header.h"

Int f_Int() {
    return 0;
}

Int _redirect_main(const Vector<String>& s) {
	return f_Int();
};

int main(int argc, char** argv) {
	std::vector<std::string> args {};
	for (int i = 0; i < argc; ++i)
		args.push_back(std::string(argv[i]));
	return _redirect_main(args);
};
