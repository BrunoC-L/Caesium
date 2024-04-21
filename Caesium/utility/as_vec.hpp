#pragma once
#include <vector>

void push(auto&& v) {}

void push(auto&& v, auto e, auto&&... es) {
	v.push_back(std::move(e));
	push(v, std::forward<decltype(es)>(es)...);
}

template <typename T>
std::vector<T> as_vec(T e, auto&&... es) {
	std::vector<T> res;
	res.push_back(std::move(e));
	push(res, std::forward<decltype(es)>(es)...);
	return res;
}
