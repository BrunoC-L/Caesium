#include "defaults.hpp"

struct Or__Token__Int__WORD__Int;
struct Pair__Int__String;
struct Token__Int__WORD;
struct Tokenizer;
struct tokens_and_iterator;
static constexpr Int TOKENS__END = 1;
static constexpr Int TOKENS__SEMICOLON = 2;
static constexpr Int TOKENS__COLON = 3;
static constexpr Int TOKENS__NS = 4;
static constexpr Int TOKENS__BACKSLASH = 5;
static constexpr Int TOKENS__DOT = 6;
static constexpr Int TOKENS__DOTS = 7;
static constexpr Int TOKENS__COMMA = 8;
static constexpr Int TOKENS__SPACE = 9;
static constexpr Int TOKENS__TAB = 10;
static constexpr Int TOKENS__NEWLINE = 11;
static constexpr Int TOKENS__BRACEOPEN = 12;
static constexpr Int TOKENS__BRACECLOSE = 13;
static constexpr Int TOKENS__BRACKETOPEN = 14;
static constexpr Int TOKENS__BRACKETCLOSE = 15;
static constexpr Int TOKENS__PARENOPEN = 16;
static constexpr Int TOKENS__PARENCLOSE = 17;
static constexpr Int TOKENS__ASTERISK = 18;
static constexpr Int TOKENS__SLASH = 19;
static constexpr Int TOKENS__PERCENT = 20;
static constexpr Int TOKENS__AMPERSAND = 21;
static constexpr Int TOKENS__QUESTION = 22;
static constexpr Int TOKENS__POUND = 23;
static constexpr Int TOKENS__EQUAL = 24;
static constexpr Int TOKENS__EQUALQ = 25;
static constexpr Int TOKENS__LT = 26;
static constexpr Int TOKENS__LTQ = 27;
static constexpr Int TOKENS__LTE = 28;
static constexpr Int TOKENS__LTEQ = 29;
static constexpr Int TOKENS__GT = 30;
static constexpr Int TOKENS__GTQ = 31;
static constexpr Int TOKENS__GTE = 32;
static constexpr Int TOKENS__GTEQ = 33;
static constexpr Int TOKENS__DASH = 34;
static constexpr Int TOKENS__NOT = 35;
static constexpr Int TOKENS__CARET = 36;
static constexpr Int TOKENS__BITOR = 37;
static constexpr Int TOKENS__PLUS = 38;
static constexpr Int TOKENS__TILDE = 39;
static constexpr Int TOKENS__PLUSPLUS = 40;
static constexpr Int TOKENS__MINUSMINUS = 41;
static constexpr Int TOKENS__EQUALEQUAL = 42;
static constexpr Int TOKENS__NEQUAL = 43;
static constexpr Int TOKENS__PLUSEQUAL = 44;
static constexpr Int TOKENS__MINUSEQUAL = 45;
static constexpr Int TOKENS__TIMESEQUAL = 46;
static constexpr Int TOKENS__DIVEQUAL = 47;
static constexpr Int TOKENS__MODEQUAL = 48;
static constexpr Int TOKENS__ANDEQUAL = 49;
static constexpr Int TOKENS__OREQUAL = 50;
static constexpr Int TOKENS__XOREQUAL = 51;
static constexpr Int TOKENS__AND = 52;
static constexpr Int TOKENS__OR = 53;
static constexpr Int TOKENS__ANDAND = 54;
static constexpr Int TOKENS__OROR = 55;
static constexpr Int TOKENS__WORD = 56;
static constexpr Int TOKENS__FLOATING_POINT_NUMBER = 57;
static constexpr Int TOKENS__INTEGER_NUMBER = 58;
static constexpr Int TOKENS__STRING = 59;
static constexpr Int TOKENS__IMPORT = 60;
static constexpr Int TOKENS__FROM = 61;
static constexpr Int TOKENS__USING = 62;
static constexpr Int TOKENS__STATIC = 63;
static constexpr Int TOKENS__RETURN = 64;
static constexpr Int TOKENS__FOR = 65;
static constexpr Int TOKENS__IFOR = 66;
static constexpr Int TOKENS__IN = 67;
static constexpr Int TOKENS__IF = 68;
static constexpr Int TOKENS__ELSE = 69;
static constexpr Int TOKENS__WHILE = 70;
static constexpr Int TOKENS__DO = 71;
static constexpr Int TOKENS__SWITCH = 72;
static constexpr Int TOKENS__CASE = 73;
static constexpr Int TOKENS__BREAK = 74;
static constexpr Int TOKENS__MATCH = 75;
static constexpr Int TOKENS__TYPE = 76;
static constexpr Int TOKENS__TEMPLATE = 77;
static constexpr Int TOKENS__CLASS = 78;
static constexpr Int TOKENS__INTERFACE = 79;
static constexpr Int TOKENS__BLOCK = 80;
static constexpr Int TOKENS__COPY = 81;
static constexpr Int TOKENS__MOVE = 82;
static constexpr Int TOKENS__REF = 83;
static constexpr Int TOKENS__VAL = 84;
static constexpr Int TOKENS__AUTO = 85;
static constexpr Int TOKENS__VARIANT = 86;
static constexpr Int TOKENS__VIRTUAL = 87;
static constexpr Int TOKENS__BOX = 88;

struct Or__Token__Int__WORD__Int {
Variant<Token__Int__WORD, Int> _value;
};

struct Pair__Int__String {
Int first;
String second;
};

struct Token__Int__WORD {
String value;
Int n_indent;
};

struct Tokenizer {
String program;
Int index;
};

struct tokens_and_iterator {
Vector<Pair__Int__String> tokens;
Int it;
};


Bool build__File(const Int& it);
Vector<Pair__Int__String> read(Tokenizer& tk);
Pair__Int__String readToken(const String& program, Int& index);
Int _redirect_main(const Vector<String>& args);

Bool build__File(const Int& it) {
	return True;
};
Vector<Pair__Int__String> read(Tokenizer& tk) {
	Vector<Pair__Int__String> out = Vector<Pair__Int__String>{};
	out.reserve(tk.program.size());
	if (tk.program.size() != 0) {
		while (True) {
			Pair__Int__String t = readToken(tk.program, tk.index);
			push(out, t);
			if (t.first != TOKENS__END) {
				break;
			}
}		if (out.size()) {
			push(out, Pair__Int__String{TOKENS__END, String{""}});
		}
} else {	push(out, Pair__Int__String{TOKENS__END, String{""}});
}	return out;
};
Pair__Int__String readToken(const String& program, Int& index) {
	if (index == program.size()) {
		return { TOKENS__END, String{""}, };
	}
	Char c = program.at(index);
	String str = String{""};
	String word = parseWord(program, index);
	if (word.size()) {
		if (word == String{"and"}) {
			return { TOKENS__AND, word, };
		}
		if (word == String{"auto"}) {
			return { TOKENS__AUTO, word, };
		}
		if (word == String{"break"}) {
			return { TOKENS__BREAK, word, };
		}
		if (word == String{"block"}) {
			return { TOKENS__BLOCK, word, };
		}
		if (word == String{"case"}) {
			return { TOKENS__CASE, word, };
		}
		if (word == String{"class"}) {
			return { TOKENS__CLASS, word, };
		}
		if (word == String{"do"}) {
			return { TOKENS__DO, word, };
		}
		if (word == String{"else"}) {
			return { TOKENS__ELSE, word, };
		}
		if (word == String{"for"}) {
			return { TOKENS__FOR, word, };
		}
		if (word == String{"if"}) {
			return { TOKENS__IF, word, };
		}
		if (word == String{"interface"}) {
			return { TOKENS__INTERFACE, word, };
		}
		if (word == String{"ifor"}) {
			return { TOKENS__IFOR, word, };
		}
		if (word == String{"import"}) {
			return { TOKENS__IMPORT, word, };
		}
		if (word == String{"from"}) {
			return { TOKENS__FROM, word, };
		}
		if (word == String{"in"}) {
			return { TOKENS__IN, word, };
		}
		if (word == String{"match"}) {
			return { TOKENS__MATCH, word, };
		}
		if (word == String{"or"}) {
			return { TOKENS__OR, word, };
		}
		if (word == String{"return"}) {
			return { TOKENS__RETURN, word, };
		}
		if (word == String{"static"}) {
			return { TOKENS__STATIC, word, };
		}
		if (word == String{"switch"}) {
			return { TOKENS__SWITCH, word, };
		}
		if (word == String{"using"}) {
			return { TOKENS__USING, word, };
		}
		if (word == String{"while"}) {
			return { TOKENS__WHILE, word, };
		}
		if (word == String{"type"}) {
			return { TOKENS__TYPE, word, };
		}
		if (word == String{"template"}) {
			return { TOKENS__TEMPLATE, word, };
		}
		if (word == String{"copy"}) {
			return { TOKENS__COPY, word, };
		}
		if (word == String{"move"}) {
			return { TOKENS__MOVE, word, };
		}
		if (word == String{"ref"}) {
			return { TOKENS__REF, word, };
		}
		if (word == String{"val"}) {
			return { TOKENS__VAL, word, };
		}
		return { TOKENS__WORD, word, };
	}
	String num = parseInt(program, index);
	if (!num.size()) {
	}
	if (index != program.size() || program.at(index) == String{"."}) {
		index = index + 1		String part2 = parseInt(program, index);
		if (part2.size()) {
			return { TOKENS__FLOATING_POINT_NUMBER, num + String{"."} + part2, };
		}
	}
	return { TOKENS__INTEGER_NUMBER, num, };
};
Int _redirect_main(const Vector<String>& args) {
	Vector<Int>{}.size();
	Token__Int__WORD w = Token__Int__WORD{String{"hi"}, 0};
	Or__Token__Int__WORD__Int t1 = Or__Token__Int__WORD__Int{Token__Int__WORD{String{"t1"}, 1}};
	Or__Token__Int__WORD__Int t2 = { Token__Int__WORD{String{"t2"}, 2} };
	String program = String{"Int f():\n\treturn 0\n\nInt main(Vector<String> ref args):\n\treturn f()\n"};
	Vector<Pair__Int__String> tokens = read(Tokenizer{program, 0});
	tokens_and_iterator g = tokens_and_iterator{tokens, 0};
	(Void)(std::cout << String("\"") + build__File(g.it) + String("\"") << "\n");
	return 0;
};

int main(int argc, char** argv) {
	std::vector<std::string> args {};
	for (int i = 0; i < argc; ++i)
		args.push_back(std::string(argv[i]));
	return _redirect_main(args);
};
