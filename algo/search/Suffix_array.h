#pragma once
#include <algorithm>    // sort
#include <iomanip>  // setw
#include <iostream>
#include <vector>

namespace sal {

// longest common substring using suffix arrays
// might consider implementing SAIS construction for almost sublinear performance!
template <typename Sequence = std::string>
class Suffix_array {
    // assume the original sequence won't be modified...
    const Sequence& s;
    // sa[i] gives starting index of ith smallest suffix
    // longest common prefix (lcp) lcp[i] gives length of longest common prefix of suffix starting at sa[i] and sa[i+1]
    std::vector<size_t> sa, rank, lcp;

    void build() {
        sa.resize(s.size());
        rank.resize(s.size());
        std::vector<size_t> temp(s.size());
        // initialize sa[] and rank[]
        // initial rank is just the character value (1-gram)
        for (size_t i = 0; i < s.size(); ++i) {
            sa[i] = i;
            rank[i] = s[i];
        }

        // gap is length of m-gram / 2; start with 2-grams
        for (size_t gap = 1;; gap *= 2) {
            // sort m-grams
            Suffix_cmp suffix_cmp(*this, gap);
            std::sort(sa.begin(), sa.begin() + s.size(), suffix_cmp);
        
            // compute lexicographic rank by comparing against neighbour
            for (size_t i = 0; i < s.size() - 1; ++i) 
                temp[i + 1] = temp[i] + suffix_cmp(sa[i], sa[i+1]);
                // suffix_cmp is 0 if they're the same, else 1

            // temp holds rank relative to position, map to per m-gram
            for (size_t i = 0; i < s.size(); ++i)
                rank[sa[i]] = temp[i];

            // top rank being s.size() - 1 means no gram have same rank, done
            if (temp[s.size() - 1] == s.size() - 1) break;
        }
    }

    void build_lcp(){
        lcp.resize(s.size());
        for (size_t suffix = 0, common_c = 0; suffix < s.size(); ++suffix)
            // going through lexicographic rank of each suffix
            if (rank[suffix] != s.size() - 1) {
                // next is starting location of the next lexicographical suffix
                for (size_t next = sa[rank[suffix] + 1]; s[suffix + common_c] == s[next + common_c];) ++common_c;
                lcp[rank[suffix]] = common_c;
                // next common character length is atleast last - 1
                if (common_c) --common_c; 
            }

        // after building lcp, no longer need rank
        rank.clear();
        rank.shrink_to_fit();
    }

    size_t common_start(size_t i, size_t j){
        size_t common {0};
        while (s[i++] == s[j++]) ++common;
        return common;
    }


    class Suffix_cmp {
        Suffix_array<Sequence>& sa;
        size_t gap;
    public:
        Suffix_cmp(Suffix_array& s, size_t g) : sa(s), gap{g} {}
        bool operator()(size_t i, size_t j) {
            if (sa.rank[i] != sa.rank[j])
                return sa.rank[i] < sa.rank[j];
            i += gap;   // every m-gram is split into 2 known parts, + gap is 2nd part
            j += gap;
            // index i > j implies i is at the end, meaning it is shorter
            return (i < sa.s.size() && j < sa.s.size())? sa.rank[i] < sa.rank[j] : i > j;
        }
    };
    friend class Suffix_cmp;

public:
    Suffix_array(const Sequence& text) : s(text) {build(); build_lcp();}

    size_t size() const {return sa.size();}
    // reference to original text
    const Sequence& text() const {return s;}
    // starting index of ith smallest suffix
    size_t suffix(size_t ith_suffix) const {return sa[ith_suffix];}
    // length of common prefix for ith smallest suffix and ith-1 suffix
    size_t common_prefix_len(size_t ith_suffix) const {return lcp[ith_suffix];}
    // print out all suffixes in lexicographic order for learning
    void print() const {
        for (size_t i = 0; i < sa.size(); ++i) {
            std::cout << std::setw(5) << sa[i] << '-' << lcp[i] << ": "  << Sequence(s.begin() + sa[i], s.end()) << std::endl;
        }
    }

    // number of suffixes starting target
    std::vector<size_t> occurrance(const Sequence& target) const {
        auto begin = std::lower_bound(sa.begin(), sa.end(), target,
            [&](size_t i, const Sequence& t){
                for (size_t ii = 0; ii < t.size(); ++ii) {
                    // overflow for original text; shorter text is smaller
                    if (i+ii >= s.size()) return true;
                    if (s[i+ii] < t[ii]) return true;
                    if (s[i+ii] > t[ii]) return false;
                    // else check the next character
                }
                // on tie, return current pointer (property of lower_bound)
                return false; });

        if (begin == sa.end()) return {};

        std::vector<size_t> occurance_list;
        size_t suffix = begin-sa.begin();
        // substrings are adjacent, so just have to check common prefix length exceeds target length
        while (lcp[suffix] >= target.size()) {occurance_list.push_back(sa[suffix]); ++suffix;}
        // last suffix covered by previous suffix's lcp
        occurance_list.push_back(sa[suffix]);
        return occurance_list;
    }
};

}
