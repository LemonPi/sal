#pragma once
#include <algorithm>
#include <initializer_list>
#include <unordered_map>
#include <vector>
#define SENTINEL(T) (T{})

namespace sal {

// differentiate the implementation based on whether data is simple
template <typename T, typename Cmp = std::greater<T>, bool ispod = false>
struct Heap_impl {};

// simple, plain old data implementation can work with copies of it
// swapping should be cheap and there shouldn't be ownership issues
// by default maxheap where parent greater than children
template <typename T, typename Cmp>
class Heap_impl<T, Cmp, true> {	
	// default representation is as a vector, index at 1 so need 1 extra element
	std::vector<T> elems;
	// associate each key with an index
	std::unordered_map<T,size_t> index;
	// comparator for sorting in a maxheap
	Cmp cmp;
	size_t parent(size_t i) {return i >> 1;}
	size_t left(size_t i) {return i << 1;}
	size_t right(size_t i) {return (i << 1) + 1;}
	// runs in O(n) time, produces max heap from unordered input array
	void build_heap() {
		// second half of elems are leaves, 1 elem is maxheap by default
		for (size_t i = elems.size() >> 1; i != 0; --i) 
			heapify(i);
	}
	// adjusts elems[i] assuming it's been modified to be smaller than its children
	// runs in O(lgn) time, floats elems[i] down
	void heapify(size_t i) {
		while (true) {
			size_t l = left(i), r = right(i);
			size_t largest = i;	
			// largest of elems[i], elems[l], and elems[r]
			if (l < elems.size() && cmp(elems[l], elems[i]))
				largest = l;
			if (r < elems.size() && cmp(elems[r], elems[largest]))
				largest = r;
			// do until elems[i] is max heap
			if (largest == i) break;
			std::swap(elems[i], elems[largest]);
			std::swap(index[elems[i]], index[elems[largest]]);
			i = largest;
		}
	}
public:
	using value_type = T;
	using iterator = typename std::vector<T>::iterator;
	using const_iterator = typename std::vector<T>::const_iterator;
	// default T value as sentinel

	// construction ------------
	// O(n) time construction via these constructors (would be O(nlgn) for repeated inserts)
	Heap_impl(Cmp& c) : elems{T{}}, cmp(c) {}
	Heap_impl(std::initializer_list<T> l, Cmp&& c = Cmp{}) : cmp(c) {
		elems.reserve(l.size()+1);
		elems.push_back(SENTINEL(T));
		for (const T& v : l) {index[v] = elems.size(); elems.push_back(v);}
		build_heap();
	}
	template <typename Iter>
	Heap_impl(Iter begin, Iter end, Cmp&& c = Cmp{}) : cmp(c) {
		elems.reserve(end - begin + 1);
		elems.push_back(SENTINEL(T));
		while (begin != end) {index[*begin] = elems.size(); elems.push_back(*begin); ++begin;}
		build_heap();
	}

	// query ---------------
	bool empty() const 	{return elems.size() <= 1;}
	size_t size() const {return elems.size() - 1;}
	T top() const 		{return elems[1];}
	// index of key, 0 means key not found
	size_t key(T k) const {
		auto itr = index.find(k);
		return (itr == index.end())? 0 : itr->second;
	}

	// extraction ----------
	// runs in O(lgn) time due to heapify
	T extract_top() {
		// heap underflow
		if (elems.size() <= 1) return SENTINEL(T); 

		T top {elems[1]};
		std::swap(elems[1],elems.back());
		index[elems[1]] = 1;

		index.erase(elems.back());
		elems.pop_back();
		heapify(1);
		return top;
	}

	// modification ----------
	void insert(T key) {
		index[key] = elems.size();
		elems.push_back(key);
		check_key(elems.size()-1);
	}
	// O(n) like constructor for all elements
	template <typename Iter>
	void batch_insert(Iter begin, Iter end) {
		while (begin != end) {index[*begin] = elems.size(); elems.push_back(*begin); ++begin;}
		build_heap();
	}
	// repeadtedly compare against parent
	void change_key(size_t i, T key) {
		// change should only float up (so only increase on maxheap and decrease on minheap)
		if (cmp(elems[i], key)) return;
		elems[i] = key;
		while (i > 1 && cmp(elems[i], elems[parent(i)])) {
			std::swap(elems[i], elems[parent(i)]);
			std::swap(index[elems[i]], index[elems[parent(i)]]);
			i = parent(i);
		}
	}
	void change_val(T old, T changed) {
		change_key(key(old),changed);
	}
	// float value up if its value was changed indirectly (through comparator)
	void check_key(size_t i) {
		while (i > 1 && cmp(elems[i], elems[parent(i)])) {
			std::swap(elems[i], elems[parent(i)]);
			std::swap(index[elems[i]], index[elems[parent(i)]]);
			i = parent(i);
		}		
	}


	// iteration -------------
	iterator begin() {return elems.begin() + 1;}
	iterator end() 	 {return elems.end();}
	const_iterator begin() const {return elems.cbegin() + 1;}
	const_iterator end() const 	 {return elems.cend();}

	bool is_maxheap() const {return std::is_heap(elems.begin()+1, elems.end());}
	bool is_minheap() const {return std::is_heap(elems.begin()+1, elems.end(), std::greater<T>{});}
	bool correct_index() const {
		for (auto itr : index) if (itr.first != elems[itr.second])
			return false;
		return true;
	}
};


// iterator wrappers for non-POD types
// only difference is operator* which does 2 levels of dereference
template <typename Iter>
struct Heap_iterator {
	using CR = const Heap_iterator<Iter>&;
	using T = typename std::iterator_traits<typename Iter::value_type>::value_type;
	Iter cur;

	void operator++() {++cur;}
	void operator--() {--cur;}
	Heap_iterator operator++(int) {return {cur++};}
	Heap_iterator operator--(int) {return {cur--};}
	T& operator*() {return **cur;}
	Iter& operator->() {return cur;}
	bool operator==(CR other) {return other.cur == cur;}
	bool operator!=(CR other) {return !(*this == other);}
};
template <typename Iter>
struct Heap_const_iterator {
	using CR = const Heap_const_iterator<Iter>&;
	using T = typename std::iterator_traits<typename Iter::value_type>::value_type;
	Iter cur;

	void operator++() {++cur;}
	void operator--() {--cur;}
	Heap_const_iterator operator++(int) {return {cur++};}
	Heap_const_iterator operator--(int) {return {cur--};}
	T operator*() const {return **cur;}
	const Iter& operator->() const {return cur;}
	bool operator==(CR other) {return other.cur == cur;}
	bool operator!=(CR other) {return !(*this == other);}
};

// for non-PODs, hold pointers to only 1 copy of data
// pointers are relatively small and easily swappable
template <typename T, typename Cmp>
class Heap_impl<T, Cmp, false>{	
	// in case raw pointer gets swapped out for std::shared_ptr
	using TP = T*;
	// hash on the data pointed to
	struct TP_hash {
		size_t operator()(const TP& vp) const {
			return std::hash<T>()(*vp);
		}
	};
	// equality on data pointed to
	struct TP_equal {
		bool operator()(const TP& a, const TP& b) const {
			return *a == *b;
		}
	};
	// default representation is as a vector, index at 1 so need 1 extra element
	std::vector<TP> elems;
	// associate each key with an index, hash with value pointed by
	std::unordered_map<TP, size_t, TP_hash, TP_equal> index;
	// comparator
	Cmp cmp;
	size_t parent(size_t i) {return i >> 1;}
	size_t left(size_t i) {return i << 1;}
	size_t right(size_t i) {return (i << 1) + 1;}

	// create only 1 actual copy of the data
	void push_back(const T& v) {
		TP actual_value {new T{v}};
		index[actual_value] = elems.size();
		elems.push_back(actual_value);
	}
	// runs in O(n) time, produces max heap from unordered input array
	void build_heap() {
		// second half of elems are leaves, 1 elem is maxheap by default
		for (size_t i = elems.size() >> 1; i != 0; --i) 
			heapify(i);
	}
	// adjusts elems[i] assuming it's been modified to be smaller than its children
	// runs in O(lgn) time, floats elems[i] down
	void heapify(size_t i) {
		while (true) {
			size_t l = left(i), r = right(i);
			size_t largest = i;	
			// largest of elems[i], elems[l], and elems[r]
			if (l < elems.size() && cmp(*elems[l], *elems[i]))
				largest = l;
			if (r < elems.size() && cmp(*elems[r], *elems[largest]))
				largest = r;
			// do until elems[i] is max heap
			if (largest == i) break;
			std::swap(elems[i], elems[largest]);
			std::swap(index[elems[i]], index[elems[largest]]);
			i = largest;
		}
	}
public:
	using value_type = T;
	using iterator = Heap_iterator<typename std::vector<TP>::iterator>;
	using const_iterator = Heap_const_iterator<typename std::vector<TP>::const_iterator>;

	// construction ------------
	// O(n) time construction via these constructors (would be O(nlgn) for repeated inserts)
	Heap_impl(Cmp& c) : elems{nullptr}, cmp(c) {}
	Heap_impl(std::initializer_list<T> l, Cmp&& c = Cmp{}) : cmp(c) {
		elems.reserve(l.size()+1);
		elems.push_back(nullptr);
		for (const T& v : l) push_back(v);
		build_heap();
	}
	template <typename Iter>
	Heap_impl(Iter begin, Iter end, Cmp&& c = Cmp{}) : cmp(c) {
		elems.reserve(end - begin + 1);
		elems.push_back(nullptr);
		while (begin != end) {push_back(*begin); ++begin;}
		build_heap();
	}
	~Heap_impl() {for (TP p : elems) delete p;}

	// query ---------------
	bool empty() const 	{return elems.size() <= 1;}
	size_t size() const {return elems.size() - 1;}
	T top() const 		{return *elems[1];}
	// index of key, 0 means key not found
	size_t key(const T& k) const {
		TP kp {&k};
		auto itr = index.find(kp);
		return (itr == index.end())? 0 : itr->second;
	}
	// for temporary references	
	size_t key(T&& k) const {
		TP kp {&k};
		auto itr = index.find(kp);
		return (itr == index.end())? 0 : itr->second;
	}

	// extraction ----------
	// runs in O(lgn) time due to heapify
	T extract_top() {
		// heap underflow
		if (elems.size() <= 1) return T{}; 

		T top {*elems[1]};
		std::swap(elems[1],elems.back());
		index[elems[1]] = 1;

		index.erase(elems.back());
		delete elems.back();
		elems.pop_back();

		heapify(1);
		return top;
	}

	// modification ----------
	void insert(const T& key) {
		push_back(key);
		check_key(elems.size()-1);
	}
	// O(n) like constructor for all elements
	template <typename Iter>
	void batch_insert(Iter begin, Iter end) {
		while (begin != end) {push_back(*begin); ++begin;}
		build_heap();
	}
	// repeadtedly compare against parent
	void change_key(size_t i, const T& key) {
		// change should only float up (so only increase on maxheap and decrease on minheap)
		if (cmp(*elems[i], key)) return;
		index.erase(elems[i]);
		delete elems[i];

		elems[i] = new TP{key};

		index[elems[i]] = i;

		while (i > 1 && cmp(elems[i], elems[parent(i)])) {
			std::swap(elems[i], elems[parent(i)]);
			std::swap(index[elems[i]], index[elems[parent(i)]]);
			i = parent(i);
		}
	}
	void change_val(const T& old, const T& changed) {
		change_key(key(old), changed);
	}
	// float value up if its value was changed indirectly (through comparator)
	void check_key(size_t i) {
		while (i > 1 && cmp(*elems[i], *elems[parent(i)])) {
			std::swap(elems[i], elems[parent(i)]);
			std::swap(index[elems[i]], index[elems[parent(i)]]);
			i = parent(i);
		}		
	}

	// iteration -------------
	iterator begin() {return {elems.begin() + 1};}
	iterator end() 	 {return {elems.end()};}
	const_iterator begin() const {return {elems.begin() + 1};}
	const_iterator end() const 	 {return {elems.end()};}

	// bool is_maxheap() const {return std::is_heap(elems.begin()+1, elems.end(), cmp);}
	// bool is_minheap() const {return std::is_heap(elems.rbegin(), elems.rend()-1, cmp);}
	bool correct_index() const {
		for (auto itr : index) if (itr.first != elems[itr.second])
			return false;
		return true;
	}
};


template <typename T, typename Cmp = std::greater<T>>
using Heap = Heap_impl<T, Cmp, std::is_pod<T>::value>;

}



