#include "parse.hpp"

std::string file_name_stub() {
	throw;
}

void parse_whitespaces(Iterator& it) {
	while (it.vec[it.index].first == TAB || it.vec[it.index].first == SPACE)
		it.index++;
}

bool build_newline_token(Iterator& it) {
	auto beg = it.index;
	auto checkpoint = it.index;
	while (true) {
		parse_whitespaces(it);
		bool ok = it.vec[it.index].first == NEWLINE;
		if (ok) {
			it.index++;
			checkpoint = it.index;
		}
		else {
			it.index = checkpoint;
			break;
		}
	}
	return it.index != beg;
}

bool build(IndentToken& indent, Iterator& it) {
	indent.beg_offset = it.index;
	bool correct = true;
	for (int i = 0; i < indent.n_indent; ++i) {
		correct &= it.vec[it.index].first == TAB;
		if (it.vec[it.index].first == END)
			return false;
		it.index++;
	}
	correct &= it.vec[it.index].first != TAB && it.vec[it.index].first != NEWLINE && it.vec[it.index].first != SPACE;
	indent.end_offset = it.index;
	return correct;
}

bool parse_empty_line(Iterator& it) {
	auto newline = Token<NEWLINE>{ 0 };
	return build(newline, it);
}

bool parse_indented_line(TemplateBody& body, Iterator& it) {
	auto cp = it.index;
	for (int i = 0; i < body.n_indent + 1; ++i) {
		bool is_indented = it.vec[it.index].first != END && it.vec[it.index].first == TAB;
		if (!is_indented) {
			it.index = cp;
			return false;
		}
		it.index++;
	}
	it.index--;
	auto until_newline = Until<Token<NEWLINE>>{ body.n_indent };
	bool has_newline = build(until_newline, it);
	_ASSERT(has_newline); // if that doesnt work something is just wrong with the tokenizer.
	return true;
}

bool parse_one_line(TemplateBody& body, Iterator& it) {
	return parse_empty_line(it) || parse_indented_line(body, it);
}

bool build(TemplateBody& body, Iterator& it) {
	// look for first line that isnt just spaces/tabs/newline and doesnt begin with a tab, leave the iterator at the beginning of that line
	auto beg = it.index;
	body.beg_offset = it.index;
	while (parse_one_line(body, it));

	std::stringstream ss;
	while (beg != it.index) {
		ss << it.vec[beg].second;
		++beg;
	}

	body.value = std::move(ss).str();
	body.end_offset = it.index;
	return true;
}

// specialize build for typename or expression
bool build(grammar::TypenameOrExpression& x, Iterator& it) {
	auto it_typename = it;
	auto it_expr = it;
	auto tn = grammar::Typename{ x.n_indent };
	auto expr = grammar::Expression{ x.n_indent };
	bool b_typename = build(tn, it_typename);
	tn.beg_offset = it.index;
	tn.end_offset = it_typename.index;
	bool b_expr = build(expr, it_expr);
	expr.beg_offset = it.index;
	expr.end_offset = it_expr.index;
	if (!b_typename && !b_expr)
		return false;
	if (b_typename && !b_expr || it_typename.index >= it_expr.index) {
		x.beg_offset = tn.beg_offset;
		x.end_offset = tn.end_offset;
		x._value.emplace(std::move(tn));
		it.index = it_typename.index;
		return true;
	}
	if (b_expr && !b_typename || it_expr.index > it_typename.index) {
		x.beg_offset = expr.beg_offset;
		x.end_offset = expr.end_offset;
		x._value.emplace(std::move(expr));
		it.index = it_expr.index;
		return true;
	}
	throw;
}

bool build(grammar::CompareOperator& op, Iterator& it) {
	op.beg_offset = it.index;
	using CompareOperator = And<Or<Token<LT>, Token<LTE>, Token<GT>, Token<GTE>>, Token<QUESTION>>;
	bool b1 = build(std::get<Or<Token<LT>, Token<LTE>, Token<GT>, Token<GTE>>>(op.value), it);
	if (!b1)
		return false;
	auto prev = it.vec[it.index - 1].first;
	if (it.vec[it.index].first == QUESTION && (prev == LT || prev == LTE || prev == GT || prev == GTE)) {
		build(std::get<Token<QUESTION>>(op.value), it);
		op.end_offset = it.index;
		return true;
	}
	return false;
}

