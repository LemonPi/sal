#pragma once
#include <deque>
#include "search.h"		// bfs visitor
#include "utility.h"	// topological sort
#include "../../algo/macros.h"
// subset of graph searches, specifically searching for shortest paths
// solves (1) single-source (2) single-destination (3) single pair (4) all pairs



// cannot have cycles, since negative weight cycles would cause -infinity
// positive cycles can be removed to get shorter path, and 0 weight cycles do nothing


namespace sal {

// property map vertices need parent to backtrace to source
// these algorithms are like BFS, except with weighted edges
template <typename V, typename E>
struct Shortest_vertex {
	using edge_type = E;
	// distance estimate of s to vertex, always >= distance
	// after completion, estimate is == distance
	E distance;
	V parent;
	E edge() const {return distance;}
};

template <typename V, typename E>
using Shortest_property_map = std::unordered_map<V, Shortest_vertex<V,E>>;
template <typename Graph>
using SPM = Shortest_property_map<typename Graph::vertex_type, typename Graph::edge_type>;


struct Shortest_visitor : public BFS_visitor {
	// returns whether relaxed or not
	template <typename Property_map>
	bool relax(Property_map& property,
		const Edge<typename Property_map::key_type, 
				   typename Property_map::mapped_type::edge_type>& edge) {
		if (property[edge.dest()].distance > property[edge.source()].distance + edge.weight()) {
			property[edge.dest()].distance = property[edge.source()].distance + edge.weight();
			property[edge.dest()].parent = edge.source();
			return true;
		}
		return false;
	}
	// default initialization of BFS visitor (initialization single source)
};


// single-source general edge weights, DP and is slower than others
// O(VE) time, does O(V) relaxations on each vertex
template <typename Graph, typename Visitor = Shortest_visitor>
SPM<Graph> bellman_ford(const Graph& g, typename Graph::vertex_type s, Visitor&& visitor = Shortest_visitor{}) {
	SPM<Graph> property;
	visitor.initialize_vertex(property, g, s);

	// need at most |V| - 1 passes since shortest path is always simple (cycles are banned)
	bool changed {true};
	while (changed) {
		changed = false;
		// iterate over all edges (u,v)
		for (auto u = g.begin(); u != g.end(); ++u)
			for (auto v = u.begin(); v != u.end(); ++v) {
				if (visitor.relax(property, {*u, v})) changed = true;
			}
	}
	// check if triangle inequality violated
	for (auto u = g.begin(); u != g.end(); ++u)
		for (auto v = u.begin(); v != u.end(); ++v) 
			if (property[*v].distance > property[*u].distance + v.weight())
				return {};	// -infinity cycle exists
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
	visitor.initialize_vertex(property, g, s);

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




// at most |V| distinct vertices and |V| - 1 edges
// assumes non-negative weighted edges, O((V+E)lgV) with binary heap (priority queue)
template <typename Graph, typename Visitor = Shortest_visitor>
size_t dijkstra(const Graph& g, typename Graph::vertex_type s, Visitor&& visitor = Shortest_visitor{}) {
	// using V = typename Graph::vertex_type;
	// using E = typename Graph::edge_type;
	SPM<Graph> property;
	visitor.initialize_vertex(property, g);
	return 0;
}

}	// end namespace sal