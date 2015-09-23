#pragma once
#include "Suffix_array.h"

namespace sal {

// wrapper around suffix array usage
template <typename Sequence>
Sequence lc_substr(Sequence a, const Sequence& b) {
    size_t b_index {a.size()};
    a.push_back('$');
    a.insert(a.end(), b.begin(), b.end());
    Suffix_array<> sa {a};

    size_t longest {};
    size_t index {};

    for (size_t i = 0; i < sa.size() - 1; ++i) {
        // cannot be from 1 sequence only, one has to be below and one above 
        if ((sa.suffix(i) < b_index) ^ (sa.suffix(i+1) < b_index)) 
            if (sa.common_prefix_len(i) > longest) {
                longest = sa.common_prefix_len(i);
                index = i;
            }
    }
    return Sequence(a.begin() + sa.suffix(index), a.begin() + sa.suffix(index) + longest);
}


// O(n*m) time and space where n and m are lengths of a and b, respectively
template <typename Indexable>
Indexable lc_subseq(const Indexable& a, const Indexable& b) {
    size_t table[a.size()+1][b.size()+1];
    // build table
    for (size_t row = 0; row <= a.size(); ++row) {
        for (size_t col = 0; col <= b.size(); ++col) {
            if (col == 0 || row == 0) table[row][col] = 0;
            // common last character, is the lcs of the prefixes + 1
            else if (a[row-1] == b[col-1]) table[row][col] = table[row-1][col-1] + 1;
            else table[row][col] = max(table[row-1][col], table[row][col-1]);
        }
    } 
    size_t len {table[a.size()][b.size()]};
    Indexable lcs(len+1, 0);
    // backtrack table from lower right corner
    for (size_t row = a.size(), col = b.size(); row > 0 && col > 0;) {
        // current char matches then is part of lcs
        if (a[row-1] == b[col-1]) {
            lcs[len - 1] = a[row - 1];
            --row; --col; --len;    // move upper left
        }
        // else go where there are more matches
        else if (table[row-1][col] > table[row][col-1]) --row;
        else --col;
    }
    return lcs;   
}
// same as lc_subseq except that only O(min(n,m)) space is used
template <typename Indexable>
size_t lc_subseq_len(const Indexable& a, const Indexable& b) {
    // has optimal substructure and overlapping subproblems, therefore use dynamic programming
    if (a == b) return 0;
    if (a.empty()) return b.size();
    if (b.empty()) return a.size();
    // ensure space usage is O(min(a.size(), b.size()))
    if (b.size() > a.size()) return lc_subseq_len(b, a);

    // only 1 row actually needed
    size_t cur[b.size() + 1];
    // 1 extra element to hold "just found" value
    size_t prev_cost, next_cost;

    for (size_t col = 0; col <= b.size(); ++col) cur[col] = 0;

    // [0, cur[col-1]][cur[col], end] is used so that everything before index col is the current row
    // current row     previous row
    // prev_cost holds value just computed at index col (to not override prev)
    for (size_t row = 0; row < a.size(); ++row) {
        // delete row + 1 chars from a to match empty t
        prev_cost = 0;
        for (size_t col = 0; col < b.size(); ++col) {
            if (a[row] == b[col]) next_cost = cur[col] + 1; // \ diagonal + 1
            else next_cost = max(cur[col + 1], prev_cost);  // max(| up, -- left) 
            // next_cost = min(prev_cost + 1, min(cur[col + 1] + 1, cur[col] + cost));
            // update the previous one only after calculating the current
            cur[col] = prev_cost;
            prev_cost = next_cost;
        }
        // last element
        cur[b.size()] = next_cost;
    }

    return cur[b.size()];
}

}