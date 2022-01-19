#pragma once
#include <sstream>
#include <vector>
#include <forward_list>

enum TOKENS {
	END,
	SEMICOLON,
	COLON,
	NS,
	BACKSLASH,
	DOT,
	COMMA,
	EQUAL,
	LT,
	GT,
	DASH,
	SPACE,
	TAB,
	NEWLINE,
	BRACEOPEN,
	BRACECLOSE,
	BRACKETOPEN,
	BRACKETCLOSE,
	PARENOPEN,
	PARENCLOSE,
	ASTERISK,
	SLASH,
	PERCENT,
	AMPERSAND,
	QUESTION,
	POUND,
	NOT,
	CARET,
	BITOR,
	BITAND,
	PLUS,
	TILDE,

	EQUALEQUAL,
	NEQUAL,
	PLUSEQUAL,
	MINUSEQUAL,
	TIMESEQUAL,
	DIVEQUAL,
	MODEQUAL,
	ANDEQUAL,
	OREQUAL,
	XOREQUAL,

	GTE,
	LTE,
	ANDAND,
	OROR,

	LSHIFT,
	RSHIFT,

	PLUSPLUS,
	MINUSMINUS,

	ARROW,

	WORD,
	NUMBER,

	CLASS,
	RETURN,
	NEW,
	SWITCH,
	IN,
	IFOR,
	FOR,
	WHILE,
	IF,
	ELSE,
	BREAK,
	CASE,
	DO,
	USING,
	DEFINE,
	STATIC,
	EXTENDS,

	PUBLIC,
	PRIVATE,
	PROTECTED,

	TOKENS_SIZE
};

using TOKENVALUE = std::pair<TOKENS, std::string>;

class Tokenizer {
private:
	std::string program;
	unsigned index = 0;
public:
	static std::string tokenLookup[TOKENS_SIZE];
	Tokenizer(std::string program) : program(program) {
		tokenLookup[END] = "END";
        tokenLookup[SEMICOLON] = ";";
        tokenLookup[COLON] = ":";
        tokenLookup[NS] = "::";
        tokenLookup[BACKSLASH] = "\\";
        tokenLookup[DOT] = ".";
        tokenLookup[COMMA] = ",";
        tokenLookup[EQUAL] = "=";
        tokenLookup[LT] = "<";
        tokenLookup[GT] = ">";
        tokenLookup[DASH] = "-";
        tokenLookup[SPACE] = "SPACE";
        tokenLookup[TAB] = "TAB";
        tokenLookup[NEWLINE] = "NEWLINE";
        tokenLookup[BRACEOPEN] = "{";
        tokenLookup[BRACECLOSE] = "}";
        tokenLookup[BRACKETOPEN] = "[";
        tokenLookup[BRACKETCLOSE] = "]";
        tokenLookup[PARENOPEN] = "(";
        tokenLookup[PARENCLOSE] = ")";
        tokenLookup[ASTERISK] = "*";
		tokenLookup[SLASH] = "/",
		tokenLookup[PERCENT] = "%",
        tokenLookup[AMPERSAND] = "&";
        tokenLookup[QUESTION] = "?";
        tokenLookup[POUND] = "#";
		tokenLookup[NOT] = "!";
		tokenLookup[CARET] = "^";
		tokenLookup[BITOR] = "|";
		tokenLookup[BITAND] = "&";
		tokenLookup[PLUS] = "+";

		tokenLookup[EQUALEQUAL] = "==";
		tokenLookup[NEQUAL] = "!=";
		tokenLookup[PLUSEQUAL] = "+=";
		tokenLookup[MINUSEQUAL] = "-=";
		tokenLookup[TIMESEQUAL] = "*=";
		tokenLookup[DIVEQUAL] = "/=";
		tokenLookup[MODEQUAL] = "%=";
		tokenLookup[ANDEQUAL] = "&=";
		tokenLookup[OREQUAL] = "|=";
		tokenLookup[XOREQUAL] = "^=";

		tokenLookup[LSHIFT] = "<<";
		tokenLookup[RSHIFT] = ">>";



		tokenLookup[GTE] = ">=";
		tokenLookup[LTE] = "<=";
		tokenLookup[ANDAND] = "&&";
		tokenLookup[OROR] = "||";

        tokenLookup[WORD] = "WORD";
        tokenLookup[NUMBER] = "NUMBER";

        tokenLookup[CLASS] = "CLASS";
        tokenLookup[RETURN] = "RETURN";
        tokenLookup[NEW] = "NEW";
        tokenLookup[SWITCH] = "SWITCH";
		tokenLookup[IN] = "IN";
		tokenLookup[IFOR] = "IFOR";
		tokenLookup[FOR] = "FOR";
        tokenLookup[WHILE] = "WHILE";
        tokenLookup[IF] = "IF";
		tokenLookup[ELSE] = "ELSE";
        tokenLookup[BREAK] = "BREAK";
        tokenLookup[CASE] = "CASE";
        tokenLookup[DO] = "DO";
        tokenLookup[USING] = "USING";
        tokenLookup[DEFINE] = "DEFINE";
        tokenLookup[STATIC] = "STATIC";

        tokenLookup[PUBLIC] = "PUBLIC";
        tokenLookup[PRIVATE] = "PRIVATE";
        tokenLookup[PROTECTED] = "PROTECTED";
		tokenLookup[EXTENDS] = "EXTENDS";
	}

	std::forward_list<TOKENVALUE> read() {
		std::forward_list<TOKENVALUE> out;
		int index = 0;
		TOKENVALUE t;
		do {
			t = readToken();
			out.push_front(t);
		} while (t.first != END);
		out.reverse();
		return out;
	}
private:
	bool isNum(char c) {
		return c >= '0' && c <= '9';
	}

	bool isStartOfWord(char c) {
		return c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z' || c == '_';
	}

	bool isPartWord(char c) {
		return isNum(c) || isStartOfWord(c);
	}

	std::string parseWord() {
		if (!isStartOfWord(program[index]))
			return "";
		std::string out;
		while (index != program.length()) {
			char c = program[index];
			if (isPartWord(c)) {
				out += c;
				++index;
			}
			else
				break;
		}
		return out;
	}

	std::string parseInt() {
		std::string out;
		while (index != program.length()) {
			char c = program[index];
			if (isNum(c)) {
				out += c;
				++index;
			}
			else
				break;
		}
		return out;
	}

	TOKENVALUE readToken() {
		if (index == program.length())
			return { END, "" };
		char c = program[index];
		switch (c) {
		case '\\':
			index += 1;
			if (index + 1 <= program.size() && program[index] == '\n') {
				index += 1;
				return readToken();
			}
			return { BACKSLASH, "\\\\" };
		case '{':
			index += 1;
			return { BRACEOPEN, "{" };
		case '}':
			index += 1;
			return { BRACECLOSE, "}" };
		case '(':
			index += 1;
			return { PARENOPEN, "(" };
		case ')':
			index += 1;
			return { PARENCLOSE, ")" };
		case '[':
			index += 1;
			return { BRACKETOPEN, "[" };
		case ']':
			index += 1;
			return { BRACKETCLOSE, "]" };
		case ' ':
			index += 1;
			if (index + 1 <= program.size() && program[index + 0] == ' ' &&
				index + 2 <= program.size() && program[index + 1] == ' ' &&
				index + 3 <= program.size() && program[index + 2] == ' ') {
				index += 3;
				return { TAB, "\t" };
			}
			return { SPACE, " " };
		case '\n':
			index += 1;
			return { NEWLINE, "\\n" };
		case '\t':
			index += 1;
			return { TAB, "\\t" };
		case '.':
			index += 1;
			return { DOT, "." };
		case ';':
			index += 1;
			return { SEMICOLON, ";" };
		case ':':
			index += 1;
			if (index + 1 <= program.size() && program[index] == ':') {
				index += 1;
				return { NS, "::" };
			}
			return { COLON, ":" };
		case ',':
			index += 1;
			return { COMMA, "," };
		case '=':
			index += 1;
			if (index + 1 <= program.size() && program[index] == '=') {
				index += 1;
				return { EQUALEQUAL, "==" };
			}
			return { EQUAL, "=" };
		case '!':
			index += 1;
			if (index + 1 <= program.size() && program[index] == '=') {
				index += 1;
				return { NEQUAL, "!=" };
			}
			return { NOT, "!" };
		case '<':
			index += 1;
			if (index + 1 <= program.size()) {
				if (program[index] == '=') {
					index += 1;
						return { LTE, "<=" };
				}
				else if (program[index] == '<') {
					index += 1;
					return { LSHIFT, "<<" };
				}
			}
			return { LT, "<" };
		case '>':
			index += 1;
			if (index + 1 <= program.size()) {
				if (program[index] == '=') {
					index += 1;
						return { GTE, ">=" };
				}
				else if (program[index] == '>') {
					index += 1;
					return { RSHIFT, ">>" };
				}
			}
			return { GT, ">" };
		case '-':
			index += 1;
			if (index + 1 <= program.size()) {
				if (program[index] == '=') {
					index += 1;
					return { MINUSEQUAL, "-=" };
				}
				else if (program[index] == '-') {
					index += 1;
					return { MINUSMINUS, "--" };
				}
				else if (program[index] == '>') {
					index += 1;
					return { ARROW, "->" };
				}
			}
			return { DASH, "-" };
		case '+':
			index += 1;
			if (index + 1 <= program.size()) {
				if (program[index] == '=') {
					index += 1;
						return { PLUSEQUAL, "+=" };
				}
				else if (program[index] == '+') {
					index += 1;
					return { PLUSPLUS, "++" };
				}
			}
			return { PLUS, "+" };
		case '*':
			index += 1;
			if (index + 1 <= program.size() && program[index] == '=') {
				index += 1;
				return { TIMESEQUAL, "*=" };
			}
			return { ASTERISK, "*" };
		case '/':
			index += 1;
			if (index + 1 <= program.size() && program[index] == '=') {
				index += 1;
				return { DIVEQUAL, "/=" };
			}
			return { SLASH, "/" };
		case '%':
			index += 1;
			if (index + 1 <= program.size() && program[index] == '=') {
				index += 1;
				return { MODEQUAL, "%=" };
			}
			return { PERCENT, "%" };
		case '&':
			index += 1;
			if (index + 1 <= program.size()) {
				if (program[index] == '&') {
					index += 1;
					return { ANDAND, "&&" };
				} else if (program[index] == '=') {
					index += 1;
					return { ANDEQUAL, "&=" };
				}
			}
			return { AMPERSAND, "&" };
		case '?':
			index += 1;
			return { QUESTION, "?" };
		case '#':
			index += 1;
			if (index + 6 <= program.size() && program.substr(index, index + 6) == "define") {
				index += 6;
				return { DEFINE, "#define" };
			}
			return { POUND, "#" };
		case '^':
			index += 1;
			if (index + 1 <= program.size() && program[index] == '^') {
				index += 1;
				return { XOREQUAL, "^=" };
			}
			return { CARET, "^" };
		case '|':
			index += 1;
			if (index + 1 <= program.size()) {
				if (program[index] == '|') {
					index += 1;
					return { OROR, "||" };
				}
				else if (program[index] == '=') {
					index += 1;
					return { OREQUAL, "|=" };
				}
			}
			return { BITOR, "|" };
		}

		std::string word = parseWord();
		if (word.length()) {
			if (word == "class")
				return { CLASS, word };
			if (word == "return")
				return { RETURN, word };
			if (word == "new")
				return { NEW, word };
			if (word == "switch")
				return { SWITCH, word };
			if (word == "in")
				return { IN, word };
			if (word == "ifor")
				return { IFOR, word };
			if (word == "for")
				return { FOR, word };
			if (word == "while")
				return { WHILE, word };
			if (word == "if")
				return { IF, word };
			if (word == "else")
				return { ELSE, word };
			if (word == "break")
				return { BREAK, word };
			if (word == "case")
				return { CASE, word };
			if (word == "do")
				return { DO, word };
			if (word == "using")
				return { USING, word };
			if (word == "static")
				return { STATIC, word };

			if (word == "public")
				return { PUBLIC, word };
			if (word == "private")
				return { PRIVATE, word };
			if (word == "protected")
				return { PROTECTED, word };
			if (word == "extends")
				return { EXTENDS, word };

			return { WORD, word };
		}

		std::string num = parseInt();
		if (!num.length())
			throw std::exception();
		if (index != program.length() && program[index] == '.') {
			index += 1;
			std::string part2 = parseInt();
			if (part2.length())
				return { NUMBER, num + "." + part2 };
		}
		return { NUMBER, num };
	}
};
