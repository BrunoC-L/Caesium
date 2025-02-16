#include "grammar/grammar.hpp"
#include "structured/structurizer.hpp"
#include <set>

int main() {
	puts("Hello, World!\n");
	grammar::File{ 0 };
	getExpressions(std::string{}, {}, {});
	/*NodeStructs::Typename t{ NodeStructs::BaseTypename{ "T" }, NodeStructs::Value{}, rule_info{
		.file_name = "stub:/",
		.content = "stub",
		.beg = { 0, 0 },
		.end = { 0, 0 }
	} };
	auto t2 = copy(t);
	NodeStructs::Function f{ .name = "", .name_space = {}, .returnType = copy(t2), .parameters = {}, .statements = {} };*/
	//bool b1 = t2 < t2;
	//bool b2 = f < f;
	//auto b3 = f <=> f;
	/*std::set<NodeStructs::Function> s;
	s.insert(std::move(f));*/
	//std::set<NodeStructs::Type> s;
	//s.insert(std::move(f));
	//std::map<NodeStructs::Function, int> m;
	//m.insert({ std::move(f), 0 });
	return 0;
}
