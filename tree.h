#pragma once
#include "tree/tree.h"
#include "tree/order_tree.h"
#include "tree/treap.h"

// recommended types for replacing std functions
namespace sal {

// 4 times faster insert (for all cases), 1.3 times slower iteration (!?), same clear speed, 2 times faster find
// 2 times faster erase compared to std::set
template <typename T>
using Set = Basic_treap<T>;

}