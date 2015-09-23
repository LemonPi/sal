#pragma once
#include <limits>
#include <vector>

namespace sal {

// Merge sort O(nlogn) time O(n) space
template <typename Iter>
void merge(Iter begin, Iter middle, Iter last) {
    using T = typename Iter::value_type;
    std::vector<T> left(begin, middle + 1);
    std::vector<T> right(middle + 1, last + 1);
    left.push_back(std::numeric_limits<T>::max()); // sentinel
    right.push_back(std::numeric_limits<T>::max());
    for (int i = 0, j = 0, k = 0; k <= last - begin; ++k) {
        if (left[i] < right[j]) begin[k] = left[i++];
        else begin[k] = right[j++];
    }
}
template <typename Iter>
void merge_helper(Iter begin, Iter last) {
    if (begin < last) {
        Iter pivot = begin + (last - begin)/2;   // half way point rounded down
        merge_helper(begin, pivot);
        merge_helper(pivot + 1, last);
        merge(begin, pivot, last);
    }
}
template <typename Iter>
void mer_sort(Iter begin, Iter end) {
    merge_helper(begin, end - 1);
}
template <typename Container>
void mer_sort(Container& c) { mer_sort(c.begin(), c.end()); }

// Quick sort O(nlgn) time O(n) space sorts [begin, last]
template <typename Iter>
void quick_sort(Iter begin, Iter last) {
    if (last - begin > 0) {
        auto pivot = *(begin + std::distance(begin, last) / 2); 
        Iter left {begin};
        Iter right {last};
        while (left <= right) {
            while (*left < pivot) ++left;
            while (pivot < *right) --right;
            if (left <= right) { std::swap(*left, *right); ++left; --right; }
        }
        quick_sort(begin, right);
        quick_sort(left, last);
    }
}
template <typename Iter>
void qck_sort(Iter begin, Iter end) {
    quick_sort(begin, end - 1);
}
template <typename Container>
void qck_sort(Container& c) { qck_sort(c.begin(), c.end()); }

template <typename Iter>
void heap_sort(Iter begin, Iter end) {
    make_heap(begin, end);
    sort_heap(begin, end);
    // sort heap puts the root (largest elem) at the back
    // then considers the heap as [begin, end-1)
    // max_heapify root and proceed in loop
}
template <typename Container>
void heap_sort(Container& c) { heap_sort(c.begin(), c.end()); }

}