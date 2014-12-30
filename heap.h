#pragma once
#include <algorithm>
#include <vector>
#include <initializer_list>

using namespace std;

namespace sal {


template <typename T>
class Heap {	// maxheap parent greater than children
	vector<T> elems;
	size_t parent(size_t i) {return i >> 1;}
	size_t left(size_t i) {return i << 1;}
	size_t right(size_t i) {return (i << 1) + 1;}
	void max_heapify(size_t i);
	void build_maxheap();
public:
	Heap(initializer_list<T> l) {
		elems.reserve(l.size());
		for (const T& v : l) elems.push_back(v);
		build_maxheap();
	}
	template <typename Iter>
	Heap(Iter first, Iter last) {
		elems.reserve(last - first);
		while (first != last) elems.push_back(*first++);
		build_maxheap();
	}
	bool is_maxheap();
};

template <typename T>
void Heap<T>::max_heapify(size_t i) {
	// assumes left(i) and right(i) are max heaps
	// float elems[i] down
	while (true) {
		size_t l = left(i), r = right(i);
		size_t largest = i;	// largest of elems[i], elems[l], and elems[r]
		if (l < elems.size() && elems[l] > elems[i])
			largest = l;
		if (r < elems.size() && elems[r] > elems[largest])
			largest = r;
		// do until elems[i] is max heap
		if (largest == i) break;
		swap(elems[i], elems[largest]);
		i = largest;
	}
}

template <typename T>
void Heap<T>::build_maxheap() {
	// second half of elems are leaves, 1 elem is maxheap by default
	for (size_t i = elems.size() >> 1; ; --i) {
		max_heapify(i);
		if (i == 0) break;
	}
}

template <typename T>
bool Heap<T>::is_maxheap() {
	return is_heap(elems.begin(), elems.end());
}

}