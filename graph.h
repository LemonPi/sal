#pragma once
#include <iostream>
#include "graph/adjacency_list.h"
#include "graph/adjacency_matrix.h"


namespace sal {
// undirected by default
template <typename V, typename E = int>
using graph = Adjacency_list<V,E>;
template <typename E = int> 
using graph_mat = Adjacency_matrix<size_t,E>;
// directed
template <typename V, typename E = int>
using digraph = Adjacency_list_directed<V,E>;
template <typename E = int>
using digraph_mat = Adjacency_matrix_directed<E>;
}