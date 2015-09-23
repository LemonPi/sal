/* Written by Johnson Zhong unless otherwise noted
contact: johnsonzhong@hotmail.ca | johnsonzhong.me
------------------------
permutations and combinatorics

perm(sequence, k)   -> make sequence into its kth permutation, k up to factorial(sequence.size())
allperms(sequence)  -> set of all permutations of sequence

combine(start, end, operation)       -> set of values resulting from combining each element using operation
combine(start, end, operation, pred) -> combine, but only combine elements satisfying predicate
count_combos(values, sum)            -> number of ways to reach sum by combining values

*/

#pragma once
#include <algorithm>
#include <iterator>
#include <set>
#include <string>
#include <vector>

namespace sal {

using namespace std;
using large_int = unsigned long long;

large_int fact_int(int i) {
    large_int res {1};
    while (i > 1) res *= i--;
    return res;
}

// taken from Permaquid's stackoverflow response, easily parallizable
template <typename Indexable>
void perm(Indexable& s, large_int k) { // k is from 0 to s.size()! - 1
    for (unsigned j = 1; j < s.size(); ++j) {
        swap(s[k % (j + 1)], s[j]);
        k = k / (j + 1);
    }
}

template <typename Indexable>
vector<Indexable> allperms(const Indexable& s) {
    large_int max_perm {fact_int(s.size())};
    std::vector<Indexable> res;
    for (large_int k = 0; k < max_perm; ++k) {
        // std::cout << "inside loop with k " << k << std::endl;
        Indexable s_perm(s);
        perm(s_perm, k);
        res.push_back(std::move(s_perm));
    }
    return res;
}

template <typename Indexable>
set<Indexable> allperms_distinct(const Indexable& s) {
    large_int max_perm {fact_int(s.size())};
    std::set<Indexable> res;
    for (large_int k = 0; k < max_perm; ++k) {
        Indexable s_perm(s);
        perm(s_perm, k);
        res.insert(s_perm);
    }
    return res;
}

// combinations
template <typename Iter, typename Op>
set<typename Iter::value_type> combine(Iter start, Iter end, Op op) {
    set<int> combos;
    for (; start != end; ++start) 
        for (auto j = start; j != end; ++j)
            combos.insert(op(*start, *j));
    return combos;
}
template <typename Container, typename Op>
set<typename Container::value_type> combine(Container& c, Op op) { return combine(c.begin(), c.end(), op); }

template <typename Iter, typename Op, typename Pred>
set<typename Iter::value_type> combine(Iter start, Iter end, Op op, Pred pred) {
    set<int> combos;
    for (; start != end; ++start) 
        for (auto j = start; j != end; ++j)
            if (pred(*start, *j))
                combos.insert(op(*start, *j));
    return combos;
}
template <typename Container, typename Op, typename Pred>
set<typename Container::value_type> combine(Container& c, Op op, Pred pred) { return combine(c.begin(), c.end(), op, pred); }

// count number of ways to generate sum using num_vals elements from vals
template <typename T>
size_t count_combos(const vector<T>& vals, T sum) {
    // create table to avoid recomputation of same subproblem
    // table stores # of solutions for certain sum (as index)
    vector<int> table(sum+1, 0);

    table[0] = 1;   // base case of 1 solution

    for (const T& value_unit : vals)
        for (T val = value_unit; val <= sum; ++val)
            table[val] += table[val - value_unit];

    return table[sum];
}

}
