#include <vector>

template <typename T>
struct cursor_vector_view {
	const std::vector<T>& viewed_vec;
	size_t current_index = 0;

	size_t remaining() const {
		return viewed_vec.size() - current_index;
	}

	std::pair<T, cursor_vector_view> take_one() const {
		return { viewed_vec.at(current_index), { viewed_vec, current_index + 1} };
	}
};