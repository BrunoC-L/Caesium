// make this a private member of a struct and keep rule of zero but disable copy construction
struct no_copy {
	no_copy() = default;
	no_copy(no_copy&&) = default;
	no_copy& operator=(no_copy&&) = default;

	no_copy(const no_copy&) = delete;
	no_copy& operator=(const no_copy&) = delete;
};
