#pragma once
#include <sstream>
#include <vector>
#include <forward_list>

enum TOKENS {
	END, // marks the last token of a stream

	SEMICOLON,
	COLON,
	NS,
	BACKSLASH,
	DOT,
	COMMA,
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

	EQUAL,
	EQUALQ,
	LT,
	LTQ,
	LTE,
	LTEQ,
	GT,
	GTQ,
	GTE,
	GTEQ,
	DASH,
	NOT,
	CARET,
	BITOR,
	PLUS,
	TILDE,

	PLUSPLUS,
	MINUSMINUS,
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

	AND,
	OR,
	ANDAND,
	OROR,

	WORD,
	FLOATING_POINT_NUMBER,
	INTEGER_NUMBER,
	STRING,

	IMPORT,
	FROM,
	USING,
	STATIC,

	RETURN,
	FOR,
	IFOR,
	IN,
	IF,
	ELSE,
	WHILE,
	DO,
	SWITCH,
	CASE,
	BREAK,

	TYPE,
	TEMPLATE,
	BLOCK,

	COPY,
	MOVE,
	REF,
	VAL,
	//KEY,

	AUTO,
	VARIANT,
	VIRTUAL,
	BOX,
};

using TOKENVALUE = std::pair<TOKENS, std::string>;

struct tokens_and_iterator {
	std::forward_list<TOKENVALUE>& tokens;
	std::forward_list<TOKENVALUE>::iterator it;
};

struct Tokenizer {
	std::string program;
	unsigned index = 0;

	std::forward_list<TOKENVALUE> read() {
		std::forward_list<TOKENVALUE> out;
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
			// backslash at the end of a line means ignore it and the newline
			if (index + 1 <= program.size() && program[index + 1] == '\n') {
				index += 1;
				// so we just return the next token
				return readToken();
			}
			// else assume the backslash makes sense in context
			return { BACKSLASH, "\\" };

		// string begin
		case '\'':
		case '`':
		case '"':
			while (true) {
				index += 1;
				if (index == program.length())
					throw std::runtime_error("Hit end of program before end of string token, parsed so far: " + str);
				if (program[index] == '\\' && index + 1 <= program.size() && program[index + 1] == c) {
					// backslash with open string character means dont close the string
					// but all our transpiled strings use " for strings so we just replace that
					index += 1;
					str += "\\\"";
				}
				else {
					// close string
					if (program[index] == c) {
						index += 1;
						// the transpiled string uses " always, not ' or `
						return { STRING, "\"" + str + "\"" };
					}
					// in case there are " in the string and the string uses ' or `, we need to replace them accordingly since
					// our transpiled c++ string uses "
					// so we just escape out of " not to end the string and the user will have his " in the string as expected
					else if (c != '"' && program[index] == '"')
						str += "\\\"";
					else
						str += program[index];
				}
			}
			std::unreachable();
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
			if (index + 3 < program.size() &&
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
			if (index < program.size() && program[index] == ':') {
				index += 1;
				return { NS, "::" };
			}
			return { COLON, ":" };
		case ',':
			index += 1;
			return { COMMA, "," };
		case '=':
			index += 1;
			if (index < program.size() && program[index] == '=') {
				index += 1;
				return { EQUALEQUAL, "==" };
			}
			return { EQUAL, "=" };
		case '!':
			index += 1;
			if (index < program.size() && program[index] == '=') {
				index += 1;
				return { NEQUAL, "!=" };
			}
			return { NOT, "!" };
		case '<':
			index += 1;
			if (index < program.size()) {
				if (program[index] == '=') {
					index += 1;
					if (index < program.size()) {
						if (program[index] == '?') {
							index += 1;
							return { LTEQ, "<=?" };
						}
					}
					return { LTE, "<=" };
				}
				if (program[index] == '?') {
					index += 1;
					return { LTQ, "<?" };
				}
			}
			return { LT, "<" };
		case '>':
			index += 1;
			if (index < program.size()) {
				if (program[index] == '=') {
					index += 1;
					if (index < program.size()) {
						if (program[index] == '?') {
							index += 1;
							return { GTEQ, ">=?" };
						}
					}
					return { GTE, ">=" };
				}
				if (program[index] == '?') {
					index += 1;
					return { GTQ, ">?" };
				}
			}
			return { GT, ">" };
		case '-':
			index += 1;
			if (index < program.size()) {
				if (program[index] == '=') {
					index += 1;
					return { MINUSEQUAL, "-=" };
				}
				else if (program[index] == '-') {
					index += 1;
					return { MINUSMINUS, "--" };
				}
			}
			return { DASH, "-" };
		case '+':
			index += 1;
			if (index < program.size()) {
				if (program[index] == '=') {
					index += 1;
					return { PLUSEQUAL, "+=" };
				} else if (program[index] == '+') {
					index += 1;
					return { PLUSPLUS, "++" };
				}
			}
			return { PLUS, "+" };
		case '*':
			index += 1;
			if (index < program.size() && program[index] == '=') {
				index += 1;
				return { TIMESEQUAL, "*=" };
			}
			return { ASTERISK, "*" };
		case '/':
			index += 1;
			// skip // comments
			if (index < program.size() && program[index] == '/') {
				index += 1;
				while (index < program.size()) {
					if (program[index] == '\n') {
						index += 1;
						break;
					}
					else
						index += 1;
				}
				return readToken();
			}
			// skip /* */ comments
			if (index < program.size() && program[index] == '*') {
				index += 1;
				while (index + 1 < program.size())
					if (program[index] == '*' && program[index + 1] == '/') {
						index += 2;
						break;
					}
					else
						index += 1;
				return readToken();
			}
			if (index < program.size() && program[index] == '=') {
				index += 1;
				return { DIVEQUAL, "/=" };
			}
			return { SLASH, "/" };
		case '%':
			index += 1;
			if (index < program.size() && program[index] == '=') {
				index += 1;
				return { MODEQUAL, "%=" };
			}
			return { PERCENT, "%" };
		case '&':
			index += 1;
			if (index < program.size()) {
				if (program[index] == '&') {
					index += 1;
					return { ANDAND, "&&" };
				} else if (program[index] == '=') {
					index += 1;
					return { ANDEQUAL, "&=" };
				}
			}
			//return { AMPERSAND, "&" };
			throw std::runtime_error("Unknown Symbol `&`");
		case '?':
			index += 1;
			return { QUESTION, "?" };
		case '#':
			index += 1;
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
			if (word == "auto")
				return { AUTO, word };
			if (word == "break")
				return { BREAK, word };
			if (word == "block")
				return { BLOCK, word };
			if (word == "case")
				return { CASE, word };
			if (word == "do")
				return { DO, word };
			if (word == "else")
				return { ELSE, word };
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
			if (word == "or")
				return { OR, word };
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
			if (word == "type")
				return { TYPE, word };
			if (word == "template")
				return { TEMPLATE, word };
			if (word == "copy")
				return { COPY, word };
			if (word == "move")
				return { MOVE, word };
			if (word == "ref")
				return { REF, word };
			if (word == "val")
				return { VAL, word };
			/*if (word == "key")
				return { KEY, word };*/

			return { WORD, word };
		}

		std::string num = parseInt();
		if (!num.length())
			throw std::exception();
		if (index != program.length() && program[index] == '.') {
			index += 1;
			std::string part2 = parseInt();
			if (part2.length())
				return { FLOATING_POINT_NUMBER, num + "." + part2 };
		}
		return { INTEGER_NUMBER, num };
	}
};
