#pragma once
#include <memory>
#include "is_specialization.hpp"

template <typename T>
struct Box {
	static_assert(!is_specialization<T, Box>::value, "Box<Box<T>> may not behave as expected, place your box type inside an other type to box it.");
public:
	const T& get() const& {
		return *ptr;
	}

	T& get()& {
		return *ptr;
	}

	T&& get()&& {
		return std::move(*ptr);
	}

	operator T& ()& {
		return *ptr;
	}

	operator const T& () const& {
		return *ptr;
	}

	operator const T()&& {
		return std::move(*ptr);
	}

	// ptr never empty
	Box() = delete;

	Box& operator=(const Box&) = delete;
	Box& operator=(Box&&) = default;

	// construct from T& or U&
	Box(const T& t) : ptr(std::make_unique<T>(t)) {}
	template <typename U>
	Box(const U& u) : ptr(std::make_unique<T>(u)) {}

	// construct from T&& or U&&
	Box(T&& t) : ptr(std::make_unique<T>(std::move(t))) {}
	template <typename U>
	Box(U&& u) : ptr(std::make_unique<T>(std::forward<U>(u))) {}

	// copy constructor
	Box(const Box& other) : Box(other.get()) {};

	// template copy constructor
	template <typename U>
	Box(const Box<U>& other) : Box(other.get()) {};

	// move constructor
	Box(Box&& other) : ptr(std::move(other).ptr) {};

	// template move constructor
	template <typename U>
	Box(Box<U>&& other) : Box(std::move(other).get()) {};

	std::weak_ordering operator<=>(const Box& other) const noexcept {
		return (*ptr) <=> (*other.ptr);
	};

	bool operator==(const Box& other) const noexcept {
		return (*ptr) == (*other.ptr);
	};

	~Box() = default;
protected:
	std::unique_ptr<T> ptr;
};

template <typename T>
struct NonCopyableBox {
	static_assert(!is_specialization<T, NonCopyableBox>::value, "Box<Box<T>> may not behave as expected, place your box type inside an other type to box it.");
public:
	const T& get() const& {
		return *ptr;
	}

	T& get()& {
		return *ptr;
	}

	T&& get()&& {
		return std::move(*ptr);
	}

	operator T& ()& {
		return *ptr;
	}

	operator const T& () const& {
		return *ptr;
	}

	operator const T()&& {
		return std::move(*ptr);
	}

	// ptr never empty
	NonCopyableBox() = delete;

	NonCopyableBox& operator=(const NonCopyableBox&) = delete;
	NonCopyableBox& operator=(NonCopyableBox&&) = delete;

	// construct from T&& or U&&
	NonCopyableBox(T&& t) : ptr(std::make_unique<T>(std::move(t))) {}
	template <typename U>
	NonCopyableBox(U&& u) : ptr(std::make_unique<T>(std::forward<U>(u))) {}

	// move constructor
	NonCopyableBox(NonCopyableBox&& other) : ptr(std::move(other).ptr) {};

	// template move constructor
	template <typename U>
	NonCopyableBox(NonCopyableBox<U>&& other) : NonCopyableBox(std::move(other).get()) {};

	std::weak_ordering operator<=>(const NonCopyableBox& other) const noexcept {
		return (*ptr) <=> (*other.ptr);
	};

	bool operator==(const NonCopyableBox& other) const noexcept {
		return (*ptr) == (*other.ptr);
	};

	~NonCopyableBox() = default;
protected:
	std::unique_ptr<T> ptr;
};
