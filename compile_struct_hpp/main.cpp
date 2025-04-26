//#include "grammar/grammar.hpp"
//#include "structured/structurizer.hpp"
//#include <set>

#include "helpers.hpp"

int main() {
	puts("Hello, World!\n");
	Realised::PrimitiveType pt{ Realised::PrimitiveType::NonValued<Realised::void_t>{} };
	auto t2 = copy(pt);
	puts("Hello, World!\n");
	using vt = std::variant<Variant<char>, Variant<int>>;
	vt m{ Variant<int>{1} };
	/*vt n{ std::visit([](const auto& e) {
		return vt{ []() ->std::remove_cvref_t<decltype(e)> {throw; }() };
		}, m) };*/

	//grammar::File{ 0 };
	//getExpressions(std::string{}, {}, {});
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
