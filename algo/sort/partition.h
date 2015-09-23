#pragma once

namespace sal {

// Partition, O(n) core algorithm that's used in many others
template <typename Iter>
Iter partition(Iter begin, Iter end) {
    using T = typename Iter::value_type;
    // swap first element with middle, could replace with random
    // put pivot element out of the way at begin (never to be swapped out during partition)
    std::iter_swap(begin, begin+(end-begin)/2);
    T pivot {*begin};
    // i points to the boundary of the left partition, grown when something less than pivot should be put there
    auto i = begin;
    for (auto j = begin + 1; j != end; ++j) {
        if (*j < pivot)
            std::iter_swap(++i, j);
    }
    // put pivot element back to the boundary and its correct rank
    std::iter_swap(i, begin);
    return i;
}
template <typename Container>
typename Container::iterator partition(Container& c) { return partition(c.begin(), c.end()); }

// partition on a unary predicate
template <typename Iter, typename Unary_pred>
Iter partition(Iter begin, Iter end, Unary_pred p) {
    // cannot know whether one pass will be enough unlike the standard < partition
    while (true) {
        // continuous left side that satisfies the predicate
        while ((begin != end) && p(*begin)) ++begin;
        if (begin == end--) break;
        // continuous right side that satisfies the predicate
        while ((begin != end) && !p(*end)) --end;
        if (begin == end) break;
        std::iter_swap(begin++, end);
    }
    return begin;
}

}