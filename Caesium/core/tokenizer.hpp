#pragma once
#include <sstream>
#include <vector>

enum TOKENS {
	END, // marks the last token of a stream

	SEMICOLON,
	COLON,
	NS,
	BACKSLASH,
	BACKTICK,
	DOT,
	DOTS,
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
	LTE,
	GT,
	GTE,
	DASH,
	NOT,
	CARET,
	BITOR,
	PLUS,
	TILDE,

	EQUALEQUAL,
	NEQUAL,

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
	EXISTS,

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
	MATCH,

	ENUM,
	TYPE,
	TEMPLATE,
	CLASS,
	INTERFACE,
	BLOCK,

	MOVE,
	REF,
	VAL,
	//KEY,

	AUTO,
	VARIANT,
	VIRTUAL,
	BOX,
	NONE,
};

using TokenValue = std::pair<TOKENS, std::string>;

struct Iterator {
	std::vector<TokenValue>& vec;
	unsigned index;
};

struct Tokenizer {
	std::string program;
	unsigned index = 0;

	std::vector<TokenValue> read() {
		std::vector<TokenValue> out;
		out.reserve(program.size());
		if (program.size() != 0) {
			TokenValue t;
			do {
				t = readToken();
				out.push_back(t);
			} while (t.first != END);

			// this allows for files not to end with a new line, we add it manually so we can later act as if all files end with a new line
			if (out.size() > 1 && out.at(out.size() - 2).first != NEWLINE) {
				out.back().first = NEWLINE; // replace END with newline
				out.push_back(TokenValue(END, "")); // add the new END
			}
		}
		else {
			out.push_back(TokenValue(END, ""));
		}
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
			TokenValue v = readToken();
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

	TokenValue string_case(auto c, auto& index) {
		std::string str = "";
		while (true) {
			index += 1;
			if (index == program.length())
				throw std::runtime_error("Reached end of file before end of string, parsed so far: " + str);
			if (program[index] == '\\') {
				if (index + 1 <= program.size() && program[index + 1] == c) {
					index += 1;
					str += '\\';
					str += c;
					continue;
				}
				else if(index + 1 <= program.size() && program[index + 1] == '\\') {
					// double backslash stays as a double backslash
					index += 1;
					str += "\\\\";
					continue;
				}
			}
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
		std::unreachable();
	}

	bool space_is_comment(auto& index) {
		auto cp = index;
		while (cp < program.size()) {
			auto c = program[cp];
			if (c == ' ' || c == '\t') {
				++cp;
				continue;
			}
			if (c == '/' && cp + 1 < program.size() && program[cp + 1] == '/') {
				cp += 2;
				skip_comment(cp);
				index = cp;
				return true;
			}
			return false;
		}
		return false;
	}

	void skip_comment(auto& index) {
		while (index < program.size())
			if (program[index] == '\n') {
				index += 1;
				break;
			}
			else
				index += 1;
	}

	TokenValue readToken() {
		while (true) {
			if (index == program.length())
				return { END, "" };
			char c = program[index];
			switch (c) {
			case '\\':
				// backslash at the end of a line means ignore it and the newline
				if (index + 1 < program.size() && program[index + 1] == '\n') {
					index += 2;
					// so we just return the next token
					continue;
				}
				// else assume the backslash makes sense in context
				index += 1;
				return { BACKSLASH, "\\" };

				// string begin
			case '`':
				index += 1;
				return { BACKTICK, "`" };
			case '\'':
			case '"':
				return string_case(c, index);
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
				if (space_is_comment(index))
					return { NEWLINE, "\n" };
				if (index + 3 < program.size() &&
					program[index + 1] == ' ' &&
					program[index + 2] == ' ' &&
					program[index + 3] == ' ') {
					index += 4;
					return { TAB, "\t" };
				}
				index += 1;
				return { SPACE, " " };
			case '\t':
				if (space_is_comment(index))
					return { NEWLINE, "\n" };
				index += 1;
				return { TAB, "\t" };
			case '\n':
				index += 1;
				return { NEWLINE, "\n" };
			case '.':
				if (index + 2 < program.size() &&
					program[index + 1] == '.' &&
					program[index + 2] == '.') {
					index += 3;
					return { DOTS, "..." };
				}
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
						return { LTE, "<=" };
					}
				}
				return { LT, "<" };
			case '>':
				index += 1;
				if (index < program.size()) {
					if (program[index] == '=') {
						index += 1;
						return { GTE, ">=" };
					}
				}
				return { GT, ">" };
			case '-':
				index += 1;
				/*if (index < program.size()) {
					if (program[index] == '=') {
						index += 1;
						return { MINUSEQUAL, "-=" };
					}
					else if (program[index] == '-') {
						index += 1;
						return { MINUSMINUS, "--" };
					}
				}*/
				return { DASH, "-" };
			case '+':
				index += 1;
				/*if (index < program.size()) {
					if (program[index] == '=') {
						index += 1;
						return { PLUSEQUAL, "+=" };
					}
					else if (program[index] == '+') {
						index += 1;
						return { PLUSPLUS, "++" };
					}
				}*/
				return { PLUS, "+" };
			case '*':
				index += 1;
				/*if (index < program.size() && program[index] == '=') {
					index += 1;
					return { TIMESEQUAL, "*=" };
				}*/
				return { ASTERISK, "*" };
			case '/':
				index += 1;
				// skip // comments
				if (index < program.size() && program[index] == '/') {
					index += 1;
					skip_comment(index);
					return { NEWLINE, "\n" };
				}
				// skip /* */ comments
				if (index < program.size() && program[index] == '*') {
					index += 1;
					if (index == program.size())
						throw; // no end of comment
					while (index < program.size()) {
						if (index + 1 == program.size())
							throw; // no end of comment
						if (program[index] == '*' && program[index + 1] == '/') {
							index += 2;
							break;
						}
						else
							index += 1;
					}
					continue;
				}
				/*if (index < program.size() && program[index] == '=') {
					index += 1;
					return { DIVEQUAL, "/=" };
				}*/
				return { SLASH, "/" };
			case '%':
				index += 1;
				/*if (index < program.size() && program[index] == '=') {
					index += 1;
					return { MODEQUAL, "%=" };
				}*/
				return { PERCENT, "%" };
			case '&':
				index += 1;
				if (index < program.size()) {
					if (program[index] == '&') {
						index += 1;
						return { ANDAND, "&&" };
					}
					/*else if (program[index] == '=') {
						index += 1;
						return { ANDEQUAL, "&=" };
					}*/
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
				/*if (index + 1 <= program.size() && program[index] == '^') {
					index += 1;
					return { XOREQUAL, "^=" };
				}*/
				return { CARET, "^" };
			case '|':
				index += 1;
				if (index + 1 <= program.size()) {
					if (program[index] == '|') {
						index += 1;
						return { OROR, "||" };
					}
					/*else if (program[index] == '=') {
						index += 1;
						return { OREQUAL, "|=" };
					}*/
				}
				return { BITOR, "|" };
			}

			std::string word = parseWord();
			if (word.length()) {
#define return_token(s, v) if (word == #s) return { v, word }
				return_token(and, AND);
				return_token(auto, AUTO);
				return_token(break, BREAK);
				return_token(block, BLOCK);
				return_token(case, CASE);
				return_token(class, CLASS);
				return_token(do, DO);
				return_token(else, ELSE);
				return_token(enum, ENUM);
				return_token(exists, EXISTS);
				return_token(for, FOR);
				return_token(if, IF);
				return_token(interface, INTERFACE);
				return_token(ifor, IFOR);
				return_token(import, IMPORT);
				return_token(from, FROM);
				return_token(in, IN);
				return_token(match, MATCH);
				return_token(or , OR);
				return_token(return, RETURN);
				return_token(static, STATIC);
				return_token(switch, SWITCH);
				return_token(using, USING);
				return_token(while, WHILE);
				return_token(type, TYPE);
				return_token(template, TEMPLATE);
				return_token(move, MOVE);
				return_token(ref, REF);
				return_token(val, VAL);
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
	}
};
