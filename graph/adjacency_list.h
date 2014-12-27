#pragma once
#include <iostream>
#include <initializer_list>
#include <map>
#include <set>

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
template <typename Iter>
struct Adjacent_iterator {
	using CR = const Adjacent_iterator<Iter>&;
	using V = typename Iter::value_type::first_type;
	using E = typename Iter::value_type;
	Iter cur;

	void operator++() {++cur;}
	void operator--() {--cur;}
	E& operator*() {return *cur;}
	E* operator->() {return &(*cur);}
	bool operator==(CR other) {return other.cur == cur;}
	bool operator!=(CR other) {return !(*this == other);}
	V& dest() {return cur->first;}
	int& weight() {return cur->second;}
};
template <typename Iter>
struct Adjacent_const_iterator {
	using CR = const Adjacent_const_iterator<Iter>&;
	using V = typename Iter::value_type::first_type;
	using E = typename Iter::value_type;
	Iter cur;

	void operator++() {++cur;}
	void operator--() {--cur;}
	E operator*() {return *cur;}
	const E* operator->() {return &(*cur);}
	bool operator==(CR other) {return other.cur == cur;}
	bool operator!=(CR other) {return !(*this == other);}
	V dest() {return cur->first;}
	int weight() {return cur->second;}
};
// iterate over vertices of a graph
// for adjacency lists just a wrapper around map's iterator
template <typename Iter>
struct Vertex_iterator {
	using CR = const Vertex_iterator<Iter>&;
	using V = typename Iter::value_type::first_type;
	using Adj_iter = Adjacent_iterator<typename Iter::value_type::second_type::iterator>;
	Iter cur;

	void operator++() {++cur;}
	void operator--() {--cur;}
	V operator*() {return cur->first;}
	bool operator==(CR other) {return other.cur == cur;}
	bool operator!=(CR other) {return !(*this == other);}
	std::pair<Adj_iter, Adj_iter> adjacent() {
		return {{cur->second.begin()}, {cur->second.end()}};
	}
};
template <typename Iter>
struct Vertex_const_iterator {
	using CR = const Vertex_const_iterator<Iter>&;
	using V = typename Iter::value_type::first_type;
	using Adj_citer = Adjacent_const_iterator<typename Iter::value_type::second_type::const_iterator>;
	Iter cur;

	void operator++() {++cur;}
	void operator--() {--cur;}
	V operator*() {return cur->first;}
	bool operator==(CR other) {return other.cur == cur;}
	bool operator!=(CR other) {return !(*this == other);}
	std::pair<Adj_citer, Adj_citer> adjacent() const {
		return {{cur->second.begin()}, {cur->second.end()}};
	}
};


// undirected weighted graph
template <typename V = int, typename Edges = std::map<V, int>>
class Adjacency_list {
protected:
	// destination and weight
	// internally vertices can be integers/indices since they
	// can be labeled vertex 0, vertex 1, vertex 2, ...
	using Adj = std::map<V, Edges>;
	Adj adj;
public:
	using Adj_iter = Adjacent_iterator<typename Edges::iterator>;
	using Adj_citer = Adjacent_const_iterator<typename Edges::const_iterator>;
	using V_iter = Vertex_iterator<typename Adj::iterator>;
	using V_citer = Vertex_const_iterator<typename Adj::const_iterator>;
	// constructors
	Adjacency_list() = default;
	Adjacency_list(const std::initializer_list<std::pair<V, V>>& l) {
		for (auto& pair : l) {
			adj[pair.first][pair.second] = 1;
			adj[pair.second][pair.first] = 1;
		}
	}
	template <typename Iter_edgelist>
	Adjacency_list(Iter_edgelist begin, const Iter_edgelist end) {
		while (begin != end) {
			adj[begin->source][begin->dest] = begin->get_weight();
			adj[begin->dest][begin->source] = begin->get_weight();
			++begin;
		}
	}
	~Adjacency_list() = default;

	// cardinality of vertex set and edge set
	size_t num_vertex() const {return adj.size();}
	virtual size_t num_edge() const {
		size_t edges {};
		for (auto v = adj.begin(); v != adj.end(); ++v)
			edges += v->second.size();
		return edges >> 1;	// divide by 2 for unweighted
	} 

	// check for existence of vertex and edge
	bool vertex(V v) const {return (adj.find(v) != adj.end());}
	bool edge(V u, V v) const {
		auto u_itr = adj.find(u);
		if (u_itr == adj.end()) return false;
		auto v_itr = u_itr->second.find(v);
		return !(v_itr == u_itr->second.end());
	}

	// weight of edge, 1/0 for unweighted edge
	int weight(V u, V v) const {
		auto u_itr = adj.find(u);
		if (u_itr == adj.end()) return 0;
		auto v_itr = u_itr->second.find(v);
		return (v_itr == u_itr->second.end())? 0 : v_itr->second;		
	}
	size_t degree(V v) const {
		auto v_itr = adj.find(v); 
		return (v_itr == adj.end())? 0 : v_itr->second.size();
	}
	// begin and end
	std::pair<Adj_iter, Adj_iter> adjacent(V v) {
		auto v_itr = adj.find(v);
		if (v_itr != adj.end()) 
			return {{v_itr->second.begin()}, {v_itr->second.end()}};
		else return {{},{}};
	}	
	std::pair<Adj_citer, Adj_citer> adjacent(V v) const {
		auto v_itr = adj.find(v);
		if (v_itr != adj.end()) 
			return {{v_itr->second.begin()}, {v_itr->second.end()}};
		else return {{},{}};
	}
	// vertex iteration
	V_iter begin() 			{return {adj.begin()};}
	V_iter end() 			{return {adj.end()};}
	V_citer begin() const 	{return {adj.begin()};}
	V_citer end() const 	{return {adj.end()};}


	V min_vertex() const {
		if (adj.empty()) return {};	// default
		return adj.begin()->first;
	}

	// modifier interface
	void add_vertex(V v) {adj[v];}
	// undirected
	virtual void add_edge(V u, V v, int weight = 1) {
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

// directed adjacency list
template <typename V = int, typename Edges = std::map<V, int>>
class Adjacency_list_directed : public Adjacency_list<V, Edges> {
	using Adjacency_list<V, Edges>::adj;
public:
	using Adj_iter = typename Adjacency_list<V, Edges>::Adj_iter;
	using Adj_citer = typename Adjacency_list<V, Edges>::Adj_citer;
	// constructors
	Adjacency_list_directed() = default;
	Adjacency_list_directed(const std::initializer_list<std::pair<V, V>>& l) {
		for (auto& pair : l) {
			adj[pair.first][pair.second] = 1;
			adj[pair.second];
		}
	}
	template <typename Iter_edgelist>
	Adjacency_list_directed(Iter_edgelist begin, const Iter_edgelist end) {
		while (begin != end) {
			adj[begin->source][begin->dest] = begin->get_weight();
			adj[begin->dest];	// default construct destination vertex
			++begin;
		}
	}
	~Adjacency_list_directed() = default;

	// most operations the same as undirected, degree is outdegree
	size_t num_edge() const override {
		size_t edges {};
		for (auto v = adj.begin(); v != adj.end(); ++v)
			edges += v->second.size();
		return edges;
	} 
	// add edge only to source vertex
	void add_edge(V u, V v, int weight = 1) override {
		adj[u][v] = weight;
		adj[v]; // default construct destination vertex
	}
};


}	// end namespace sal