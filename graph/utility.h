#pragma once
#include <deque>
#include <limits>
#include <map>
#include <set>
#include <unordered_set>
#include <vector>
#include "search.h"		// dfs used in many algorithms
#include "../heap.h"	// used for piority queue
#include "../../algo/macros.h"	// POS_INF

namespace sal {
// for directed acyclic graph (dag)
// orders all vertices so that parent vertices are always before children
// if vertices are events, then sorting gives one possible sequence of events
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
		using E = typename Property_map::mapped_type::edge_type;
		for (auto v : finish_order) 
			property[v] = {POS_INF(E), 0, v};

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


// Prim's algorithm for minimum spanning tree
// connected undirected graph, assuming positive weight
template <typename V, typename E = size_t>
struct Prim_vertex {
	using edge_type = E;
	V parent;
	E min_edge;
	Prim_vertex() = default;
	Prim_vertex(V v) : parent{v}, min_edge{POS_INF(E)} {}
	E edge() const {return min_edge;}
};
template <typename Property_map>
struct Prim_cmp {
	using V = typename Property_map::key_type;
	const Property_map& property;
	Prim_cmp(const Property_map& p) : property(p) {}

	bool operator()(const V& u, const V& v) const {
		return property.find(u)->second.min_edge < property.find(v)->second.min_edge; 
	}
};

// MST property map
template <typename Graph>
using MPM = std::map<typename Graph::vertex_type, Prim_vertex<typename Graph::vertex_type, typename Graph::edge_type>>;

struct MST_visitor : public BFS_visitor {
	// relaxes an edge if it meets certain requirements
	template <typename Property_map, typename Queue>
	void relax(Property_map& property, Queue& exploring, 
		const Edge<typename Property_map::key_type, 
				   typename Property_map::mapped_type::edge_type>& edge) {
		// const Edge<typename Property_map::key_type>& edge, Op addition = []{}) {
		size_t d_i {exploring.key(edge.dest())};
		// d_i == 0 means not in exploring
		if (d_i && edge.weight() < property[edge.dest()].min_edge) {
			property[edge.dest()].min_edge = edge.weight();
			property[edge.dest()].parent = edge.source();
			// fix heap property
			exploring.check_key(d_i);
		}		
	}

};

// can be made faster to O(E) time if edge weights are integers using an array as the exploring
// with each slot holding doubly linked list of vertices with that min_edge
template <typename Graph>
MPM<Graph> min_span_tree(const Graph& g) {
	using V = typename Graph::vertex_type;
	using E = typename Graph::edge_type;
	using Cmp = Prim_cmp<MPM<Graph>>;
	MST_visitor visitor;
	// property map of each vertex to their min_edge
	MPM<Graph> property;
	// comparator querying on min_edge
	Cmp cmp {property};
	Heap<V, Cmp> exploring {cmp};

	for (V v : g) property[v] = Prim_vertex<V,E>{v};
	// let root of MST be the smallest vertex by name
	V root {*g.begin()};
	property[root].min_edge = 0;

	// batch insert is O(n) rather than O(nlgn) of inserting each sequentially
	exploring.batch_insert(g.begin(), g.end());

	while (!exploring.empty()) {
		V u {exploring.extract_top()};
		// for each adjacent vertex
		auto edges = g.adjacent(u);
		for (auto v = edges.first; v != edges.second; ++v) {
			// relaxation
			visitor.relax(property, exploring, {u, v});
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

}