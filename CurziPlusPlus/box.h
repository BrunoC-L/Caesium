#pragma once
#include <memory>

template <typename T>
struct Box {
public:
	const T& get() const& {
		return *ptr;
	}
	T& get() & {
		return *ptr;
	}
	T&& get() && {
		return std::move(*ptr);
	}

	operator T& () & {
		return *ptr;
	}

	operator const T& () const & {
		return *ptr;
	}

	operator const T () && {
		return std::move(*ptr);
	}

	// ptr never empty
	Box() = delete;

	// construct from T or U &
	Box(const T& t) : ptr(std::make_unique<T>(t)) {}
	template <typename U>
	Box(const U& u) : ptr(std::make_unique<T>(u)) {}

	// contrust from T or U &&
	Box(T&& t) : ptr(std::make_unique<T>(std::move(t))) {}
	template <typename U>
	Box(U&& u) : ptr(std::make_unique<T>(std::move(u))) {}

	// copy constructor
	Box(const Box& other) : Box(other.get()) {};
	
	// template copy constructor
	template <typename U>
	Box(const Box<U>& other) : Box(other.get()) {};

	// move constructor
	Box(Box&& other) : Box(std::move(other).get()) {};

	// template move constructor
	template <typename U>
	Box(Box<U>&& other) : Box(std::move(other).get()) {};

	// operator= same type&
	Box& operator=(const Box& other) noexcept {
		ptr = std::make_unique<T>(other.get());
		return *this;
	}

	// operator= template&
	template <typename U>
	Box& operator=(const Box<U>& other) noexcept {
		ptr = std::make_unique<T>(other.get());
		return *this;
	}

	// operator= same type&&
	Box& operator=(Box&& other) noexcept {
		ptr = std::make_unique<T>(std::move(other).get());
		return *this;
	}

	// operator= template&&
	template <typename U>
	Box& operator=(Box<U>&& other) noexcept {
		ptr = std::make_unique<T>(std::move(other).get());
		return *this;
	}

	auto operator<=>(const Box& other) const {
		return (*ptr) <=> (*other.ptr);
	};

	~Box() = default;
protected:
	std::unique_ptr<T> ptr;
};

template <typename T>
struct Unique {
public:
	const T& get() const& {
		return *t;
	}
	T& get() & {
		return *t;
	}
	T get() && {
		return std::move(t);
	}

	template <typename U>
	Unique(const U& u) : t(u) {}
	template <typename U>
	Unique(U&& u) : t(std::move(u)) {}
	Unique(const Unique& other) = delete;
	Unique(Unique&& other) : Unique(other.get()) {};
	~Unique() = default;
protected:
	T t;
};
