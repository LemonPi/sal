/* Written by Johnson Zhong unless otherwise noted
contact: johnsonzhong@hotmail.ca | johnsonzhong.me
------------------------
finding elements inside sets

bin_search(begin, end, key) -> iterator to the element comparing equal to key, else end

intersection(set of sets)   -> set of values contained in all the sets

select(begin, end, i)       -> ith smallest value from begin to end

min_max(begin, end)			-> pair with minimum as first and maximum as second of a sequence

sub_match(sentence, word)   -> iterator to the starting element of a match of word in sentence, else end

lc_subseq_len(sequence, sequence) -> returns length of longest common subsequence (extraction of characters)
lc_subseq(sequence, sequence)     -> returns longest common subsequence


class: Suffix_array efficient data structure for finding common substrings, exposed through
lc_substr(string, string)   -> string of longest common substring
Suffix_array(string, string)-> construct for reuse
sa.lc_substr()              -> calls are cheap after construction
*/

#pragma once
#include "search/element_select.h"
#include "search/longest_common.h"
#include "search/string_search.h"
#include "search/Suffix_array.h"
