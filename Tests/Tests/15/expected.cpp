#include "defaults.hpp"



Int f__Int__String();
Int f__String__Int();
Int f__String__String();
Int _redirect_main(const Vector<String>& s);

Int f__Int__String() {
	return 0;
};
Int f__String__Int() {
	return 1;
};
Int f__String__String() {
	return 2;
};
Int _redirect_main(const Vector<String>& s) {
	return f__String__String() + f__String__Int() + f__Int__String();
};

int main(int argc, char** argv) {
	std::vector<std::string> args {};
	for (int i = 0; i < argc; ++i)
		args.push_back(std::string(argv[i]));
	return _redirect_main(args);
};
