#pragma once
#include <memory>

template <typename T>
struct Allocated {
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
	Allocated() = delete;

	// construct from T or U &
	Allocated(const T& t) : ptr(new T(t)) {}
	template <typename U>
	Allocated(const U& u) : ptr(new T(u)) {}

	// contrust from T or U &&
	Allocated(T&& t) : ptr(new T(std::move(t))) {}
	template <typename U>
	Allocated(U&& u) : ptr(new T(std::move(u))) {}

	// copy constructor
	Allocated(const Allocated& other) : Allocated(other.get()) {};
	
	// template copy constructor
	template <typename U>
	Allocated(const Allocated<U>& other) : Allocated(other.get()) {};

	// move constructor
	Allocated(Allocated&& other) : Allocated(std::move(other).get()) {};

	// template move constructor
	template <typename U>
	Allocated(Allocated<U>&& other) : Allocated(std::move(other).get()) {};

	// operator= same type&
	Allocated& operator=(const Allocated& other) {
		ptr = std::make_unique<T>(other.get());
		return *this;
	}

	// operator= template&
	template <typename U>
	Allocated& operator=(const Allocated<U>& other) {
		ptr = std::make_unique<T>(other.get());
		return *this;
	}

	// operator= same type&&
	Allocated& operator=(Allocated&& other) {
		ptr = std::make_unique<T>(std::move(other).get());
		return *this;
	}

	// operator= template&&
	template <typename U>
	Allocated& operator=(Allocated<U>&& other) {
		ptr = std::make_unique<T>(std::move(other).get());
		return *this;
	}

	~Allocated() = default;
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
