#include "defaults.hpp"

struct Cat;
struct CatsAndDogs;
struct Dog;

struct Cat {
String name;
};

struct CatsAndDogs {
Vector<Cat> cats;
Vector<Dog> dogs;
};

struct Dog {
String name;
};


CatsAndDogs add(CatsAndDogs&& cats_and_dogs, Cat&& cat);
Int _redirect_main(const Vector<String>& s);

CatsAndDogs add(CatsAndDogs&& cats_and_dogs, Cat&& cat) {
	push(cats_and_dogs.cats, std::move(cat));
	return cats_and_dogs;
};
Int _redirect_main(const Vector<String>& s) {
	CatsAndDogs a = CatsAndDogs{Vector<Cat>{}, Vector<Dog>{}};
	CatsAndDogs b = add(std::move(a), Cat{String{"cat1"}});
	CatsAndDogs c = add(std::move(b), Cat{String{"cat2"}});
	CatsAndDogs d = add(std::move(c), Cat{String{"cat3"}});
	return 0;
};

int main(int argc, char** argv) {
	std::vector<std::string> args {};
	for (int i = 0; i < argc; ++i)
		args.push_back(std::string(argv[i]));
	return _redirect_main(args);
};
