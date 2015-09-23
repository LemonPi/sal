#pragma once
#include <limits>
// divide by 2 to prevent overflow
#define POS_INF(T) (std::numeric_limits<T>::max() >> 1)
#define PRINTLINE(line) (std::cout << line << std::endl)
#define PRINT2D(table,row,col) {for (size_t r = 0; r < (row); ++r) {for (size_t c = 0; c < (col); ++c) std::cout << table[r][c] << ' '; std::cout << '\n';}
#define SENTINEL(T) (T{})

template <typename T>
using Iter_value = typename std::iterator_traits<T>::value_type;
template <typename T>
using Iter_diff = typename std::iterator_traits<T>::difference_type;

