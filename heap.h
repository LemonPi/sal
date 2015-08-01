#pragma once
#include <algorithm>
#include <initializer_list>
#include <unordered_map>
#include <vector>
#include "../algo/macros.h"	// SENTINEL

namespace sal {

// simple, plain old data implementation can work with copies of it
// swapping should be cheap and there shouldn't be ownership issues
// by default maxheap where parent greater than children
template <typename T, typename Cmp = std::greater<T>>
class Heap {	
	// default representation is as a vector, index at 1 so need 1 extra element
	std::vector<T> elems;
	// comparator for sorting in a maxheap cmp(a, b) being true means a is an ancestor of b
	Cmp cmp;
	size_t parent(size_t i) const {return i >> 1;}
	size_t left(size_t i)   const {return i << 1;}
	size_t right(size_t i)  const {return (i << 1) + 1;}
	size_t tail() const {return elems.size() - 1;}
public:
	// only exposing due to occasionaly indirect value changes
	// float value up for directly changed value
	void sift_up(size_t i, const T& item) {
		while (i > 1 && cmp(item, elems[parent(i)])) {
			elems[i] = elems[parent(i)];
			i = parent(i);
		}		
		elems[i] = item;
	}
	// float value up if its value was changed indirectly (through comparator)
	void sift_up(size_t i) {
		const T& prev_val {elems[i]};
		sift_up(i, prev_val);
	}
	// adjusts elems[i] assuming it's been modified to be smaller than its children
	// runs in O(lgn) time, floats elems[i] down
	void sift_down(size_t hole) {
		T item {std::move(elems[hole])};
		size_t child {left(hole)};
		while (child < elems.size()) {
			if (child + 1 < elems.size() && cmp(elems[child + 1], elems[child]))
				++child;
			if (cmp(elems[child], item)) {
				elems[hole] = std::move(elems[child]);
				hole = child;
				child = left(child);
			}
			else break;
		}
		elems[hole] = std::move(item);
	}
private:	
	// runs in O(n) time, produces max heap from unordered input array
	void build_heap() {
		// second half of elems are leaves, 1 elem is maxheap by default
		for (size_t i = elems.size() >> 1; i != 0; --i) 
			sift_down(i);
	}
	size_t find_key(size_t head, const T& elem) const {
		if (elems[head] == elem) return head;
		size_t child_location {0};	// 0 indicates not found
		// cmp(elem, child) being true would imply that elem has to be a parent of child, therefore don't search there
		if (right(head) < elems.size() && !cmp(elem, elems[right(head)]))
			child_location = find_key(right(head), elem);
		if (child_location) return child_location;

		if (left(head) < elems.size() && !cmp(elem, elems[left(head)]))
			child_location = find_key(left(head), elem);
		return child_location;
	}
public:
	using value_type = T;
	using iterator = typename std::vector<T>::iterator;
	using const_iterator = typename std::vector<T>::const_iterator;
	// default T value as sentinel

	// construction ------------
	// O(n) time construction via these constructors (would be O(nlgn) for repeated inserts)
	explicit Heap(Cmp&& c) : elems{T{}}, cmp(c) {}
	// transfer ownership of contents (int to prevent overload conflict)
	template <typename Container>
	Heap(Container&& container, int, Cmp&& c = Cmp{}) : elems(std::move(container)), cmp(c) {
		elems.push_back(SENTINEL(T));
		std::swap(elems.back(), elems[0]);
		build_heap();
	} 
	Heap(std::initializer_list<T> l, Cmp&& c = Cmp{}) : cmp(c) {
		elems.reserve(l.size()+1);
		elems.push_back(SENTINEL(T));
		// cannot move from initializer list...
		for (const T& v : l) {elems.push_back(v);}
		build_heap();
	}
	template <typename Iter>
	Heap(Iter begin, Iter end, Cmp&& c = Cmp{}) : cmp(c) {
		elems.reserve(end - begin + 1);
		elems.push_back(SENTINEL(T));
		while (begin != end) {elems.emplace_back(std::move(*begin)); ++begin;}
		build_heap();
	}

	// query ---------------
	bool empty() const 	{return elems.size() <= 1;}
	size_t size() const {return elems.size() - 1;}
	const T& top() const 		{return elems[1];}
	// get index to item
	size_t key(const T& item) const {
		return find_key(1, item);
	}


	// extraction ----------
	// runs in O(lgn) time due to sift_down
	T extract_top() {
		// heap underflow
		if (empty()) return SENTINEL(T); 

		T top {elems[1]};

		size_t hole {1};
		size_t child {2};
		while (child+1 < elems.size()) {
			if (cmp(elems[child + 1], elems[child])) 
				++child;
			elems[hole] = elems[child];
			hole = child;
			child = left(child);
		}
		// replace hole with rightmost leaf
		elems[hole] = elems[tail()];
		elems.pop_back();
		sift_up(hole, elems[hole]);
		return top;
	}
	// gets top while inserting a new element, same complexity as extracting top
	T replace_top(T&& new_elem) {
		if (empty()) {elems.emplace_back(new_elem); return SENTINEL(T);}
		T top {elems[1]};

		size_t hole {1};
		size_t child {2};
		while (child+1 < elems.size()) {
			if (cmp(elems[child + 1], elems[child])) 
				++child;
			elems[hole] = elems[child];
			hole = child;
			child = left(child);
		}
		// replace hole with new element
		sift_up(hole, new_elem);
		return top;
	}



	// modification ----------
	// O(lgn)
	void insert(T key) {
		elems.emplace_back();
		sift_up(elems.size()-1, key);
	}
	// O(n) like constructor for all elements
	template <typename Iter>
	void batch_insert(Iter begin, Iter end) {
		while (begin != end) {elems.push_back(*begin); ++begin;}
		build_heap();
	}

	// only for direct changes; for indirect changes, have to know whether to sift up or down
	void increase_key(size_t i, const T& changed) {
		sift_up(i, changed);	// move closer to root
	}
	void decrease_key(size_t i, const T& changed) {
		elems[i] = changed;
		sift_down(i);	// move closer to a leaf
	}



	// iteration in element order (not heap) -------------
	iterator begin() {return elems.begin() + 1;}
	iterator end() 	 {return elems.end();}
	const_iterator begin() const {return elems.cbegin() + 1;}
	const_iterator end() const 	 {return elems.cend();}

	bool is_maxheap() const {return std::is_heap(elems.begin()+1, elems.end());}
	bool is_minheap() const {return std::is_heap(elems.begin()+1, elems.end(), std::greater<T>{});}
};


}



