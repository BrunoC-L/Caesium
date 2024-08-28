#include "defaults.hpp"

struct Dog;
struct Cat;
struct Named;

struct Dog {
String name;
};

struct Cat {
String name;
};

struct Named {
	Union<Dog, Cat> value;
};

Int _redirect_main(const Vector<String>& s);

Int _redirect_main(const Vector<String>& s) {
	Vector<Named> vec = Vector<Named>{};
	push(vec, Dog{String{"doggo"}});
	push(vec, Cat{String{"gato"}});
	for (auto&& e : vec) {
		(Void)(std::cout << "Named{name = " << ('"' + std::visit(overload([&](auto&& XX){ return XX.name; }), e.value) + '"') << "}" << "\n");
	}
	return 0;
};

int main(int argc, char** argv) {
	std::vector<std::string> args {};
	for (int i = 0; i < argc; ++i)
		args.push_back(std::string(argv[i]));
	return _redirect_main(args);
};
