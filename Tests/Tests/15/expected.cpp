#include "defaults.hpp"



Int f_Int_String_();
Int f_String_Int_();
Int f_String_String_();
using Vector_String_ = Vector<String>;
Int _redirect_main(const Vector_String_& s);

Int f_Int_String_() {
	return 0;
};
Int f_String_Int_() {
	return 1;
};
Int f_String_String_() {
	return 2;
};
Int _redirect_main(const Vector_String_& s) {
	return (f_String_String_() + f_String_Int_() + f_Int_String_());
};

int main(int argc, char** argv) {
	std::vector<std::string> args {};
	for (int i = 0; i < argc; ++i)
		args.push_back(std::string(argv[i]));
	return _redirect_main(args);
};
