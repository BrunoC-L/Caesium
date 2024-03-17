#include "expected.hpp"
Bool test_transpile_all_folders(const Vector<Variant<filesystem__directory, filesystem__file>>& folder) {
	return False;
};
Bool test_parse() {
	return False;
};
Int _redirect_main(const Vector<String>& args) {
	Int argc = args.size();
	Int bad_exit = 1;
	if (argc == 1) {
		(Void)(std::cout << std::string("\"") + "Expected a test folder" + std::string("\"") << "\n");
		return bad_exit;
	}
	if (argc > 2) {
		(Void)(std::cout << std::string("\"") + "Expected a single argument" + std::string("\"") << "\n");
		return bad_exit;
	}
	if (!test_parse()) {
		return bad_exit;
	}
	(Void)(std::cout << std::string("\"") + "All parse tests passed" + std::string("\"") << "\n");
	if (!test_transpile_all_folders(filesystem__entries("C:/"))) {
		return bad_exit;
	}
	(Void)(std::cout << std::string("\"") + "All transpile tests passed" + std::string("\"") << "\n");
	return 0;
};

int main(int argc, char** argv) {
	std::vector<std::string> args {};
	for (int i = 0; i < argc; ++i)
		args.push_back(std::string(argv[i]));
	return _redirect_main(args);
};
