#pragma once
#include <map>
#include <unordered_map>
#include "data/matrix.h"
#include "interface.h"

namespace sal {

// undirected unweighted
class Adjacency_matrix : public Graph<size_t> {
	using Edges = std::map<size_t, int>;
	// n x n matrix representing each vertex's edges
	Matrix<int> adj;
	// vertex name (user defined) to index inside array
	std::unordered_map<size_t, size_t> v_index;

	// unprotected, assume inside range
	int index_edge(size_t u_i, size_t v_i) const {
		return adj.get(u_i, v_i);
	}

public:
	Adjacency_matrix() = default;
	template <typename Iter_edgelist>
	Adjacency_matrix(Iter_edgelist begin, const Iter_edgelist end, size_t v_num) 
	: adj(v_num, v_num) {
		// need to map incoming values to indices
		v_index.reserve(v_num);
		size_t index {0};
		while (begin != end) {
			size_t source = begin->source;
			size_t dest = begin->dest;
			if (v_index.find(source) == v_index.end()) v_index[source] = index++;
			if (v_index.find(dest) == v_index.end()) v_index[dest] = index++; 
			adj.get(v_index[source], v_index[dest]) = begin->get_weight();
			adj.get(v_index[dest], v_index[source]) = begin->get_weight();
			++begin;
		}		
	}

	// selectors
	size_t vertex() const override {
		return adj.row();
	}
	// weight of edge, 1/0 for unweighted edge
	int edge(size_t u, size_t v) const override {
		auto u_itr = v_index.find(u);
		auto v_itr = v_index.find(v);

		if (u_itr == v_index.end() || v_itr == v_index.end()) return 0;
		return adj.get(u_itr->second, v_itr->second);
	}
	size_t degree(size_t v) const override {
		if (v_index.find(v) == v_index.end()) return 0;
		size_t v_i {v_index.find(v)->second};
		size_t res {};
		for (size_t col = 0; col < adj.col(); ++col)
			res += (index_edge(v_i, col) == 0)? 0 : 1;
		return res;
	}
	const Edges adjacent(size_t v) const {
		if (v_index.find(v) == v_index.end()) return {};
		size_t v_i = v_index.find(v)->second;
		Edges neighbours;
		for (size_t u_i = 0; u_i < vertex(); ++u_i)
			if (index_edge(v_i, u_i) != 0) neighbours[u_i] = index_edge(v_i, u_i);
		return std::move(neighbours);
	}

	// vertex numbered by order inserted
	void add_vertex(const size_t u) {
		v_index[u] = adj.row();
		adj.resize(adj.row() + 1, adj.col() + 1);
	}

	// int weight, -1 is deleted edge
	void add_edge(size_t u, size_t v, int weight = 1) {
		if (v_index.find(u) == v_index.end()) add_vertex(u);
		if (v_index.find(v) == v_index.end()) add_vertex(v);
		adj.get(v_index[u], v_index[v]) = weight;
		adj.get(v_index[v], v_index[u]) = weight;
	}


	friend ostream& operator<<(ostream& os, const Adjacency_matrix& g) {
		os << g.adj;
		// for (size_t v = 0; v < g.vertex(); ++v) {
		// 	os << v << '(';
		// 	bool empty {true};
		// 	for (size_t neighbour = 0; neighbour < g.vertex(); ++neighbour) 
		// 		if (g.edge(v, neighbour) > 0) { os << neighbour << ':' << g.edge(v, neighbour) << ','; empty = false; }
		// 	if (!empty) os << '\b';
		// 	os << ") ";
		// }
		return os;
	}
};

}