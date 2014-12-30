#pragma once
#include <deque>
#include "search.h"

namespace sal {
// for directed acyclic graph (dag)
// orders all vertices so that parent vertices are always before children
// if vertices are events, then sorting gives one possible sequence of events
template <typename Output_iter>
struct Topological_visitor : public Graph_visitor {
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

struct Cycle_visitor : public Graph_visitor {
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
	}
	return std::move(g_t);
} 

// for directed graphs, strongly connected components
// any vertex in a component can get to any other
// simple algorithm by guessing a vertex in a sink component, 
// then traversing full cycle to get entire component
// no easy way to get sink vertex, so use source vertices of the transpose
template <typename Output_iter>
struct Inorder_finish_visitor : public Graph_visitor {
	Output_iter out;
	Inorder_finish_visitor(Output_iter o) : out{o} {}

	template <typename Graph>
	void finish_vertex(typename Graph::vertex_type u, const Graph&) {*out++ = u;}

};

template <typename Graph>
using Connected_set =  std::vector<std::unordered_set<typename Graph::vertex_type>>;

template <typename Graph>
struct Connected_visitor : public Graph_visitor {
	using V = typename Graph::vertex_type;
	Connected_set<Graph> component_set;
	std::vector<V> finish_order;
	Connected_visitor(std::vector<V>&& vec) : finish_order{vec} {}

	// visit in order of descending finish time 
	template <typename Property_map>
	std::vector<V> initialize_vertex(Property_map& property, const Graph& g) {
		for (auto v : finish_order) 
			property[v] = {unsigned_infinity, 0, v};

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

}