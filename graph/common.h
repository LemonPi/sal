#pragma once

namespace sal {
// edge used in algorithms
template <typename V, typename E>
struct Edge {
	using edge_type = E;
	V u, v;
	E w;
	template <typename Adj_iter>
	Edge(V a, Adj_iter& b) : u{a}, v{*b}, w(b.weight()) {}
	V source() const   {return u;}
	V dest() const     {return v;}
	E weight() const   {return w;}
};

// helper edges for constructing graphs
template <typename V, typename E = int>
struct WEdge {
	V source;
	V dest;
	E weight;
	E get_weight() const {return weight;}
	// can't maintain as POD because V might not be POD
	WEdge(V u, V v, E w) : source{u}, dest{v}, weight{w} {} 
};
template <typename V>
struct UEdge {
	V source;
	V dest;
	size_t get_weight() const {return 1;}
	UEdge(V u, V v) : source{u}, dest{v} {} 
};

}