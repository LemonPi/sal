#pragma once
#include <climits>
#include <iterator>
#include <algorithm>
#include <vector>
#include "../search/element_select.h"	// min_max
#include "../macros.h"	// Iter_value
#include "simple_sorts.h"	// lin_sort

namespace sal {

// counting sort, assumes each input is integral between 0 to k
// O(n) if k = O(n)
template <typename Iter, typename Op>
void cnt_sort(Iter begin, Iter end, size_t range, Op op) {
    using T = typename Iter::value_type;
    std::vector<int> counts(range);   // init to 0
    for (auto i = begin; i != end; ++i) // count # elems == i
        ++counts[op(*i)]; 
    for (size_t i = 1; i < range; ++i)
        counts[i] += counts[i-1];   // turn into # elems <= i
    std::vector<T> res(end - begin);
    for (auto j = end;;) {
        --j;
        res[--counts[op(*j)]] = *j;
        if (j == begin) break;
    }
    std::copy(res.begin(), res.end(), begin);
}
template <typename Iter>
void cnt_sort(Iter begin, Iter end, size_t range = 0) {
	// no range specified requires one pass through the data to find the maximum
	if (range == 0) {
		auto mm_pair = min_max(begin, end);
		range = mm_pair.second - mm_pair.first;
	}
    cnt_sort(begin, end, range, [](typename Iter::value_type v){return v;});
}
template <typename Container>
void cnt_sort(Container& c, size_t range) { cnt_sort(c.begin(), c.end(), range); }

// radix sort, more practical than counting sort
// O(d(n + k)) running time where d is # digits, k is size of digit
template <typename T>	// overload digit_cmp for non-integral types to provide radix sort with digits
class Digit_cmp {   // functor for comparing a "digit" (particular bits)
    const long long mask; // 0..63 bitfield to test against
    const size_t to_shift;
public:
    Digit_cmp(long long m, size_t ts) : mask{m}, to_shift{ts} {}
    // by default assumes integral, just shifts
    size_t operator()(T n) const {    // char assuming r = 8
        return (n >> to_shift) & mask; // shift then mask for unit digit
    }
};
// specialization for strings
template <>
class Digit_cmp<std::string> {
	const size_t digit;
public:
	Digit_cmp(size_t d) : digit{d} {}
	size_t operator()(const std::string& str) {
		// 0 indicates past the end of the string
		return str.size() > digit ? str[digit] : 0;
	}
};






// intermediate struct to get partial template specialization
template<typename Iter, typename T, size_t range = 256>
struct rdx_impl {
	static void rdx_sort(Iter begin, Iter end, size_t bits) { 
	    // bits is # bits to consider up to if a max val is known ahead of time
	    // most efficent (theoretically) when digits are base n, having lg(n) bits
	    constexpr size_t digit_bits {8};		// # bits in digit, 8 works well for 32 and 64 bit vals

	    size_t d {0};                   // current digit #
	    for (long long mask = (1 << digit_bits) - 1;
	    	d * digit_bits < bits;) {// ex. 0x000000ff for setting lower 8 bits on 32 bit num
	        cnt_sort(begin, end, range, Digit_cmp<T>(mask, digit_bits*d));
	        ++d;
	    }
	}
};

// specialization for potentially negative values
template <typename Iter>
struct rdx_impl<Iter, int> {
	static void rdx_sort(Iter begin, Iter end, size_t bits) {
		// offset every element by adding min element (to ensure smallest is 0)
		int min_elem {};
		for (Iter cur = begin; cur < end; ++cur) min_elem = std::min(*cur, min_elem);
		for (Iter cur = begin; cur < end; ++cur) *cur -= min_elem;

	    constexpr size_t digit_bits {8};		// # bits in digit, 8 works well for 32 and 64 bit vals

	    size_t d {0};                   // current digit #
	    for (long long mask = (1 << digit_bits) - 1;
	    	d * digit_bits < bits;) {// ex. 0x000000ff for setting lower 8 bits on 32 bit num
	        cnt_sort(begin, end, 256, Digit_cmp<int>(mask, digit_bits*d));
	        ++d;
	    }	

	    // reapply offset 
	    for (Iter cur = begin; cur < end; ++cur) *cur += min_elem;	
	}
};

// specialization of rdx_sort for strings
template <typename Iter>	
struct rdx_impl<Iter, std::string> {	// enough to hold ASCII char range
	static void rdx_sort(Iter begin, Iter end, size_t) {
		// ignore additional int argument
		int len_max = {};
		for (auto str = begin; str != std::endl; ++str) len_max = max(len_max, str->size());
		for (int d = len_max - 1; d >= 0; --d)
			cnt_sort(begin, end, 128, Digit_cmp<std::string>(d));
	}
};



// specialization for floats
// if sign bit is set (negative), flip all bits, otherwise flip sign only
size_t float_flip(float f) {
	size_t mask = -(static_cast<int>(f) >> 31) | 0x80000000;
	return static_cast<int>(f) ^ mask;
}
// if sign bit is set (was positive), flip sign bit, otherwise flip all bits
size_t ifloat_flip(float f) {
	size_t mask = ((static_cast<int>(f) >> 31) - 1) | 0x80000000;
	return static_cast<int>(f) ^ mask;
}
template <typename Iter>
struct rdx_impl<Iter, float> {
	// static void rdx_sort(Iter begin, Iter end, )
};

// different general calling interfaces
template <typename Iter>   // use intermediate struct for partial specialization
void rdx_sort(Iter begin, Iter end, int bits) {
    rdx_impl<Iter, Iter_value<Iter>>::rdx_sort(begin, end, bits);
}
template <typename Iter>   // range of input not known, just use max ex. 32 bits for ints
void rdx_sort(Iter begin, Iter end) {
    int bits {sizeof(Iter_value<Iter>)*CHAR_BIT};
    rdx_sort(begin, end, bits);
}
template <typename Container>
void rdx_sort(Container& c) { rdx_sort(c.begin(), c.end()); }



// bucket sort in O(n) with std::sort for sorting on individual buckets
// hash must have size_t size() = #buckets; size_t operator(const T&) = bucket for object 
template <typename Iter, typename Hash>
void bucket_sort(Iter begin, const Iter end, Hash&& hash) {
	using T = typename std::iterator_traits<Iter>::value_type;
	std::vector<std::vector<T>> buckets(hash.size());
	// scatter into buckets
	for (auto cur = std::make_move_iterator(begin); cur != std::make_move_iterator(end); ++cur) {
		size_t b {hash(*cur)};
		std::cout << b << ' ' << *cur << std::endl;
		buckets[b].emplace_back(*cur);	// moved
	}
	// sort each bucket separately
	for (auto& bucket : buckets)
		std::sort(bucket.begin(), bucket.end());
	// gather back into original container
	for (auto& bucket : buckets) {
		for (auto cur = std::make_move_iterator(bucket.begin());
			  cur != std::make_move_iterator(bucket.end()); ++cur) {
			*begin = *cur;	// move back
			++begin;
		}
	}
}

// insertion sort is O(nk) where k is supposed to be small after bucketing
// sorting after putting back into container allows better locality, but requires comparison
template <typename Iter, typename Hash>
void bucket_ins_sort(Iter begin, const Iter end, Hash&& hash) {
	using T = typename std::iterator_traits<Iter>::value_type;
	std::vector<std::vector<T>> buckets(hash.size());
	// scatter into buckets
	for (auto cur = std::make_move_iterator(begin); cur != std::make_move_iterator(end); ++cur) {
		size_t b {hash(*cur)};
		std::cout << b << ' ' << *cur << std::endl;
		buckets[b].emplace_back(*cur);	// moved
	}
	// gather unsorted elements back into array
	auto beg = begin;
	for (auto& bucket : buckets) {
		for (auto cur = std::make_move_iterator(bucket.begin());
			  cur != std::make_move_iterator(bucket.end()); ++cur) {
			*beg = *cur;	// move back
			++beg;
		}
	}
	// insertion sort on the original container
	lin_sort(begin, end);
}

struct Number_bucket_hash {
	size_t num_buckets;
	double proportion_of_max;

	Number_bucket_hash() = default;
	template <typename Iter>
	Number_bucket_hash(Iter begin, Iter end, size_t n) : num_buckets{n}, 
		proportion_of_max{(num_buckets-1) / (double)*std::max_element(begin, end)} {}

	size_t size() const {
		return num_buckets;
	}
	template <typename T>
	size_t operator()(const T& val) const {
		return (size_t) (val * proportion_of_max);
	}
};


}	// end namespace sal