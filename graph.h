#pragma once
#include <iostream>
#include "graph/interface.h"
#include "graph/adjacency_list.h"
#include "graph/adjacency_matrix.h"

namespace sal {
// by default undirected
using graph = Adjacency_list<>;
using graph_alt = Adjacency_matrix;

}