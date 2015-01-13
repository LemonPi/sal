#pragma once
#include <map>
#include <unordered_map>
#include "common.h"	// edges
#include "../matrix.h"
#include "../../algo/macros.h"

namespace sal {

template <typename V, typename E>
class Adjacency_matrix {};

template <typename E>
struct Matrix_adjacent_iterator {
	using CR = const Matrix_adjacent_iterator<E>&;
	using Mat = Adjacency_matrix<size_t,E>;
	Mat* mat;
	size_t u, v;

	Matrix_adjacent_iterator() = default;
	Matrix_adjacent_iterator(Mat* m, size_t row) : mat{m}, u{row}, v{0} {
		while (!mat->is_edge(u, v)) ++v;
	}

	void operator++() {do {++v;} while (!mat->is_edge(u, v));}
	void operator--() {do {--v;} while (!mat->is_edge(u, v));}
	size_t operator*() const {return v;}
	bool operator==(CR other) {return other.v == v && other.u == u && other.mat == mat;}
	bool operator!=(CR other) {return !(*this == other);}
	size_t dest() const {return v;}
	E& weight() {return mat->weight(u, v);}
	friend std::ostream& operator<<(std::ostream& os, CR itr) {return os << *itr << ' ';}
};
template <typename E>
struct Matrix_adjacent_const_iterator {
	using CR = const Matrix_adjacent_const_iterator<E>&;
	using Mat = Adjacency_matrix<size_t,E>;
	const Mat* mat;
	size_t u, v;

	Matrix_adjacent_const_iterator() = default;
	Matrix_adjacent_const_iterator(const Mat* m, size_t row) : mat{m}, u{row}, v{0} {
		while (!mat->is_edge(u, v)) ++v;
	}

	void operator++() {do {++v;} while (!mat->is_edge(u, v));}
	void operator--() {do {--v;} while (!mat->is_edge(u, v));}
	size_t operator*() const {return v;}
	bool operator==(CR other) {return other.v == v && other.u == u && other.mat == mat;}
	bool operator!=(CR other) {return !(*this == other);}
	size_t dest() const {return v;}
	E weight() const {return mat->weight(u, v);}
	friend std::ostream& operator<<(std::ostream& os, CR itr) {return os << *itr << ' ';}
};

// iterate over vertices of a graph
// for adjacency lists just a wrapper around map's iterator
template <typename E>
struct Matrix_vertex_iterator {
	using CR = const Matrix_vertex_iterator<E>&;
	using adjacent_iterator = Matrix_adjacent_iterator<E>;
	using Mat = Adjacency_matrix<size_t,E>;
	Mat* mat;
	size_t u;

	void operator++() {++u;}
	void operator--() {--u;}
	CR operator+(int scalar) const {return {mat, u + scalar};}
	CR operator-(int scalar) const {return {mat, u - scalar};}
	size_t operator*() const {return u;}
	bool operator==(CR other) {return other.u == u && other.mat == mat;}
	bool operator!=(CR other) {return !(*this == other);}
	E& edge(size_t v) {return mat->get(u, v);}
	std::pair<adjacent_iterator, adjacent_iterator> adjacent() {
		return {{mat, u}, {mat, u, mat->num_vertex()}};
	}
	adjacent_iterator begin() 	{return {mat, u};}
	adjacent_iterator end() 	{return {mat, u, mat->num_vertex()};}	// index past end
	friend std::ostream& operator<<(std::ostream& os, CR itr) {return os << *itr << ' ';}
};
template <typename E>
struct Matrix_vertex_const_iterator {
	using CR = const Matrix_vertex_const_iterator<E>&;
	using adjacent_const_iterator = Matrix_adjacent_const_iterator<E>;
	using Mat = Adjacency_matrix<size_t,E>;
	const Mat* mat;
	size_t u;

	void operator++() {++u;}
	void operator--() {--u;}
	CR operator+(int scalar) const {return {mat, u + scalar};}
	CR operator-(int scalar) const {return {mat, u - scalar};}
	size_t operator*() const {return u;}
	bool operator==(CR other) {return other.u == u && other.mat == mat;}
	bool operator!=(CR other) {return !(*this == other);}
	E edge(size_t v) const {return mat->get(u, v);}
	std::pair<adjacent_const_iterator, adjacent_const_iterator> adjacent() const {
		return {{mat, u}, {mat, u, mat->num_vertex()}};
	}
	adjacent_const_iterator begin() const	{return {mat, u};}
	adjacent_const_iterator end() const		{return {mat, u, mat->num_vertex()};}	// index past end
	friend std::ostream& operator<<(std::ostream& os, CR itr) {return os << *itr << ' ';}
};



// only the version where vertex name is indices 0 .. n-1 is defined
// undirected by default
template <typename E>
class Adjacency_matrix<size_t, E> {
protected:
	// n x n matrix representing each vertex's edges
	Matrix<E> adj;

public:
	using vertex_type = size_t;
	using edge_type = E;
	using iterator = Matrix_vertex_iterator<E>;
	using const_iterator = Matrix_vertex_const_iterator<E>;
	using adjacent_iterator = Matrix_adjacent_iterator<E>;
	using adjacent_const_iterator = Matrix_adjacent_const_iterator<E>;

	Adjacency_matrix() = default;
	Adjacency_matrix(const std::initializer_list<UEdge<size_t>>& l) {
		// have to establish maximum vertex first
		size_t max_vertex {0};
		for (const auto& edge : l) max_vertex = max(max_vertex, max(edge.dest, edge.source));
		adj.resize(max_vertex+1, max_vertex+1, POS_INF(E)); 
		for (const auto& edge : l) {
			adj.get(edge.source, edge.dest) = (E)1;
			adj.get(edge.dest, edge.source) = (E)1;
		}		
	}
	Adjacency_matrix(const std::initializer_list<WEdge<size_t,E>>& l) {
		// have to establish maximum vertex first
		size_t max_vertex {0};
		for (const auto& edge : l) max_vertex = max(max_vertex, max(edge.dest, edge.source));
		adj.resize(max_vertex+1, max_vertex+1, POS_INF(E)); 
		for (const auto& edge : l) {
			adj.get(edge.source, edge.dest) = edge.weight;
			adj.get(edge.dest, edge.source) = edge.weight;
		}		
	}	
	template <typename Iter_edgelist>
	Adjacency_matrix(Iter_edgelist begin, const Iter_edgelist end, size_t v_num) 
	: adj{v_num, v_num, POS_INF(E)} {
		while (begin != end) {
			adj.get(begin->source, begin->dest) = begin->get_weight();
			adj.get(begin->dest, begin->source) = begin->get_weight();
			++begin;
		}		
	}

	// cardinality of vertex set
	size_t num_vertex() const {return adj.row();}
	virtual size_t num_edge() const {
		size_t edges {};
		for (size_t edge = 0; edge < adj.row() * adj.col(); ++edge) edges += (edge != POS_INF(E));
		return edges >> 1;	// divide by 2 for undirected
	}

	// check existence of vertex and edge
	bool is_vertex(size_t v) const {return v < adj.row();}
	bool is_edge(size_t u, size_t v) const {
		if (u < adj.row() && v < adj.row())
			return adj.get(u, v) != POS_INF(E);
		return false;
	}

	// weight of edge, 1/0 for unweighted edge, assumes edge exists
	E& weight(size_t u, size_t v) 	   {return adj.get(u, v);}
	E weight(size_t u, size_t v) const {return adj.get(u, v);}
	// out degree, assumes vertex exists
	size_t degree(size_t v) const {
		size_t res {0};
		for (size_t col = 0; col < adj.col(); ++col)
			res += (adj.get(v, col) == POS_INF(E))? 0 : 1;
		return res;
	}

	std::pair<adjacent_iterator, adjacent_iterator> adjacent(size_t v) {
		if (v < adj.row()) 
			return {{this, v}, {this, v, adj.cols()}};
		else return {{},{}};
	}	
	std::pair<adjacent_const_iterator, adjacent_const_iterator> adjacent(size_t v) const {
		if (v < adj.row()) 
			return {{this, v}, {this, v, adj.cols()}};
		else return {{},{}};
	}

	// retrieving vertex (find by default gives vertex)
	iterator vertex(size_t v) 		 		{return {this, v};}
	iterator find(size_t v) 		 		{return {this, v};}
	const_iterator vertex(size_t v) const 	{return {this, v};}	
	const_iterator find(size_t v) const 	{return {this, v};}

	// vertex iteration
	iterator begin() 				{return {this, 0};}
	iterator end() 					{return {this, adj.row()};}
	const_iterator begin() const	{return {this, 0};}
	const_iterator end() const 		{return {this, adj.row()};}

	// vertex numbered by order inserted
	void add_vertex(size_t u) {
		adj.resize(adj.row() + 1, adj.col() + 1, POS_INF(E));
	}

	// int weight, -1 is deleted edge
	virtual void add_edge(size_t u, size_t v, int weight = 1) {
		if (u >= adj.row() || v >= adj.row()) 
			adj.resize(max(u,v)+1, max(u,v)+1, POS_INF(E));  
		adj.get(u, v) = weight;
		adj.get(v, u) = weight;
	}

	size_t min_vertex() const {return 0;}
	size_t max_vertex() const {return adj.row() - 1;}


	friend ostream& operator<<(ostream& os, const Adjacency_matrix& g) {
		os << g.adj;
		// for (size_t v = 0; v < g.num_vertex(); ++v) {
		// 	os << v << '(';
		// 	bool empty {true};
		// 	for (size_t neighbour = 0; neighbour < g.num_vertex(); ++neighbour) 
		// 		if (g.edge(v, neighbour) > 0) { os << neighbour << ':' << g.edge(v, neighbour) << ','; empty = false; }
		// 	if (!empty) os << '\b';
		// 	os << ") ";
		// }
		return os;
	}
};
template <typename E>
class Adjacency_matrix_directed : public Adjacency_matrix<size_t,E> {
	// n x n matrix representing each vertex's edges
	using Adjacency_matrix<size_t,E>::adj;

public:
	using vertex_type = size_t;
	using edge_type = E;
	using iterator = typename Adjacency_matrix<size_t,E>::iterator;
	using const_iterator = typename Adjacency_matrix<size_t,E>::const_iterator;
	using adjacent_iterator = typename Adjacency_matrix<size_t,E>::adjacent_iterator;
	using adjacent_const_iterator = typename Adjacency_matrix<size_t,E>::adjacent_const_iterator;

	Adjacency_matrix_directed() = default;
	Adjacency_matrix_directed(const std::initializer_list<UEdge<size_t>>& l) {
		// have to establish maximum vertex first
		size_t max_vertex {0};
		for (const auto& edge : l) max_vertex = max(max_vertex, max(edge.dest, edge.source));
		adj.resize(max_vertex+1, max_vertex+1, POS_INF(E)); 
		for (const auto& edge : l) {
			adj.get(edge.source, edge.dest) = (E)1;
		}		
	}
	Adjacency_matrix_directed(const std::initializer_list<WEdge<size_t,E>>& l) {
		// have to establish maximum vertex first
		size_t max_vertex {0};
		for (const auto& edge : l) max_vertex = max(max_vertex, max(edge.dest, edge.source));
		adj.resize(max_vertex+1, max_vertex+1, POS_INF(E)); 
		for (const auto& edge : l) {
			adj.get(edge.source, edge.dest) = edge.weight;
		}		
	}	
	template <typename Iter_edgelist>
	Adjacency_matrix_directed(Iter_edgelist begin, const Iter_edgelist end, size_t v_num) {
		adj.resize(v_num, v_num, POS_INF(E));
		while (begin != end) {
			adj.get(begin->source, begin->dest) = begin->get_weight();
			++begin;
		}		
	}

	// cardinality of vertex set
	size_t num_edge() const override {
		size_t edges {};
		for (size_t edge = 0; edge < adj.row() * adj.col(); ++edge) edges += (edge != POS_INF(E));
		return edges;
	}

	// int weight, -1 is deleted edge
	void add_edge(size_t u, size_t v, int weight = 1) override {
		if (u >= adj.row() || v >= adj.row()) 
			adj.resize(max(u,v)+1, max(u,v)+1, POS_INF(E));  
		adj.get(u, v) = weight;
	}

	friend ostream& operator<<(ostream& os, const Adjacency_matrix_directed& g) {
		os << g.adj;
		// for (size_t v = 0; v < g.num_vertex(); ++v) {
		// 	os << v << '(';
		// 	bool empty {true};
		// 	for (size_t neighbour = 0; neighbour < g.num_vertex(); ++neighbour) 
		// 		if (g.edge(v, neighbour) > 0) { os << neighbour << ':' << g.edge(v, neighbour) << ','; empty = false; }
		// 	if (!empty) os << '\b';
		// 	os << ") ";
		// }
		return os;
	}
};

}