#pragma once
#include <queue>
#include <stack>
#include <unordered_map>
#include <limits>
#include "adjacency_list.h"
#define IS_WHITE(x) (property[x].start == unsigned_infinity)
#define IS_GREY(x) (property[x].finish == 0)

namespace sal {
constexpr size_t unsigned_infinity {std::numeric_limits<size_t>::max()};

/* algorithms expects Graph to have: 
	vertex iterators in begin() and end() (and reverse iterators in rbegin(), rend())
  		* gives vertex name of type V
  		begin() and end() to give adjacent iterators

  	adjacent iterator pair in adjacent(V)
  		* gives vertex name of type V, the destination vertex
  		dest() gives name of destination vertex (same as * operator)
  		weight() gives weight of edge to destination vertex
*/

template <typename V, typename E>
struct Edge {
	V u, v;
	E w;
	template <typename Adj_iter>
	Edge(V a, Adj_iter& b) : u{a}, v{*b}, w(b.weight()) {}
	V source() const   {return u;}
	V dest() const     {return v;}
	E weight() const   {return w;}
};

template <typename V>
struct BFS_vertex {
	using edge_type = size_t;
	// distance estimate of s to vertex, always >= distance
	// after completion, estimate is == distance
	size_t distance;
	// flattened tree to trace back path, unnecessary if only path weight needed
	V parent;
	size_t edge() const {return 1;}
};

template <typename V>
struct DFS_vertex {
	// time stamps
	// discovery and finish time, from 1 to |V|
	size_t start;
	size_t finish;
	V parent;
	size_t edge() const {return 1;}
};

// resulting property maps from BFS and DFS
template <typename V>
using BFS_property_map = std::unordered_map<V, BFS_vertex<V>>;
template <typename Graph>
using BPM = BFS_property_map<typename Graph::vertex_type>;

template <typename V>
using DFS_property_map = std::unordered_map<V, DFS_vertex<V>>;
template <typename Graph>
using DPM = DFS_property_map<typename Graph::vertex_type>;

// assumes unweighted graph, and assumes V is simple type (name of vertex)
// works with directed and undirected
// O(V + E) time
// creates a vertex property map relative to source (breadth first tree)

struct BFS_visitor {
	template <typename Property_map, typename Queue>
	void relax(Property_map& property, Queue& exploring, 
		const Edge<typename Property_map::key_type, 
				   typename Property_map::mapped_type::edge_type>& edge) {
		if (property[edge.dest()].distance == unsigned_infinity) {
			property[edge.dest()].distance = property[edge.source()].distance + 1;
			property[edge.dest()].parent = edge.source();
			exploring.push(edge.dest());
		}
	}
	// BFS doesn't load a queue to explore, start at root, so return void
	template <typename Property_map, typename Graph>
	void initialize_vertex(Property_map& property, const Graph& g) {
		for (auto v = g.rbegin(); v != g.rend(); ++v) 
			property[*v] = {unsigned_infinity, *v};
	}	
};

template <typename Graph, typename Visitor = BFS_visitor>
BPM<Graph> bfs(const Graph& g, typename Graph::vertex_type s, Visitor&& visitor = BFS_visitor{}) {
	using V = typename Graph::vertex_type;
	BPM<Graph> property;
	visitor.initialize_vertex(property, g);

	property[s].distance = 0;
	property[s].parent = s;

	std::queue<V> exploring;
	exploring.push(s);

	while (!exploring.empty()) {
		V u {exploring.back()};
		exploring.pop();
		// leaving exploring means fully explored
		// for each adjacent vertex
		auto edges = g.adjacent(u);
		for (auto v = edges.first; v != edges.second; ++v) {
			// relax
			visitor.relax(property, exploring, {u, v});
		}
	}
	return property;
}

struct DFS_visitor {
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
struct Graph_single_visitor : public DFS_visitor {
	using V = typename Graph::vertex_type;
	V source;
	Graph_single_visitor(V s) : source{s} {}
	template <typename Property_map>
	std::vector<V> initialize_vertex(Property_map& property, const Graph& g) {
		for (auto v = g.rbegin(); v != g.rend(); ++v)
			property[*v] = {unsigned_infinity, 0, *v};
		return {source};
	}
};

// depth first search, used usually in other algorithms
// explores all vertices of a graph, produces a depth-first forest
template <typename Graph, typename Visitor = DFS_visitor>
DPM<Graph> dfs(const Graph& g, Visitor&& visitor = DFS_visitor{}) {
	using V = typename Graph::vertex_type;
	DPM<Graph> property;
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
DPM<Graph> dfs(const Graph& g, typename Graph::vertex_type s, int) {	// dummy argument for overloading
	return dfs(g, Graph_single_visitor<Graph>{s});
}

// recursive version of dfs, much simpler, but can blow up the stack
template <typename Graph, typename Visitor = DFS_visitor>
DPM<Graph> dfs_recurse(const Graph& g, Visitor&& visitor = DFS_visitor{}) {
	DPM<Graph> property;
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
template <typename Graph, typename Visitor = DFS_visitor>
DPM<Graph> dfs_recurse(const Graph& g, typename Graph::vertex_type u, Visitor&& visitor = DFS_visitor{}) {
	DPM<Graph> property;
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