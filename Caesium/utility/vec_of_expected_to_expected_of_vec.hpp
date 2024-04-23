#include "expected.hpp"
#include <vector>

template <typename T>
expected<std::vector<T>> vec_of_expected_to_expected_of_vec(const std::vector<expected<T>>& vec) {
	std::vector<T> res;
	res.reserve(vec.size());
	for (const expected<T>& e : vec) {
		return_if_error(e);
		res.push_back(e.value());
	}
	return res;
}

template <typename T>
expected<std::vector<T>> vec_of_expected_to_expected_of_vec(std::vector<expected<T>>&& vec) {
	std::vector<T> res;
	res.reserve(vec.size());
	for (expected<T>& e : vec) {
		return_if_error(e);
		res.push_back(std::move(e).value());
	}
	return res;
}
