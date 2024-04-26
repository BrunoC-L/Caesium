#include "defaults.hpp"

struct Or__Token__WORD__Int;
struct Token__WORD;

struct Or__Token__WORD__Int {
Variant<Token__WORD, Int> _value;
};

struct Token__WORD {
String value;
Int n_indent;
};


Int _redirect_main(const Vector<String>& args);

Int _redirect_main(const Vector<String>& args) {
	Token__WORD w = Token__WORD{String{"hi"}, 0};
	Or__Token__WORD__Int t1 = Or__Token__WORD__Int{Token__WORD{String{"t1"}, 1}};
	Or__Token__WORD__Int t2 = { Token__WORD{String{"t2"}, 2} };
	return 0;
};

int main(int argc, char** argv) {
	std::vector<std::string> args {};
	for (int i = 0; i < argc; ++i)
		args.push_back(std::string(argv[i]));
	return _redirect_main(args);
};
