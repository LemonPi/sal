#pragma once
#include <deque>
#include <limits>
#include <map>
#include <set>
#include <unordered_set>
#include <vector>
#include "search.h"		// dfs used in many algorithms, single source initialization
#include "../heap.h"	// used for piority queue
#include "../../algo/macros.h"	// POS_INF

namespace sal {
// for directed acyclic graph (dag)
// orders all vertices so that parent vertices are always before children
// if vertices are events, then sorting gives one possible sequence of events
// O(V + E) algorithm (as are most algorithms using DFS)
template <typename Output_iter>
struct Topological_visitor : public DFS_visitor {
	Output_iter out;
	Topological_visitor(Output_iter o) : out{o} {}

	template <typename Graph>
	void finish_vertex(typename Graph::vertex_type u, const Graph&) {*out++ = u;}
	template <typename Graph>
	void back_edge(typename Graph::vertex_type u, const Graph&) {std::cout << "back edge on " << u << std::endl;}
};

template <typename Graph, typename Output_iter>
void topological_sort(const Graph& g, Output_iter res) {
	dfs(g, Topological_visitor<Output_iter>{res});
}



struct Cycle_visitor : public DFS_visitor {
	bool has_backedge {false};
	template <typename Graph>
	void back_edge(typename Graph::vertex_type, const Graph&) {has_backedge = true;}	
};
template <typename Graph>
bool has_cycle(const Graph& g) {
	Cycle_visitor visitor;
	dfs(g, visitor);
	return visitor.has_backedge;
}

// transpose of directed graph is reversing all edges (u,v) to (v,u)
template <typename Graph>
Graph transpose(const Graph& g) {
	Graph g_t;
	for (auto u = g.begin(); u != g.end(); ++u) {
		for (auto v = u.begin(); v != u.end(); ++v) {
			g_t.add_edge(*v, *u);
		}
		g_t.add_vertex(*u);
	}
	return g_t;
} 

// for directed graphs, strongly connected components
// any vertex in a component can get to any other
// simple algorithm by guessing a vertex in a sink component, 
// then traversing full cycle to get entire component
// no easy way to get sink vertex, so use source vertices of the transpose
template <typename Output_iter>
struct Inorder_finish_visitor : public DFS_visitor {
	Output_iter out;
	Inorder_finish_visitor(Output_iter o) : out{o} {}

	template <typename Graph>
	void finish_vertex(typename Graph::vertex_type u, const Graph&) {*out++ = u;}

};

template <typename Graph>
using Connected_set =  std::vector<std::unordered_set<typename Graph::vertex_type>>;

template <typename Graph>
struct Connected_visitor : public DFS_visitor {
	using V = typename Graph::vertex_type;
	Connected_set<Graph> component_set;
	std::vector<V> finish_order;
	Connected_visitor(std::vector<V>&& vec) : finish_order{vec} {}

	// visit in order of descending finish time 
	template <typename Property_map>
	std::vector<V> initialize_vertex(Property_map& property, const Graph& g) {
		for (auto v : finish_order) 
			property[v] = {v};

		return std::move(finish_order);
	}

	// start a new set
	void start_vertex(typename Graph::vertex_type u, const Graph&) {
		component_set.push_back({u});
	}
	void finish_vertex(typename Graph::vertex_type u, const Graph&) {
		component_set.back().insert(u);
	}
};


template <typename Graph>
Connected_set<Graph> strongly_connected(const Graph& g) {
	using V = typename Graph::vertex_type;
	std::vector<V> finish_order;
	finish_order.reserve(g.num_vertex());
	// fill finish_order with vertices in descending order of finish time
	dfs(g, Inorder_finish_visitor<decltype(std::back_inserter(finish_order))>
		{std::back_inserter(finish_order)});

	Graph gt {transpose(g)};
	Connected_visitor<Graph> visitor {std::move(finish_order)};
	dfs(gt, visitor);

	return std::move(visitor.component_set);
}


// start of shortest path algorithms ------------- (their utilities)

// each category of algorithms have a (1) property mapped vertex type
// many algorithms of the same type will share the same (2) comparator
// (3) visitors are the least shared among algorithms

template <typename V, typename E = size_t>	// non-neg by default
struct Shortest_vertex {
	using edge_type = E;
	// distance estimate of s to vertex, always >= distance
	// after completion, estimate is == distance
	V parent;
	E distance;
	Shortest_vertex() = default;
	Shortest_vertex(const V& v) : parent{v}, distance{POS_INF(E)} {}
	E edge() const {return distance;}
};
template <typename Property_map>
struct Shortest_cmp {
	using V = typename Property_map::key_type;
	const Property_map& property;
	Shortest_cmp(const Property_map& p) : property(p) {}

	bool operator()(const V& u, const V& v) const {
		return (property.find(u)->second.distance) < (property.find(v)->second.distance); 
	}
};
struct Shortest_visitor {
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
};

template <typename V, typename E>
using Shortest_property_map = std::unordered_map<V, Shortest_vertex<V,E>>;
template <typename Graph>
using SPM = Shortest_property_map<typename Graph::vertex_type, typename Graph::edge_type>;


// Prim's algorithm for minimum spanning tree
// connected undirected graph, assuming positive weight
struct MST_visitor {
	// relaxes an edge if it meets certain requirements
	template <typename Property_map, typename Queue>
	void relax(Property_map& property, 
		const std::unordered_set<typename Property_map::key_type>& explored,
		Queue& exploring, 
		const Edge<typename Property_map::key_type, 
				   typename Property_map::mapped_type::edge_type>& edge) {

		// d_i == 0 means not in exploring
		// distance for MST means minimum edge weight connecting to it
		if (explored.find(edge.dest()) == explored.end() && edge.weight() < property[edge.dest()].distance) {
			property[edge.dest()].distance = edge.weight();
			property[edge.dest()].parent = edge.source();
			// fix heap property
			exploring.sift_up(exploring.key(edge.dest()), edge.dest());
		}		
	}
};

// can be made faster to O(E) time if edge weights are integers using an array as the exploring
// with each slot holding doubly linked list of vertices with that distance
template <typename Graph>
SPM<Graph> min_span_tree(const Graph& g, MST_visitor&& visitor = {}) {
	using V = typename Graph::vertex_type;
	using Cmp = Shortest_cmp<SPM<Graph>>;
	// property map of each vertex to their distance
	SPM<Graph> property;
	// comparator querying on distance
	initialize_single_source(property, g, *g.begin());

	std::unordered_set<V> explored;
	Heap<V, Cmp> exploring {Cmp{property}};
	exploring.insert(g.begin(), g.end());

	// batch insert is O(n) rather than O(nlgn) of inserting each sequentially

	while (!exploring.empty()) {
		V u {exploring.extract_top()};	// eventually all vertices extracted
		explored.insert(u);
		// for each adjacent vertex
		auto edges = g.adjacent(u);
		for (auto v = edges.first; v != edges.second; ++v) {
			// relaxation
			visitor.relax(property, explored, exploring, {u, v});
		}
	}
	return property;
}

// convert a property map to a tree
// expects property vertex to have parent and edge() which returns 1 for unweighted
template <typename Property_map>
graph<typename Property_map::key_type> pm_to_tree(const Property_map& property) {
	using V = typename Property_map::key_type;
	graph<V> g_mst;
	for (const auto& edge : property) 
		// cannot have self loops in tree, means root
		if (edge.first == edge.second.parent) g_mst.add_vertex(edge.first);
		else g_mst.add_edge(edge.first, edge.second.parent, edge.second.edge());
	return g_mst;
}


// check if property map produces shortest tree of a graph on source s
template <typename Property_map, typename Graph>
bool is_shortest(Property_map& property, 
				 const Graph& g, 
				 const typename Graph::vertex_type& s) {
	using V = typename Graph::vertex_type;
	using E = typename Graph::edge_type;
	if (property[s].distance != 0) return false;
	// for every edge, triangle inequality has to be satisfied
	// d(v) <= d(u) + w(u,v)
	Property_map test;
	for (const V& u : g) 
		if (property[u].parent == u) test[u].distance = POS_INF(E);
		// distance of parent + weight of edge getting from parent to child
		else test[u].distance = property[property[u].parent].distance + g.weight(property[u].parent, u);
	test[s].distance = 0;

	for (auto u = g.begin(); u != g.end(); ++u) 
		for (auto v = u.begin(); v != u.end(); ++v) 
			if (test[*u].distance + v.weight() < test[*v].distance) return false;
		
	return true;
}

}