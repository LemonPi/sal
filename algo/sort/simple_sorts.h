#pragma once
#include <algorithm>

namespace sal {

// Bubble sort O(n^2) time O(1) space
template <typename Iter>
void bub_sort(Iter begin, const Iter end) {
    for (; begin != end - 1; ++begin) {
        for (Iter cur = end - 1; cur != begin; --cur)
            if (*cur < *(cur - 1)) std::swap(*cur, *(cur - 1));
    }
}
template <typename Container>
void bub_sort(Container& c) { bub_sort(c.begin(), c.end()); }

// Linear insert sort O(n^2) time O(1) space
// Costs only O(nk) time if all elements at most k places away from correct position
template <typename Iter>
void lin_sort(const Iter begin, const Iter end, Iter cur) {
    for (; cur != end; ++cur) {
        auto key = std::move(*cur);
        auto ins = cur - 1;
        for (; begin <= ins && key < *ins; --ins)
            *(ins + 1) = std::move(*ins);    // keep shifting to the right until element larger than key found
        *(ins + 1) = std::move(key);
    }
}
template <typename Iter>
void lin_sort(const Iter begin, const Iter end) { lin_sort(begin, end, begin); }
template <typename Container>
void lin_sort(Container& c) { lin_sort(c.begin(), c.end()); }

// Binary insertion sort O(n^2) time O(1) space, binary search to insert instead of swapping
template <typename Iter>
void ins_sort(const Iter begin, const Iter end, Iter cur ) {
    for(; cur < end; ++cur) {
        typename Iter::value_type pivot = std::move(*cur);

        // upper_bound is binary search for correct insertion position
        const Iter pos = std::upper_bound(begin, cur, pivot);
        for(Iter ins = cur; ins != pos; --ins)
            *ins = std::move(*(ins - 1));
        *pos = std::move(pivot);
    }
}
template <typename Iter>
void ins_sort(const Iter begin, const Iter end) { ins_sort(begin, end, begin); }
template <typename Container>
void ins_sort(Container& c) { ins_sort(c.begin(), c.end()); }

}