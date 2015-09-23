#pragma once
#include <iterator>			// distance
#include <unordered_set>
#include <unordered_map>
#include "../sort/partition.h"
#include "../macros.h"      // Iter_value

namespace sal {
using std::unordered_set;
using std::unordered_map;

// slightly modified version from Wikibooks
template <typename Iter, typename T, typename Cmp>  // special comparator requires less and eq methods, can compare cross types
Iter bin_search(Iter begin, Iter end, const T& key, Cmp cmp) {
    Iter not_found(end);
    Iter mid(begin + (std::distance(begin, end) / 2));
    while (begin < end) {
        // change iterators based on median val
        if (cmp.less(*mid, key)) begin = mid + 1;
        else if (cmp.eq(*mid, key)) return mid;
        else end = mid;
        mid = begin + (std::distance(begin, end) / 2);
    }
    return not_found;
}
template <typename Iter, typename T>
Iter bin_search(Iter begin, Iter end, const T& key) {
    Iter not_found = end;
    Iter mid = begin + (std::distance(begin, end) / 2);
    while (begin < end) {
        // change iterators based on median val
        if (*mid == key) return mid;
        else if (*mid < key) begin = mid + 1;
        else end = mid;
        mid = begin + (std::distance(begin, end) / 2);
    }
    return not_found;
}
template <typename Sequence, typename T>
typename Sequence::iterator bin_search(const Sequence& c, const T& key) { return bin_search(c.begin(), c.end(), key); }

// count doesn't matter (won't tell repeats)
template <typename Sequence_set>
unordered_set<typename Sequence_set::value_type::value_type> intersection(const Sequence_set& seq_set) {
    using Sequence = typename Sequence_set::value_type;
    using T = typename Sequence::value_type;

    if (seq_set.size() < 2) return unordered_set<T>{};    // can't intersect 0 or 1 sets

    unordered_map<T, size_t> elems;
    for (auto& v : *seq_set.begin()) elems[v] = 1;  // hash elements of first container
    // count occurance of each element in each container
    size_t container_num {0};
    for (const Sequence& seq : seq_set) {
        ++container_num;
        if (container_num == 1) continue;   // first container already loaded
        for (auto v : seq) {
            // short circuit logic here of using a cheap comparison before a more expensive find
            if (elems[v] == container_num-1 && elems.find(v) != elems.end()) // don't double count repeats
                ++elems[v];
        }
    }
    unordered_set<T> common;
    for (auto& pair : elems) {
        if (pair.second == container_num) common.insert(pair.first);
    }
    return common;
}


// select ith smallest element (1 is smallest) important algorithm used in many other places
// enables bypassing sorting (nlgn usually) when the exact order of elements don't matter in a range (ex. top 100)
// implements quickselect, with average O(n) performance
template <typename Iter>
Iter select(Iter begin, Iter end, size_t i) {
    if (end-begin < (int)i) return end;
    while (true) {
        if (end - begin == 1) return begin;
        Iter pivot {sal::partition(begin, end)};  
        size_t rank = pivot - begin + 1; // the order of pivot

        if (i == rank) return pivot;
        if (i < rank) end = pivot;   // look in the left partition
        else {begin = pivot + 1; i -= rank; } // look in the right
    }
}
template <typename Sequence>
typename Sequence::iterator select(Sequence& c, size_t i) { return select(c.begin(), c.end(), i); }


// find minimum and maximum in 3*ceil(n/2) comparisons
template <typename Iter>
std::pair<Iter_value<Iter>, Iter_value<Iter>> min_max(Iter begin, Iter end) {
    using T = Iter_value<Iter>;
    T min = begin[0];
    T max = begin[0];
    size_t size = end - begin;
    size_t start_index = (size & 1)? 1 : 0; // start at 0 if even, else 1
    for (size_t i = start_index; i < size; i += 2) {
        if (begin[i] < begin[i+1]) {
            if (begin[i] < min) min = begin[i];
            if (begin[i+1] > max) max = begin[i+1];
        }
        else {
            if (begin[i] > max) max = begin[i];
            if (begin[i+1] < min) min = begin[i+1];
        }
    }
    return {min,max};
}

}