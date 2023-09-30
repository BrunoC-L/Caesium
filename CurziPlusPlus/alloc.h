#pragma once
#include <memory>

template <typename T>
struct Allocated {
public:
	const T& get() const& {
		return *ptr.get();
	}
	T& get() & {
		return *ptr.get();
	}
	T&& get() && {
		return std::move(*ptr.get());
	}

	operator T& () & {
		return *ptr.get();
	}

	operator const T& () const & {
		return *ptr.get();
	}

	operator const T () && {
		return std::move(* ptr.get());
	}

	Allocated()  = delete;

	template <typename U>
	Allocated(const U& u) : ptr(std::make_unique<T>(u)) {}

	template <typename U>
	Allocated(U&& u) : ptr(std::make_unique<T>(std::move(u))) {}

	template <typename U>
	Allocated(const Allocated<U>& other) : Allocated(other.get()) {};
	template <typename U>
	Allocated& operator=(const Allocated<U>& other) {
		ptr = std::make_unique<T>(*other.ptr);
	}

	template <typename U>
	Allocated(Allocated<U>&& other) : Allocated(other.get()) {};
	template <typename U>
	Allocated& operator=(Allocated<U>&& other) {
		ptr = std::move(other.ptr);
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
