#include "expected.hpp"
#include "vector"

template <typename T, template <typename> typename cnt>
expected<cnt<T>> vec_of_expected_to_expected_of_vec(const cnt<expected<T>>& vec) {
	cnt<T> res;
	for (const expected<T>& e : vec) {
		return_if_error(e);
		res.push_back(e.value());
	}
}
