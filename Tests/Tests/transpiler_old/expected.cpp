#include "expected.hpp"
Bool test_transpile_all_folders(const Vector<Variant<filesystem__directory, filesystem__file>>& folders) {
	for (auto&& dir : folders) {
		const Variant<filesystem__directory, filesystem__file>& matchval1 = dir;
		if (std::holds_alternative<filesystem__directory>(matchval1)) {
			const filesystem__directory& d = std::get<filesystem__directory>(matchval1);
			return test_transpile_folder(d);
		} else
		if (std::holds_alternative<filesystem__file>(matchval1)) {
			const filesystem__file& f = std::get<filesystem__file>(matchval1);
		} else
			throw;
	}
	return True;
};
Bool test_transpile_folder(const filesystem__directory& dir) {
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
