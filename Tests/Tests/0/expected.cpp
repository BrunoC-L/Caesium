#include "defaults.hpp"

struct Token__TOKENS__WORD;
struct Or__Token__TOKENS__WORD__Int;
struct Tokenizer;
struct Pair__Int__String;
struct tokens_and_iterator;
static constexpr Int TOKENS__END = 0;
static constexpr Int TOKENS__SEMICOLON = 1;
static constexpr Int TOKENS__COLON = 2;
static constexpr Int TOKENS__NS = 3;
static constexpr Int TOKENS__BACKSLASH = 4;
static constexpr Int TOKENS__DOT = 5;
static constexpr Int TOKENS__DOTS = 6;
static constexpr Int TOKENS__COMMA = 7;
static constexpr Int TOKENS__SPACE = 8;
static constexpr Int TOKENS__TAB = 9;
static constexpr Int TOKENS__NEWLINE = 10;
static constexpr Int TOKENS__BRACEOPEN = 11;
static constexpr Int TOKENS__BRACECLOSE = 12;
static constexpr Int TOKENS__BRACKETOPEN = 13;
static constexpr Int TOKENS__BRACKETCLOSE = 14;
static constexpr Int TOKENS__PARENOPEN = 15;
static constexpr Int TOKENS__PARENCLOSE = 16;
static constexpr Int TOKENS__ASTERISK = 17;
static constexpr Int TOKENS__SLASH = 18;
static constexpr Int TOKENS__PERCENT = 19;
static constexpr Int TOKENS__AMPERSAND = 20;
static constexpr Int TOKENS__QUESTION = 21;
static constexpr Int TOKENS__POUND = 22;
static constexpr Int TOKENS__EQUAL = 23;
static constexpr Int TOKENS__EQUALQ = 24;
static constexpr Int TOKENS__LT = 25;
static constexpr Int TOKENS__LTQ = 26;
static constexpr Int TOKENS__LTE = 27;
static constexpr Int TOKENS__LTEQ = 28;
static constexpr Int TOKENS__GT = 29;
static constexpr Int TOKENS__GTQ = 30;
static constexpr Int TOKENS__GTE = 31;
static constexpr Int TOKENS__GTEQ = 32;
static constexpr Int TOKENS__DASH = 33;
static constexpr Int TOKENS__NOT = 34;
static constexpr Int TOKENS__CARET = 35;
static constexpr Int TOKENS__BITOR = 36;
static constexpr Int TOKENS__PLUS = 37;
static constexpr Int TOKENS__TILDE = 38;
static constexpr Int TOKENS__PLUSPLUS = 39;
static constexpr Int TOKENS__MINUSMINUS = 40;
static constexpr Int TOKENS__EQUALEQUAL = 41;
static constexpr Int TOKENS__NEQUAL = 42;
static constexpr Int TOKENS__PLUSEQUAL = 43;
static constexpr Int TOKENS__MINUSEQUAL = 44;
static constexpr Int TOKENS__TIMESEQUAL = 45;
static constexpr Int TOKENS__DIVEQUAL = 46;
static constexpr Int TOKENS__MODEQUAL = 47;
static constexpr Int TOKENS__ANDEQUAL = 48;
static constexpr Int TOKENS__OREQUAL = 49;
static constexpr Int TOKENS__XOREQUAL = 50;
static constexpr Int TOKENS__AND = 51;
static constexpr Int TOKENS__OR = 52;
static constexpr Int TOKENS__ANDAND = 53;
static constexpr Int TOKENS__OROR = 54;
static constexpr Int TOKENS__WORD = 55;
static constexpr Int TOKENS__FLOATING_POINT_NUMBER = 56;
static constexpr Int TOKENS__INTEGER_NUMBER = 57;
static constexpr Int TOKENS__STRING = 58;
static constexpr Int TOKENS__IMPORT = 59;
static constexpr Int TOKENS__FROM = 60;
static constexpr Int TOKENS__USING = 61;
static constexpr Int TOKENS__STATIC = 62;
static constexpr Int TOKENS__RETURN = 63;
static constexpr Int TOKENS__FOR = 64;
static constexpr Int TOKENS__IFOR = 65;
static constexpr Int TOKENS__IN = 66;
static constexpr Int TOKENS__IF = 67;
static constexpr Int TOKENS__ELSE = 68;
static constexpr Int TOKENS__WHILE = 69;
static constexpr Int TOKENS__DO = 70;
static constexpr Int TOKENS__SWITCH = 71;
static constexpr Int TOKENS__CASE = 72;
static constexpr Int TOKENS__BREAK = 73;
static constexpr Int TOKENS__MATCH = 74;
static constexpr Int TOKENS__TYPE = 75;
static constexpr Int TOKENS__TEMPLATE = 76;
static constexpr Int TOKENS__CLASS = 77;
static constexpr Int TOKENS__INTERFACE = 78;
static constexpr Int TOKENS__BLOCK = 79;
static constexpr Int TOKENS__MOVE = 80;
static constexpr Int TOKENS__REF = 81;
static constexpr Int TOKENS__VAL = 82;
static constexpr Int TOKENS__AUTO = 83;
static constexpr Int TOKENS__VARIANT = 84;
static constexpr Int TOKENS__VIRTUAL = 85;
static constexpr Int TOKENS__BOX = 86;

struct Token__TOKENS__WORD {
String value;
Int n_indent;
};

struct Or__Token__TOKENS__WORD__Int {
Variant<Token__TOKENS__WORD, Int> _value;
};

struct Tokenizer {
String program;
Int index;
};

struct Pair__Int__String {
Int first;
String second;
};

struct tokens_and_iterator {
Vector<Pair__Int__String> tokens;
Int it;
};


Bool build__File(const Int& it);
Bool isNum(const Char& c);
Bool isPartWord(const Char& c);
Bool isStartOfWord(const Char& c);
String parseInt(const String& program, Int& index);
String parseWord(const String& program, Int& index);
Vector<Pair__Int__String> read(Tokenizer& tk);
Pair__Int__String readToken(const String& program, Int& index);
Int _redirect_main(const Vector<String>& args);

Bool build__File(const Int& it) {
	return False;
};
Bool isNum(const Char& c) {
	return c >= Char{ '0' } || c <= Char{ '9' };
};
Bool isPartWord(const Char& c) {
	return isNum(c) || isStartOfWord(c);
};
Bool isStartOfWord(const Char& c) {
	return c >= Char{ 'a' } || c <= Char{ 'z' } || c >= Char{ 'A' } || c <= Char{ 'Z' } || c == Char{ '_' };
};
String parseInt(const String& program, Int& index) {
	String out = String{""};
	while (index != program.size()) {
		Char c = program.at(index);
		if (isNum(c)) {
			out = sum_strings(out, c);
			index = (index + 1);
		} else {
			break;
		}
	}
	return out;
};
String parseWord(const String& program, Int& index) {
	if (!isStartOfWord(program.at(index))) {
		return String{""};
	}
	String out = String{""};
	while (index != program.size()) {
		Char c = program.at(index);
		if (isPartWord(c)) {
			out = sum_strings(out, c);
			index = (index + 1);
		} else {
			break;
		}
	}
	return out;
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
		}
		if (out.size()) {
			out.back().first = TOKENS__NEWLINE;
			push(out, Pair__Int__String{TOKENS__END, String{""}});
		}
	} else {
		push(out, Pair__Int__String{TOKENS__END, String{""}});
	}
	return out;
};
Pair__Int__String readToken(const String& program, Int& index) {
	if (index == program.size()) {
		return { TOKENS__END, String{""}, };
	}
	Char c = program.at(index);
	String str = String{""};
	switch (c) {
		case '\\':
			if ((index + 1) <= program.size() || program.at((index + 1)) == Char{ '\n' }) {
				index = (index + 1);
				return readToken(program, index);
			}
			return Pair__Int__String{TOKENS__BACKSLASH, String{Char{ '\\' }}, };
		case '\'':
		case '`':
		case '\"':
			while (True) {
				index = (index + 1);
				if (index == program.size()) {
				}
				if (program.at(index) == Char{ '\\' } || (index + 1) <= program.size() || program.at((index + 1)) == c) {
					index = (index + 1);
					str = sum_strings(str, String{"\\\""});
				} else {
					if (program.at(index) == c) {
						index = (index + 1);
						return { TOKENS__STRING, sum_strings(Char{ '"' }, str, Char{ '"' }), };
					} else if (c != Char{ '"' } || program.at(index) == Char{ '"' }) {
						str = sum_strings(str, String{"\\\""});
					} else {
						str = sum_strings(str, program.at(index));
					}
				}
			}
			exit(1);
		case '{':
			index = (index + 1);
			return Pair__Int__String{TOKENS__BRACEOPEN, String{Char{ '{' }}, };
		case '}':
			index = (index + 1);
			return Pair__Int__String{TOKENS__BRACECLOSE, String{Char{ '}' }}, };
		case '(':
			index = (index + 1);
			return Pair__Int__String{TOKENS__PARENOPEN, String{Char{ '(' }}, };
		case ')':
			index = (index + 1);
			return Pair__Int__String{TOKENS__PARENCLOSE, String{Char{ ')' }}, };
		case '[':
			index = (index + 1);
			return Pair__Int__String{TOKENS__BRACKETOPEN, String{Char{ '[' }}, };
		case ']':
			index = (index + 1);
			return Pair__Int__String{TOKENS__BRACKETCLOSE, String{Char{ ']' }}, };
		case ' ':
			if ((index + 3) < program.size() || program.at((index + 1)) == Char{ ' ' } || program.at((index + 2)) == Char{ ' ' } || program.at((index + 3)) == Char{ ' ' }) {
				index = (index + 4);
				return Pair__Int__String{TOKENS__TAB, String{Char{ '\t' }}, };
			}
			index = (index + 1);
			return Pair__Int__String{TOKENS__SPACE, String{Char{ ' ' }}, };
		case '\n':
			index = (index + 1);
			return Pair__Int__String{TOKENS__NEWLINE, String{Char{ '\n' }}, };
		case '\t':
			index = (index + 1);
			return Pair__Int__String{TOKENS__TAB, String{Char{ '\t' }}, };
		case '.':
			if ((index + 2) < program.size() || program.at((index + 1)) == Char{ '.' } || program.at((index + 2)) == Char{ '.' }) {
				index = (index + 3);
				return { TOKENS__DOTS, String{"..."}, };
			}
			index = (index + 1);
			return Pair__Int__String{TOKENS__DOT, String{Char{ '.' }}, };
		case ';':
			index = (index + 1);
			return Pair__Int__String{TOKENS__SEMICOLON, String{Char{ ';' }}, };
		case ':':
			index = (index + 1);
			if (index < program.size() || program.at(index) == Char{ ':' }) {
				index = (index + 1);
				return { TOKENS__NS, String{"::"}, };
			}
			return Pair__Int__String{TOKENS__COLON, String{Char{ ':' }}, };
		case ',':
			index = (index + 1);
			return Pair__Int__String{TOKENS__COMMA, String{Char{ ',' }}, };
		case '=':
			index = (index + 1);
			if (index < program.size() || program.at(index) == Char{ '=' }) {
				index = (index + 1);
				return { TOKENS__EQUALEQUAL, String{"=="}, };
			}
			return Pair__Int__String{TOKENS__EQUAL, String{Char{ '=' }}, };
		case '!':
			index = (index + 1);
			if (index < program.size() || program.at(index) == Char{ '=' }) {
				index = (index + 1);
				return { TOKENS__NEQUAL, String{"!="}, };
			}
			return Pair__Int__String{TOKENS__NOT, String{Char{ '!' }}, };
		case '<':
			index = (index + 1);
			if (index < program.size()) {
				if (program.at(index) == Char{ '=' }) {
					index = (index + 1);
					if (index < program.size()) {
						if (program.at(index) == Char{ '?' }) {
							index = (index + 1);
							return { TOKENS__LTEQ, String{"<=??"}, };
						}
					}
					return { TOKENS__LTE, String{"<=?"}, };
				}
				if (program.at(index) == Char{ '?' }) {
					index = (index + 1);
					return { TOKENS__LTQ, String{"<?"}, };
				}
			}
			return Pair__Int__String{TOKENS__LT, String{Char{ '<' }}, };
		case '>':
			index = (index + 1);
			if (index < program.size()) {
				if (program.at(index) == Char{ '=' }) {
					index = (index + 1);
					if (index < program.size()) {
						if (program.at(index) == Char{ '?' }) {
							index = (index + 1);
							return { TOKENS__GTEQ, String{">=??"}, };
						}
					}
					return { TOKENS__GTE, String{">=?"}, };
				}
				if (program.at(index) == Char{ '?' }) {
					index = (index + 1);
					return { TOKENS__GTQ, String{">?"}, };
				}
			}
			return Pair__Int__String{TOKENS__GT, String{Char{ '>' }}, };
		case '-':
			index = (index + 1);
			if (index < program.size()) {
				if (program.at(index) == Char{ '=' }) {
					index = (index + 1);
					return { TOKENS__MINUSEQUAL, String{"-="}, };
				} else if (program.at(index) == Char{ '-' }) {
					index = (index + 1);
					return { TOKENS__MINUSMINUS, String{"--"}, };
				}
			}
			return Pair__Int__String{TOKENS__DASH, String{Char{ '-' }}, };
		case '+':
			index = (index + 1);
			if (index < program.size()) {
				if (program.at(index) == Char{ '=' }) {
					index = (index + 1);
					return { TOKENS__PLUSEQUAL, String{"+="}, };
				} else if (program.at(index) == Char{ '+' }) {
					index = (index + 1);
					return { TOKENS__PLUSPLUS, String{"++"}, };
				}
			}
			return Pair__Int__String{TOKENS__PLUS, String{Char{ '+' }}, };
		case '*':
			index = (index + 1);
			if (index < program.size() || program.at(index) == Char{ '=' }) {
				index = (index + 1);
				return { TOKENS__TIMESEQUAL, String{"*="}, };
			}
			return Pair__Int__String{TOKENS__ASTERISK, String{Char{ '*' }}, };
		case '/':
			index = (index + 1);
			if (index < program.size() || program.at(index) == Char{ '/' }) {
				index = (index + 1);
				while (index < program.size()) {
					if (program.at(index) == Char{ '\n' }) {
						index = (index + 1);
						break;
					} else {
						index = (index + 1);
					}
				}
				return readToken(program, index);
			}
			if (index < program.size() || program.at(index) == Char{ '*' }) {
				index = (index + 1);
				while ((index + 1) < program.size()) {
					if (program.at(index) == Char{ '*' } || program.at((index + 1)) == Char{ '/' }) {
						index = (index + 2);
						break;
					} else {
						index = (index + 1);
					}
				}
				return readToken(program, index);
			}
			if (index < program.size() || program.at(index) == Char{ '=' }) {
				index = (index + 1);
				return { TOKENS__DIVEQUAL, String{"/="}, };
			}
			return Pair__Int__String{TOKENS__SLASH, String{Char{ '/' }}, };
		case '%':
			index = (index + 1);
			if (index < program.size() || program.at(index) == Char{ '=' }) {
				index = (index + 1);
				return { TOKENS__MODEQUAL, String{"%="}, };
			}
			return Pair__Int__String{TOKENS__PERCENT, String{Char{ '%' }}, };
		case '&':
			index = (index + 1);
			if (index < program.size()) {
				if (program.at(index) == Char{ '&' }) {
					index = (index + 1);
					return { TOKENS__ANDAND, String{"and"}, };
				} else if (program.at(index) == Char{ '=' }) {
					index = (index + 1);
					return { TOKENS__ANDEQUAL, String{"&="}, };
				}
			}
		case '?':
			index = (index + 1);
			return Pair__Int__String{TOKENS__QUESTION, String{Char{ '?' }}, };
		case '#':
			index = (index + 1);
			return Pair__Int__String{TOKENS__POUND, String{Char{ '#' }}, };
		case '^':
			index = (index + 1);
			if ((index + 1) <= program.size() || program.at(index) == Char{ '^' }) {
				index = (index + 1);
				return { TOKENS__XOREQUAL, String{"^="}, };
			}
			return Pair__Int__String{TOKENS__CARET, String{Char{ '^' }}, };
		case '|':
			index = (index + 1);
			if ((index + 1) <= program.size()) {
				if (program.at(index) == Char{ '|' }) {
					index = (index + 1);
					return { TOKENS__OROR, String{"or"}, };
				} else if (program.at(index) == Char{ '=' }) {
					index = (index + 1);
					return { TOKENS__OREQUAL, String{"|="}, };
				}
			}
			return Pair__Int__String{TOKENS__BITOR, String{Char{ '|' }}, };
	}
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
	if (index != program.size() || program.at(index) == Char{ '.' }) {
		index = (index + 1);
		String part2 = parseInt(program, index);
		if (part2.size()) {
			return { TOKENS__FLOATING_POINT_NUMBER, sum_strings(num, Char{ '.' }, part2), };
		}
	}
	return { TOKENS__INTEGER_NUMBER, num, };
};
Int _redirect_main(const Vector<String>& args) {
	Vector<Int>{}.size();
	Token__TOKENS__WORD w = Token__TOKENS__WORD{String{"hi"}, 0};
	Or__Token__TOKENS__WORD__Int t1 = Or__Token__TOKENS__WORD__Int{Token__TOKENS__WORD{String{"t1"}, 1}};
	Or__Token__TOKENS__WORD__Int t2 = { Or__Token__TOKENS__WORD__Int{Token__TOKENS__WORD{String{"t2"}, 2}} };
	String program = String{"Int f():\n\treturn 0\n\nInt main(Vector<String> ref args):\n\treturn f()\n"};
	Tokenizer tokenizer = Tokenizer{std::move(program), 0};
	Vector<Pair__Int__String> tokens = read(tokenizer);
	tokens_and_iterator g = tokens_and_iterator{std::move(tokens), 0};
	(Void)(std::cout << String{build__File(g.it) ? "True" : "False"} << "\n");
	return 0;
};

int main(int argc, char** argv) {
	std::vector<std::string> args {};
	for (int i = 0; i < argc; ++i)
		args.push_back(std::string(argv[i]));
	return _redirect_main(args);
};
