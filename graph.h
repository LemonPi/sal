#pragma once
#include <iostream>
#include "graph/adjacency_list.h"
#include "graph/adjacency_matrix.h"
#include "graph/search.h"

namespace sal {
// by default undirected
template <typename V>
using graph = Adjacency_list<V>;
template <typename V>
using digraph = Adjacency_list_directed<V>;
using graph_alt = Adjacency_matrix;

}