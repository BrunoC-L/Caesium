#include "expected.hpp"
CatsAndDogs add(CatsAndDogs&& cats_and_dogs, Cat&& cat) {
	push(cats_and_dogs.cats, std::move(cat));
	return cats_and_dogs;
};
struct CatsAndDogs {
Vector<Cat> cats;
Vector<Dog> dogs;
};

struct Dog {
String name;
};

struct Cat {
String name;
};

Int _redirect_main(const Vector<String>& s) {
	CatsAndDogs a = CatsAndDogs{};
	CatsAndDogs b = add(std::move(a), Cat{});
	CatsAndDogs c = add(std::move(b), Cat{});
	CatsAndDogs d = add(std::move(c), Cat{});
	return 0;
};

int main(int argc, char** argv) {
	std::vector<std::string> args {};
	for (int i = 0; i < argc; ++i)
		args.push_back(std::string(argv[i]));
	return _redirect_main(args);
};
