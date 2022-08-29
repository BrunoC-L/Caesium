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

	PLUSPLUS,
	MINUSMINUS,

	ARROW,

	WORD,
	NUMBER,
	STRING,

	CLASS,
	RETURN,
	NEW,
	SWITCH,
	IN,
	IFOR,
	IMPORT,
	FROM,
	FOR,
	WHILE,
	IF,
	ELSE,
	BREAK,
	CASE,
	DO,
	USING,
	STATIC,
	EXTENDS,

	PUBLIC,
	PRIVATE,
	PROTECTED,

	AND,
	OR,
	NULL_TOKEN,
};

using TOKENVALUE = std::pair<TOKENS, std::string>;

class Tokenizer {
private:
	std::string program;
	unsigned index = 0;
public:
	Tokenizer(std::string program) : program(program) {}

	std::forward_list<TOKENVALUE> read() {
		std::forward_list<TOKENVALUE> out;
		int index = 0;
		TOKENVALUE t;
		do {
			t = readToken();
			out.push_front(t);
		} while (t.first != END);
		// allow for files not to end with a new line
		if (std::next(out.begin(), 1)->first != NEWLINE) {
			out.front().first = NEWLINE; // replace END with newline
			out.push_front(TOKENVALUE(END, "")); // add the new END
		}
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

	TOKENS peek() {
		unsigned peekFrom = index;
		while (true) {
			TOKENVALUE v = readToken();
			if (v.first == SPACE || v.first == TAB || v.first == NEWLINE)
				continue;
			index = peekFrom;
			return v.first;
		}
	}

	TOKENS peek(int n) {
		unsigned peekFrom = index;
		TOKENS v = peek();
		for (int i = 0; i < n; ++i)
			v = peek();
		index = peekFrom;
		return v;
	}

	TOKENVALUE readToken() {
		if (index == program.length())
			return { END, "" };
		char c = program[index];
		std::string str = "";
		switch (c) {
		case '\\':
			index += 1;
			if (index + 1 <= program.size() && program[index] == '\n') {
				index += 1;
				return readToken();
			}
			// TODO other \ cases
			return { BACKSLASH, "\\\\" };
		case '\'':
		case '`':
		case '"':
			while (true) {
				index += 1;
				if (index == program.length())
					throw std::exception();
				if (program[index] == c && !(program[index - 1] == '\\')) {
					index += 1;
					return { STRING, str };
				}
				str += program[index];
			}
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
			if (index + 3 <= program.size() &&
				program[index + 1] == ' ' &&
				program[index + 2] == ' ' &&
				program[index + 3] == ' ') {
				index += 4;
				return { TAB, "\t" };
			}
			index += 1;
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
			}
			return { LT, "<" };
		case '>':
			index += 1;
			if (index + 1 <= program.size() && program[index] == '=') {
				index += 1;
				return { GTE, ">=" };
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
			//return { AMPERSAND, "&" };
			throw std::exception();
		case '?':
			index += 1;
			return { QUESTION, "?" };
		case '#':
			index += 1;
			/*if (index + 6 <= program.size() && program.substr(index, index + 6) == "define") {
				index += 6;
				return { DEFINE, "#define" };
			}*/
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
			if (word == "and")
				return { AND, word };
			if (word == "break")
				return { BREAK, word };
			if (word == "case")
				return { CASE, word };
			if (word == "class")
				return { CLASS, word };
			if (word == "do")
				return { DO, word };
			if (word == "else")
				return { ELSE, word };
			if (word == "extends")
				return { EXTENDS, word };
			if (word == "for")
				return { FOR, word };
			if (word == "if")
				return { IF, word };
			if (word == "ifor")
				return { IFOR, word };
			if (word == "import")
				return { IMPORT, word };
			if (word == "from")
				return { FROM, word };
			if (word == "in")
				return { IN, word };
			if (word == "new")
				return { NEW, word };
			if (word == "null")
				return { NULL_TOKEN, word };
			if (word == "or")
				return { OR, word };
			if (word == "public")
				return { PUBLIC, word };
			if (word == "private")
				return { PRIVATE, word };
			if (word == "protected")
				return { PROTECTED, word };
			if (word == "return")
				return { RETURN, word };
			if (word == "static")
				return { STATIC, word };
			if (word == "switch")
				return { SWITCH, word };
			if (word == "using")
				return { USING, word };
			if (word == "while")
				return { WHILE, word };

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
