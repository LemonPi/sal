/* Written by Johnson Zhong unless otherwise noted
contact: johnsonzhong@hotmail.ca | johnsonzhong.me
------------------------
sorting algorithms
functions overloaded to also accept containers

comparison sorts O(nlgn) optimal
partition(begin, end) -> iterator to pivot after partitioning begin to end using center as pivot
bub_sort(begin, end)
lin_sort(begin, end)  	 linear insertion sort
ins_sort(begin, end)  	 binary insertion sort
mer_sort(begin, end)
qck_sort(begin, end)
heap_sort(begin, end)

distribution sorts O(kn) optimal, works for integer like values, k is number of "digits"
cnt_sort(begin, end, k)     k is range of digit's value
cnt_sort(begin, end, k, op) op selects for what is a digit
rdx_sort(begin, end)
rdx_sort(begin, end, bits)  sort up to consider of bits # of bits

hybrid sorts
tim_sort(begin, end)
pat_sort(begin, end)		takes a lot of patience to wait for it to sort...

*/

#pragma once

#include "sort/partition.h"			// partition
#include "sort/simple_sorts.h"		// bubble and insertion sort
#include "sort/comparison_sorts.h"	// merge, quick, and heap sort
#include "sort/distribution_sorts.h"// counting and radix sort
#include "sort/timsort.h"
#include "sort/patience_sort.h"