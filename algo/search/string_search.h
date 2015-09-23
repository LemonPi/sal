#pragma once
#include <iterator>	// advance
#include <vector>
namespace sal {

// comparator for strings
struct Strcmp {
    bool less(const std::string& a, const std::string& b) {return a.compare(b) < 0;}
    bool eq(const std::string& a, const std::string& b) {return a.compare(b) == 0;}
};


// implementation of KMP with O(n + k) where n and k are the lengths of strings to match
// returns iterator to the start of the word, else s.end()
// good for small # of possible values and resusable subpatterns
// builds backtracking table for use below
template <typename Indexable>
std::vector<int> kmp_table(const Indexable& w) {
    size_t start_pos {};    // current matched up to from start
    std::vector<int> backtrack(w.size()); 
    backtrack[0] = -1;
    backtrack[1] = 0;

    for (size_t pos = 2; pos < w.size();) {
        if (w[pos - 1] == w[start_pos]) {           // substring continues
            ++start_pos;
            backtrack[pos] = start_pos;
            ++pos;
        }
        else if (start_pos > 0) start_pos = backtrack[start_pos];   // fall back to last substring
        else {                              // can't fall back
            backtrack[pos] = 0;
            ++pos;
        }
    }
    return backtrack;
}

template <typename Indexable>
typename Indexable::const_iterator sub_match(const Indexable& s, const Indexable& w) {
    // work with size_t inside due to convenience
    size_t match_start {};    // start index of match in s
    size_t word_pos {};       // current match position in w
    std::vector<int> backtrack {kmp_table(w)}; // decide how many chars to backtrack

    while (match_start + word_pos < s.size()) {
        if (s[match_start + word_pos] == w[word_pos]) {
            if (word_pos == w.size() - 1) {
                auto itr = s.begin();
                std::advance(itr, match_start);
                return itr;
            }
            ++word_pos;
        }
        else {
            if (backtrack[word_pos] > -1) {
                match_start += word_pos - backtrack[word_pos];
                word_pos = backtrack[word_pos];
            }
            else {
                ++match_start;
                word_pos = 0;
            }
        }
    }
    return s.end(); // unsuccessful
}



}