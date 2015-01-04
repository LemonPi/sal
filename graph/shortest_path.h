#pragma once
#include "adjacency_list.h"

#ifndef POS_INF
#define POS_INF(T) (std::numeric_limits<T>::max())
#endif
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
	template <typename Property_map, typename Queue>
	void relax(Property_map& property, Queue& exploring, 
		const Edge<typename Property_map::key_type, 
				   typename Property_map::mapped_type::edge_type>& edge) {
		if (property[edge.dest()].distance > property[edge.source()].distance + edge.weight()) {
			property[edge.dest()].distance = property[edge.source()].distance + edge.weight();
			property[edge.dest()].parent = edge.source();
		}
	}
	// default initialization of BFS visitor
};


// single-source general edge weights, DP and is slower than others
// O(VE) time, does O(V) relaxations on each vertex
template <typename Graph, typename Visitor = Shortest_visitor>
SPM<Graph> bellman_ford(const Graph& g, typename Graph::vertex_type s, Visitor&& visitor = Shortest_visitor{}) {
	using V = typename Graph::vertex_type;
	using E = typename Graph::edge_type;
	SPM<Graph> property;
	std::vector<Edge<V,E>> exploring;
	visitor.initialize_vertex(property, g, s);

	// only need |V| - 1 passes since shortest path is always simple (cycles are banned)
	for (size_t pass = 1; pass != g.num_vertex(); ++pass)
		// iterate over all edges (u,v)
		for (auto u = g.begin(); u != g.end(); ++u)
			for (auto v = u.begin(); v != u.end(); ++v) {
				Edge<V,E> edge {*u, v};
				exploring.push_back(edge);
				visitor.relax(property, exploring, edge);
			}
	// check if triangle inequality violated
	for (const Edge<V,E>& edge : exploring) 
		if (property[edge.dest()].distance > property[edge.source()].distance + edge.weight())
			return {};	// -infinity cycle exists
	return property;
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