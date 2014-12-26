#pragma once
#include <iostream>
#include <map>
#include <set>
#include "interface.h"

namespace sal {

template <typename V>
struct Dedge {
	V source;
	V dest;
	int get_weight() const {return 1;}
};

template <typename V>
struct Wedge {
	V dest;
	int weight;
	int get_weight() const {return weight;}
};
template <typename V>
bool operator<(const Wedge<V>& a, const Wedge<V>& b) {
	return a.dest < b.dest;
}

template <typename V>
struct Dwedge {
	V source;
	V dest;
	int weight;
	int get_weight() const {return weight;}
};

// undirected weighted graph
template <typename V = int, typename Edges = std::map<V, int>>
class Adjacency_list : public Graph<V> {
	// destination and weight
	// internally vertices can be integers/indices since they
	// can be labeled vertex 0, vertex 1, vertex 2, ...
	std::map<V, Edges> adj;
public:
	// constructors
	Adjacency_list() = default;
	template <typename Iter_edgelist>
	Adjacency_list(Iter_edgelist begin, const Iter_edgelist end) {
		while (begin != end) {
			adj[begin->source][begin->dest] = begin->get_weight();
			adj[begin->dest][begin->source] = begin->get_weight();
			++begin;
		}
	}
	~Adjacency_list() = default;

	size_t vertex() const override {
		return adj.size();
	}
	// weight of edge, 1/0 for unweighted edge
	int edge(const V u, const V v) const override {
		auto u_itr = adj.find(u);
		if (u_itr == adj.end()) return 0;
		auto v_itr = u_itr->second.find(v);
		return (v_itr == u_itr->second.end())? 0 : v_itr->second;
	}
	size_t degree(const V v) const override {
		auto v_itr = adj.find(v); 
		return (v_itr == adj.end())? 0 : v_itr->second.size();
	}

	// modifier interface
	void add_vertex(const V v) override {
		adj[v];
	}
	// undirected
	void add_edge(const V u, const V v, int weight = 1) override {
		adj[u][v] = weight;
		adj[v][u] = weight;
	}

	// printing
	friend ostream& operator<<(ostream& os, const Adjacency_list& g) {
		for (const auto& vertex : g.adj) {
			os << vertex.first << '(';
			if (!vertex.second.empty()) {
				for (const auto& neighbour : vertex.second)
					os << neighbour.first << ':' << neighbour.second << ',';
				// backspace to remove extra comma
				os << '\b';
			}
			os << ") ";
		}
		return os;
	}
};



}	// end namespace sal