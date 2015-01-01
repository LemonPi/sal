#pragma once
#include <queue>
#include <stack>
#include <unordered_map>
#include <limits>
#include "adjacency_list.h"
#define IS_WHITE(x) (property[x].start == unsigned_infinity)
#define IS_GREY(x) (property[x].finish == 0)

namespace sal {

/* algorithms expects Graph to have: 
	vertex iterators in begin() and end() (and reverse iterators in rbegin(), rend())
  		* gives vertex name of type V
  		begin() and end() to give adjacent iterators

  	adjacent iterator pair in adjacent(V)
  		* gives vertex name of type V, the destination vertex
  		dest() gives name of destination vertex (same as * operator)
  		weight() gives weight of edge to destination vertex
*/


// assumes non-negative weighted edges, O((V+E)lgV) with binary heap (priority queue)
template <typename Graph, typename V>
size_t dijkstra(const Graph& g, V s, V x) {
	return 0;
}

template <typename V>
struct Vertex_bfs_property {
	// distance estimate of s to vertex, always >= distance
	// after completion, estimate is == distance
	size_t distance;
	// flattened tree to trace back path, unnecessary if only path weight needed
	V parent;
};

template <typename V>
struct Vertex_dfs_property {
	// time stamps
	// discovery and finish time, from 1 to |V|
	size_t start;
	size_t finish;
	V parent;
};

// resulting property maps from BFS and DFS
template <typename V>
using Vertex_bfs_property_map = std::unordered_map<V, Vertex_bfs_property<V>>;
template <typename Graph>
using VBP = Vertex_bfs_property_map<typename Graph::vertex_type>;

template <typename V>
using Vertex_dfs_property_map = std::unordered_map<V, Vertex_dfs_property<V>>;
template <typename Graph>
using VDP = Vertex_dfs_property_map<typename Graph::vertex_type>;

// assumes unweighted graph, and assumes V is simple type (name of vertex)
// works with directed and undirected
// O(V + E) time
// creates a vertex property map relative to source (breadth first tree)
constexpr size_t unsigned_infinity {std::numeric_limits<size_t>::max()};


template <typename Graph>
VBP<Graph> bfs(const Graph& g, typename Graph::vertex_type s) {
	using V = typename Graph::vertex_type;
	VBP<Graph> property;
	for (auto v = g.begin(); v != g.end(); ++v) 
		property[*v] = {unsigned_infinity, *v};

	property[s].distance = 0;
	property[s].parent = s;

	std::queue<V> exploring;
	exploring.push(s);

	while (!exploring.empty()) {
		V cur {exploring.back()};
		// leaving exploring means fully explored
		exploring.pop();

		// explore adjacent to current
		auto edges = g.adjacent(cur);
		for (auto adj = edges.first; adj != edges.second; ++adj) {
			auto& n_p = property[*adj];
			// relax
			if (n_p.distance == unsigned_infinity) {
				n_p.distance = property[cur].distance + 1;
				n_p.parent = cur;
				exploring.push(*adj);
			}
		}
	}

	return std::move(property);
}

struct Graph_visitor {
	template <typename Property_map, typename Graph>
	std::vector<typename Graph::vertex_type> initialize_vertex(Property_map& property, const Graph& g) {
		std::vector<typename Graph::vertex_type> exploring;
		for (auto v = g.rbegin(); v != g.rend(); ++v) {
			// mark unexplored
			property[*v] = {unsigned_infinity, 0, *v};
			// expore every vertex
			exploring.push_back(*v);
		}
		return std::move(exploring);
	}
	template <typename Graph>
	void start_vertex(typename Graph::vertex_type, const Graph&) {}
	template <typename Graph>
	void discover_vertex(typename Graph::vertex_type, const Graph&) {}
	template <typename Graph>
	void finish_vertex(typename Graph::vertex_type, const Graph&) {}
	template <typename Graph>
	void back_edge(typename Graph::vertex_type, const Graph&) {}
	template <typename Graph>
	void forward_or_cross_edge(typename Graph::vertex_type, const Graph&) {}
};

// for DFS on only 1 source vertex
template <typename Graph>
struct Graph_single_visitor : public Graph_visitor {
	using V = typename Graph::vertex_type;
	V source;
	Graph_single_visitor(V s) : source{s} {}
	template <typename Property_map>
	std::vector<typename Graph::vertex_type> initialize_vertex(Property_map& property, const Graph& g) {
		for (auto v = g.rbegin(); v != g.rend(); ++v)
			property[*v] = {unsigned_infinity, 0, *v};
		return {source};
	}
};

// depth first search, used usually in other algorithms
// explores all vertices of a graph, produces a depth-first forest
template <typename Graph, typename Visitor = Graph_visitor>
VDP<Graph> dfs(const Graph& g, Visitor&& visitor = Graph_visitor{}) {
	using V = typename Graph::vertex_type;
	VDP<Graph> property;
	// use visitor to initialize stack (order of DFS)
	std::vector<V> exploring {visitor.initialize_vertex(property, g)};
	
	size_t explore_time {0};

	while (!exploring.empty()) {
		V cur {exploring.back()};
		// tree edge, first exploration on (u,v)
		if (IS_WHITE(cur)) {
			property[cur].start = ++explore_time;
			visitor.start_vertex(cur, g);
		}
		
		bool fully_explored {true};
		auto edges = g.adjacent(cur);
		for (auto adj = edges.first; adj != edges.second; ++adj) {
			// check if any neighbours haven't been explored
			if (IS_WHITE(*adj)) {
				property[*adj].start = ++explore_time;
				property[*adj].parent = cur;
				exploring.push_back(*adj);
				fully_explored = false;	// still have unexplored neighbours
				break;		// only push 1 neighbour to achieve depth first
			}
			else if (IS_GREY(*adj)) visitor.back_edge(*adj, g);
			else visitor.forward_or_cross_edge(*adj, g);
		}
		if (fully_explored) {
			exploring.pop_back();
			if (property[cur].finish == 0) {// default value
				property[cur].finish = ++explore_time;
				visitor.finish_vertex(cur, g);
			}
		}
	}

	return std::move(property);
}
// overload for specifying a source, can't use other visitors
template <typename Graph>
VDP<Graph> dfs(const Graph& g, typename Graph::vertex_type s, int) {	// dummy argument for overloading
	return dfs(g, Graph_single_visitor<Graph>{s});
}

// recursive version of dfs, much simpler, but can blow up the stack
template <typename Graph, typename Visitor = Graph_visitor>
VDP<Graph> dfs_recurse(const Graph& g, Visitor&& visitor = Graph_visitor{}) {
	VDP<Graph> property;
	using V = typename Graph::vertex_type;
	// no need to reverse traverse now
	std::vector<V> exploring {visitor.initialize_vertex(property, g)};

	size_t explore_time {0};

	for (auto v = exploring.rbegin(); v != exploring.rend(); ++v) 
		// start vertex of a new depth first tree
		if (IS_WHITE(*v)) {
			visitor.start_vertex(*v, g);
			dfs_visit(g, *v, property, explore_time, visitor);
		}

	return property;
}
// explore only 1 vertex
template <typename Graph, typename Visitor = Graph_visitor>
VDP<Graph> dfs_recurse(const Graph& g, typename Graph::vertex_type u, Visitor&& visitor = Graph_visitor{}) {
	VDP<Graph> property;
	// no need to reverse traverse now
	for (auto v = g.begin(); v != g.end(); ++v)
		property[*v] = {unsigned_infinity, 0, *v};

	size_t explore_time {0};

	visitor.start_vertex(u, g);
	dfs_visit(g, u, property, explore_time, visitor);

	return property;
}

template <typename Graph, typename Visitor, typename Property_map>
void dfs_visit(const Graph& g, typename Graph::vertex_type u, Property_map& property, size_t& explore_time, Visitor& visitor) {
	// discover vertex
	property[u].start = ++explore_time;
	visitor.discover_vertex(u, g);

	auto edges = g.adjacent(u);
	for (auto adj = edges.first; adj != edges.second; ++adj) {
		if (IS_WHITE(*adj)) {
			property[*adj].parent = u;
			dfs_visit(g, *adj, property, explore_time, visitor);
		}
		else if (IS_GREY(*adj)) visitor.back_edge(*adj, g);
		else visitor.forward_or_cross_edge(*adj, g);
	}
	// finish vertex
	property[u].finish = ++explore_time;
	visitor.finish_vertex(u, g);
}	

}