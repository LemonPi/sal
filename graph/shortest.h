#pragma once
#include <deque>
#include "search.h"		// initialize single source
#include "utility.h"	// topological sort, shortest path vertex, comparator, and visitor
#include "../../algo/macros.h"
// subset of graph searches, specifically searching for shortest paths
// solves (1) single-source (2) single-destination (3) single pair (4) all pairs



// cannot have cycles, since negative weight cycles would cause -infinity
// positive cycles can be removed to get shorter path, and 0 weight cycles do nothing


namespace sal {


// these algorithms are like BFS, except with weighted edges
// start of single source shortest path -------------

// single-source general edge weights, DP and is slower than others
// O(VE) time, does O(V) relaxations on each vertex
// applicable in many other algorithms, such as for solving difference constraints
template <typename Graph, typename Visitor = Shortest_visitor>
SPM<Graph> bellman_ford(const Graph& g, typename Graph::vertex_type s, Visitor&& visitor = Shortest_visitor{}) {
	SPM<Graph> property;
	initialize_single_source(property, g, s);

	// need at most |V| - 1 passes since shortest path is always simple (cycles are banned)
	size_t pass {1};
	bool changed {true};
	while (changed) {
		changed = false;
		// iterate over all edges (u,v)
		for (auto u = g.begin(); u != g.end(); ++u)
			for (auto v = u.begin(); v != u.end(); ++v) 
				if (visitor.relax(property, {*u, v})) changed = true;
		if (++pass == g.num_vertex()) break;	// in case negative cycle exists
	}
	// check if triangle inequality violated
	for (auto u = g.begin(); u != g.end(); ++u)
		for (auto v = u.begin(); v != u.end(); ++v) 
			if (property[*v].distance > property[*u].distance + v.weight())
				return SPM<Graph>{};	// -infinity cycle exists
	return property;
}



// single source DAG shortest path in O(V+E) time
// relax according to a topological sort of vertices
// always well defined (can't have cycles)
// can be used to find longest path (critical path) by negating all weights
template <typename Graph, typename Visitor = Shortest_visitor>
SPM<Graph> shortest_dag(const Graph& g, typename Graph::vertex_type s, Visitor&& visitor = Shortest_visitor{}) {
	using V = typename Graph::vertex_type;
	SPM<Graph> property;
	initialize_single_source(property, g, s);

	// explore vertices in topological order
	std::deque<V> exploring;
	topological_sort(g, std::front_inserter(exploring));

	for (const V& u : exploring) {
		auto edge = g.adjacent(u);
		for (auto v = edge.first; v != edge.second; ++v)
			visitor.relax(property, {u, v});
	}
	return property;
}
// simpler if the graph can be modified
template <typename Graph, typename Visitor = Shortest_visitor>
SPM<Graph> critical_dag(Graph& g, typename Graph::vertex_type s, Visitor&& visitor = Shortest_visitor{}) {
	// negate each edge and run shortest dag
	for (auto u = g.begin(); u != g.end(); ++u)
		for (auto v = u.begin(); v != u.end(); ++v)
			v.weight() = -v.weight();
	return shortest_dag(g, s, visitor);
}



// single source directed graph
// assumes non-negative weighted edges, O((V+E)lgV) with binary heap (priority queue)
struct DJ_visitor {
	// relaxes an edge if it meets certain requirements
	template <typename Property_map, typename Queue>
	void relax(Property_map& property, 
				const std::unordered_set<typename Property_map::key_type>& explored,
				Queue& exploring, 
				const Edge<typename Property_map::key_type, 
			    typename Property_map::mapped_type::edge_type>& edge) {
		// haven't explored yet and the cost is less
		if (explored.find(edge.dest()) == explored.end() && 
			edge.weight() < property[edge.dest()].distance + edge.weight()) {

			property[edge.dest()].distance = property[edge.source()].distance + edge.weight();
			property[edge.dest()].parent = edge.source();
			// fix heap property
			exploring.sift_up(exploring.key(edge.dest()), edge.dest());
		}		
	}
};

template <typename Graph>
SPM<Graph> dijkstra(const Graph& g, typename Graph::vertex_type s, DJ_visitor&& visitor = {}) {
	using V = typename Graph::vertex_type;
	using Cmp = Shortest_cmp<SPM<Graph>>;
	SPM<Graph> property;
	initialize_single_source(property, g, s);

	std::unordered_set<V> explored;
	// comparator querying on distance
	Heap<V, Cmp> exploring {Cmp{property}};
	exploring.batch_insert(g.begin(), g.end());

	while (!exploring.empty()) {
		V u {exploring.extract_top()};
		explored.insert(u);
		auto edges = g.adjacent(u);
		for (auto v = edges.first; v != edges.second; ++v) 
			visitor.relax(property, explored, exploring, {u, v});
	}

	return property;
}

}	// end namespace sal