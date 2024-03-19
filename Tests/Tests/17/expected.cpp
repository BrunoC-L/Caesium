#include "expected.hpp"
struct Cat {
String name;
};

struct Dog {
String name;
};

Int _redirect_main(const Vector<String>& s) {
	Vector<Named> vec = Vector<Named>{};
	push(vec, Dog{String{"doggo"}});
	push(vec, Cat{String{"gato"}});
	for (auto&& e : vec) {
		(Void)(std::cout << "Named{name = " << String("\"") + std::visit(overload([&](auto&& XX){ return XX.name; }), e) + String("\"") << "}" << "\n");
	}
	return 0;
};

int main(int argc, char** argv) {
	std::vector<std::string> args {};
	for (int i = 0; i < argc; ++i)
		args.push_back(std::string(argv[i]));
	return _redirect_main(args);
};
