#include "header.h"
Bool bop1(const String& s) {
return True;
};
Bool bop2(String& s) {
return True;
};
Bool bop3(String&& s) {
return True;
};
struct Main {
Int main(const Vector<String>& cli_args) {
for (auto&& arg : cli_args) {
if (bop1(arg)) {
return ;
}}
};
};

int main(int argc, char** argv) {
	std::vector<std::string> args {};
	for (int i = 0; i < argc; ++i)
		args.push_back(std::string(argv[i]));
	return Main{}.main(std::move(args));
};
