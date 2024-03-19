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
		(Void)(std::cout << String("\"") + String{"Expected a test folder"} + String("\"") << "\n");
		return bad_exit;
	}
	if (argc > 2) {
		(Void)(std::cout << String("\"") + String{"Expected a single argument"} + String("\"") << "\n");
		return bad_exit;
	}
	if (!test_parse()) {
		return bad_exit;
	}
	(Void)(std::cout << String("\"") + String{"All parse tests passed"} + String("\"") << "\n");
	if (!test_transpile_all_folders(filesystem__entries(String{"C:/"}))) {
		return bad_exit;
	}
	(Void)(std::cout << String("\"") + String{"All transpile tests passed"} + String("\"") << "\n");
	return 0;
};

int main(int argc, char** argv) {
	std::vector<std::string> args {};
	for (int i = 0; i < argc; ++i)
		args.push_back(std::string(argv[i]));
	return _redirect_main(args);
};
