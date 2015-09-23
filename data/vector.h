#pragma once
#include <cstdlib>

namespace sal {

// a vector that's supposed to be reused via clear or resize
// shrinking to a smaller size_ is guaranteed to be efficent (no allocation)
// compared to std vector, 1.3 times faster push_back and emplace_back
// and same read, write, initialization, and iteration speeds
// MUCH less secure than std vector though, use in multithreaded environments at your own risk
template <typename T>
class Persistent_vector {
	T* elems;
	size_t size_;	// usable size
	size_t capacity_;	// actual size

	void grow_if_full() {
		if (size_ == capacity_) {
			if (!capacity_) capacity_ = 4;
			else capacity_ *= 2;
			elems = static_cast<T*>(realloc(elems, capacity_ * sizeof(T)));
		}
	}

	void destroy() {
		if (elems) free(elems);
	}
public:
	using value_type = T;
	using iterator = T*;
	using const_iterator = const T*;
	// core functions
	Persistent_vector() : elems{nullptr}, size_{0}, capacity_{0} {}
	Persistent_vector(size_t s) : elems{static_cast<T*>(malloc(s * sizeof(T)))}, size_{0}, capacity_{s} {}
	Persistent_vector(const Persistent_vector& pv) : elems{static_cast<T*>(malloc(pv.size_ * sizeof(T)))}, size_{pv.size_}, capacity_{pv.capacity_} {
		memcpy(elems, pv.elems, size_ * sizeof(T));
	}
	Persistent_vector(Persistent_vector&& pv) : elems{pv.elems}, size_{pv.size_}, capacity_{pv.capacity_} {pv.elems = nullptr; pv.size_ = 0;}
	Persistent_vector& operator=(const Persistent_vector& pv) {	// not even basic gurantee...
		if (pv.elems != elems) {
			destroy();
			size_ = pv.size_;
			capacity_ = pv.capacity_;
			elems = static_cast<T*>(malloc(size_ * sizeof(T)));
			memcpy(elems, pv.elems, size_ * sizeof(T));
		}
	}
	Persistent_vector& operator=(Persistent_vector&& pv) {
		if (pv.elems != elems) {
			destroy();
			elems = pv.elems;
			size_ = pv.size_;
			capacity_ = pv.capacity_;
			pv.elems = nullptr;
			pv.size_ = 0;
		}
	}
	~Persistent_vector() {destroy();}
	// modifiers, no checking for out of bounds
	void push_back(const T& e) {
		grow_if_full();
		elems[size_] = e;
		++size_;
	}
	template <typename...Args>
	void emplace_back(Args&&... args) {
		grow_if_full();
		elems[size_] = T{std::forward<Args>(args)...};
		++size_;
	}

	// query and retrieval
	bool empty() const {return size_ == 0;}
	size_t size() const {return size_;}
	size_t capacity() const {return capacity_;}
	const T& operator[](size_t i) const {return elems[i];}
	T& operator[](size_t i) {return elems[i];}

	// iteration
	iterator begin() {return elems;}
	iterator end() {return elems + size_;}	// the most important thing about this class
	const_iterator begin() const {return elems;}
	const_iterator end() const {return elems + size_;}

	// resizing and clearing, don't actually remove elements, just make them inaccessible in iteration
	// calling realloc with a smaller size will free the tail, so only do it if it grows tail
	void reserve(size_t s) {
		if (s > capacity_) {elems = realloc(elems, s * sizeof(T)); capacity_ = s;}
	}
	void resize(size_t s) {	// no testing for safety; living on the egde...
		reserve(s);
		size_ = s;
	}
	void resize(size_t s, const T& def) {	// fill space with default value padding
		reserve(s);
		while (size_ < s) {
			elems[size_] = def;
			++size_;
		}
	}	
	void clear() {size_ = 0;}	// does not actually remove elements

};

// a persistent vector that does not grow when used with push_back, just a simple wrapper
// 1.8 times faster than std vector at initialization, 2 times faster at emplace back, and 1.3 times faster at iteration
// same speed for other operations
// again, not very secure! (barely better than a raw array), but use when the situation is very regular
template <typename T>
class Fixed_vector {
	T* elems;
	size_t size_;	// actual size_

	void destroy() {
		if (elems) free(elems);
	}
public:
	using value_type = T;
	using iterator = T*;
	using const_iterator = const T*;
	// core functions
	Fixed_vector() : elems{nullptr}, size_{0} {}
	Fixed_vector(size_t s) : elems{static_cast<T*>(malloc(s * sizeof(T)))}, size_{0} {}
	Fixed_vector(const Fixed_vector& pv) : elems{static_cast<T*>(malloc(pv.size_ * sizeof(T)))}, size_{pv.size_} {
		memcpy(elems, pv.elems, size_ * sizeof(T));
	}
	Fixed_vector(Fixed_vector&& pv) : elems{pv.elems}, size_{pv.size_} {pv.elems = nullptr; pv.size_ = 0;}
	Fixed_vector& operator=(const Fixed_vector& pv) {	// not even basic gurantee...
		if (pv.elems != elems) {
			destroy();
			size_ = pv.size_;
			elems = static_cast<T*>(malloc(size_ * sizeof(T)));
			memcpy(elems, pv.elems, size_ * sizeof(T));
		}
	}
	Fixed_vector& operator=(Fixed_vector&& pv) {
		if (pv.elems != elems) {
			destroy();
			elems = pv.elems;
			size_ = pv.size_;
			pv.elems = nullptr;
			pv.size_ = 0;
		}
	}
	~Fixed_vector() {destroy();}
	// modifiers, no checking for out of bounds
	void push_back(const T& e) {
		elems[size_] = e;
		++size_;
	}
	template <typename...Args>
	void emplace_back(Args&&... args) {
		elems[size_] = T{std::forward<Args>(args)...};
		++size_;
	}

	// query and retrieval
	bool empty() const {return size_ == 0;}
	size_t size() const {return size_;}
	const T& operator[](size_t i) const {return elems[i];}
	T& operator[](size_t i) {return elems[i];}

	// iteration
	iterator begin() {return elems;}
	iterator end() {return elems + size_;}	// the most important thing about this class
	const_iterator begin() const {return elems;}
	const_iterator end() const {return elems + size_;}

	// resizing and clearing, don't actually remove elements, just make them inaccessible in iteration
	void reserve(size_t s) {
		if (s > size_) elems = static_cast<T*>(realloc(elems, s * sizeof(T)));
	}
	void resize(size_t s) {	// no testing for safety; living on the egde...
		reserve(s);
		size_ = s;
	}
	void resize(size_t s, const T& def) {	// fill space with default value padding
		reserve(s);
		while (size_ < s) {
			elems[size_] = def;
			++size_;
		}
	}
	void clear() {size_ = 0;}	// does not actually remove elements

};
}	// end namespace sal