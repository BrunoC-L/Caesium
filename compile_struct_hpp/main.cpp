#include "grammar/grammar.hpp"
#include "structured/structurizer.hpp"
#include <set>

int main() {
	grammar::File{ 0 };
	puts("Hello, World!\n");
	NodeStructs::Typename t{ NodeStructs::BaseTypename{ "T" }, NodeStructs::Value{}, rule_info{
		.file_name = "stub:/",
		.content = "stub",
		.beg = { 0, 0 },
		.end = { 0, 0 }
	} };
	auto t2 = copy(t);
	bool b1 = t2 < t2;
	NodeStructs::Function f{ .name = "",.name_space = {},.returnType = copy(t2), .parameters = {},.statements = {} };
	const auto& f2 = f;
	f2 < f2;
	bool b2 = f < f;
	auto b3 = f <=> f;
	std::set<NodeStructs::Function, decltype([](const NodeStructs::Function& l, const NodeStructs::Function& r) { return l <=> r == std::weak_ordering::less; })> s;
	s.insert(std::move(f));
	return 0;
}
