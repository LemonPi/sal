#pragma once
#include <iostream>
#include "graph/adjacency_list.h"
#include "graph/adjacency_matrix.h"


namespace sal {
// undirected by default
template <typename V>
using graph = Adjacency_list<V>;
using graph_alt = Adjacency_matrix;
// directed
template <typename V>
using digraph = Adjacency_list_directed<V>;

}