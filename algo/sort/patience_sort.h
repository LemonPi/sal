#pragma once
#include <algorithm>	// lower_bound
#include <stack>
#include "../search.h"	// bin_search

namespace sal {

// patience sort, theoretically O(nlgn) complexity
// practically much slower due to memory access and moving piles (VERY SLOW)
// implementation taken from wikibooks
template <typename Pile>
bool pless(const Pile& a, const Pile& b) { return a.top() < b.top(); }
template <typename Pile>
bool pmore(const Pile& a, const Pile& b) { return b.top() < a.top(); }

template <typename Iter>
void pat_sort(Iter begin, Iter end) {
	using T = typename Iter::value_type;
	using Pile = typename std::stack<T>;
	std::vector<Pile> piles;	// maintained in ordered by top pile element

	for (auto cur = begin; cur != end; ++cur) {
		Pile new_pile;
		new_pile.push(*cur);
		// find which pile to insert in
		auto ins = std::lower_bound(piles.begin(), piles.end(), new_pile, pless<Pile>);
		if (ins != piles.end()) ins->push(*cur);	// add to existing pile
		else piles.push_back(new_pile);	// create new pile
		
	}
	// sorted array satisfy heap property for min-heap
	for (auto cur = begin; cur != end; ++cur) {
		// greatest valued pile is moved to back while being a heap
		std::pop_heap(piles.begin(), piles.end(), pmore<Pile>);
		*cur = piles.back().top();
		piles.back().pop();
		if (piles.back().empty()) piles.pop_back();	// empty pile
		// maintain heap
		else std::push_heap(piles.begin(), piles.end(), pmore<Pile>);
	}
}
}