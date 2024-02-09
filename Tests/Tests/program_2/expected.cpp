#include "header.h"
Int f_Int_String() {
		return 0;
};
Int f_String_Int() {
		return 1;
};
Int f_String_String() {
		return 2;
};
Int _redirect_main(const Vector<String>& s) {
	return f_String_String() + f_String_Int() + f_Int_String();
};

int main(int argc, char** argv) {
	std::vector<std::string> args {};
	for (int i = 0; i < argc; ++i)
		args.push_back(std::string(argv[i]));
	return _redirect_main(args);
};
