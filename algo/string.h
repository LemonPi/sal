#pragma once
#include <string>
#include <sstream>

namespace sal {

std::string squish_whitespace(const std::string& s) {
    std::stringstream ss;
    for (size_t i = 0; i < s.size(); ++i) {
        ss << s[i];
        if (std::isspace(s[i])) {
            while (std::isspace(s[i])) {
                ++i;
            }
            --i;
        }
    }
    return ss.str();
}

// Levenshtein distance (edit actions include insertion, deletion, and substitution)
// very similar to longest common subsequence
template <typename Sequence>
size_t levenshtein(const Sequence& a, const Sequence& b) {
	// optimal substructure and overlapping subproblems, therefore can use dynamic programming
    if (a == b) return 0;
    if (a.empty()) return b.size();
    if (b.empty()) return a.size();
    // ensure space usage is O(min(a.size(), b.size()))
    if (b.size() > a.size()) return levenshtein(b, a);

    // only 1 row actually needed
    size_t cur[b.size() + 1];
    // 1 extra element to hold "just found" value
    size_t prev_cost, next_cost;

    for (size_t col = 0; col <= b.size(); ++col) cur[col] = col;

    // cur is used so that everything before index col is the current row
    // everything at and after index col is prev row
    // temp holds value just computed at index col (to not override prev)
    for (size_t row = 0; row < a.size(); ++row) {
        // delete row + 1 chars from a to match empty t
        prev_cost = row + 1;
        for (size_t col = 0; col < b.size(); ++col) {
            size_t cost = (a[row] == b[col])? 0 : 1;
            next_cost = std::min(prev_cost + 1, std::min(cur[col + 1] + 1, cur[col] + cost));
            // update the previous one only after calculating the current
            cur[col] = prev_cost;
            prev_cost = next_cost;
        }
        // last element
        cur[b.size()] = next_cost;
    }

    // prev would have last current after loop
    return cur[b.size()];
}

// split string, taken from Evan Teran's stackoverflow response
std::vector<std::string> ssplit(const std::string &s, char delim) {
    std::vector<std::string> elems;
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}


// left trim string, taken from Evan Teran's stackoverflow response
std::string& ltrim(std::string& s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
    return s;
}
std::string ltrim(const std::string& s) {
    std::string trimmed {s};
    trimmed.erase(trimmed.begin(), std::find_if(trimmed.begin(), trimmed.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
    return trimmed;
}

}	// end namespace sal