#pragma once
#include <sstream>
#include <vector>
#include <forward_list>

enum TOKEN {
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
	AMPERSAND,
	QUESTION,
	POUND,

	WORD,
	NUMBER,

	CLASS,
	RETURN,
	NEW,
	SWITCH,
	FOR,
	WHILE,
	IF,
	BREAK,
	CASE,
	DO,
	USING,
	DEFINE,
	STATIC,

	PUBLIC,
	PRIVATE,
	PROTECTED,

	INT,
	FLOAT,
	DOUBLE,
	TOKENS_SIZE
};

using TOKENVALUE = std::pair<TOKEN, std::string>;

class Tokenizer {
private:
	std::string program;
	int index = 0;
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
        tokenLookup[AMPERSAND] = "&";
        tokenLookup[QUESTION] = "?";
        tokenLookup[POUND] = "#";

        tokenLookup[WORD] = "WORD";
        tokenLookup[NUMBER] = "NUMBER";

        tokenLookup[CLASS] = "CLASS";
        tokenLookup[RETURN] = "RETURN";
        tokenLookup[NEW] = "NEW";
        tokenLookup[SWITCH] = "SWITCH";
        tokenLookup[FOR] = "FOR";
        tokenLookup[WHILE] = "WHILE";
        tokenLookup[IF] = "IF";
        tokenLookup[BREAK] = "BREAK";
        tokenLookup[CASE] = "CASE";
        tokenLookup[DO] = "DO";
        tokenLookup[USING] = "USING";
        tokenLookup[DEFINE] = "DEFINE";
        tokenLookup[STATIC] = "STATIC";

        tokenLookup[PUBLIC] = "PUBLIC";
        tokenLookup[PRIVATE] = "PRIVATE";
        tokenLookup[PROTECTED] = "PROTECTED";

        tokenLookup[INT] = "INT";
        tokenLookup[FLOAT] = "FLOAT";
        tokenLookup[DOUBLE] = "DOUBLE";
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
			return { BACKSLASH, "\\" };
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
			return { SPACE, " " };
		case '\n':
			index += 1;
			return { NEWLINE, "\n" };
		case '\t':
			index += 1;
			return { TAB, "\t" };
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
			return { EQUAL, "=" };
		case '<':
			index += 1;
			return { LT, "<" };
		case '>':
			index += 1;
			return { GT, ">" };
		case '-':
			index += 1;
			return { DASH, "-" };
		case '*':
			index += 1;
			return { ASTERISK, "*" };
		case '&':
			index += 1;
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
			if (word == "for")
				return { FOR, word };
			if (word == "while")
				return { WHILE, word };
			if (word == "if")
				return { IF, word };
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

			if (word == "int")
				return { INT, word };
			if (word == "float")
				return { FLOAT, word };
			if (word == "double")
				return { DOUBLE, word };

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
