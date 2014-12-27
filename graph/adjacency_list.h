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

// iterate over neighbours of a vertex 
// for adjacency lists just a wrapper around map's iterator
template <typename V, typename Iter>
struct Adjacent_iterator {
	using CR = const Adjacent_iterator<V, Iter>&;
	using E = typename Iter::value_type;
	Iter cur;

	void operator++() {++cur;}
	void operator--() {--cur;}
	E& operator*() {return *cur;}
	E* operator->() {return &(*cur);}
	bool operator==(CR other) {return other.cur == cur;}
	bool operator!=(CR other) {return !(*this == other);}
	const V& dest() {return cur->first;}
	int& weight() {return cur->second;}
};
template <typename V, typename Iter>
struct Adjacent_const_iterator {
	using CR = const Adjacent_const_iterator<V, Iter>&;
	using E = typename Iter::value_type;
	Iter cur;

	void operator++() {++cur;}
	void operator--() {--cur;}
	E operator*() {return *cur;}
	const E* operator->() {return &(*cur);}
	bool operator==(CR other) {return other.cur == cur;}
	bool operator!=(CR other) {return !(*this == other);}
	const V dest() {return cur->first;}
	int weight() {return cur->second;}
};

// undirected weighted graph
template <typename V = int, typename Edges = std::map<V, int>>
class Adjacency_list : public Graph<V> {
	// destination and weight
	// internally vertices can be integers/indices since they
	// can be labeled vertex 0, vertex 1, vertex 2, ...
	std::map<V, Edges> adj;
public:
	using Adj_iter = Adjacent_iterator<V, typename Edges::iterator>;
	using Adj_citer = Adjacent_const_iterator<V, typename Edges::const_iterator>;
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
	// begin and end
	std::pair<Adj_iter, Adj_iter> adjacent(const V v) {
		auto v_itr = adj.find(v);
		if (v_itr != adj.end()) 
			return {{v_itr->second.begin()}, {v_itr->second.end()}};
		else return {{},{}};
	}	
	std::pair<Adj_citer, Adj_citer> adjacent(const V v) const {
		auto v_itr = adj.find(v);
		if (v_itr != adj.end()) 
			return {{v_itr->second.begin()}, {v_itr->second.end()}};
		else return {{},{}};
	}

	const V min_vertex() const {
		if (adj.empty()) return 0;
		return adj.begin()->first;
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