#pragma once
#include <vector>
#include <set>
#include "shortest.h"
// linear programming involving graphs

namespace sal {

// difference constraints
// of the form x[j] - x[i] <= limit
// applications involve event ordering (weighted topoogical sort almost)
template <typename T>
struct Constraint {
	size_t j, i;
	T limit;
	Constraint() = default;
	Constraint(size_t first, size_t second, const T& l) : j{first}, i{second}, limit{l} {}
	bool operator==(const Constraint& c) const {return (j == c.j) && (i == c.i) && (limit == c.limit);}
};
// hash on the index j and i (should be unique, so limit is not needed)
template <typename T>
struct Constraint_hash {
	size_t operator()(const Constraint<T>& c) const {
		return std::hash<size_t>()(c.j) ^ std::hash<size_t>()(c.i);
	}
};


template <typename T>
using Constraint_sys = std::unordered_set<Constraint<T>, Constraint_hash<T>>;
template <typename T>
using Constraint_sol = std::vector<T>;

// in Ax <= b problem, b is input limits, x will be returned, A is implicit in constraints
// returns a feasible solution, or empty if infeasible
// solution will have minimized mean and variance
template <typename T>
Constraint_sol<T> feasible(const Constraint_sys<T>& constraints, size_t sol_size) {
	digraph<size_t,T> g;	// constraint graph
	for (const auto& c : constraints)
		g.add_edge(c.i, c.j, c.limit);
	// assume indices given in 1 2 .. n
	// generate source vertex that's not in graph
	// can get rid of adding source vertex if initial shortest distance is 0 instead of POS_INF
	size_t s {POS_INF(size_t)};
	while(g.is_vertex(s)) s >>= 1;
	// add 0-edges to all vertices
	for (size_t v : g)
		g.add_edge(s, v, 0);
	// run bellman ford to solve system
	auto solution = bellman_ford(g, s);
	// empty implies negative cycle exists
	if (solution.empty()) return {};
	solution.erase(s);	// remove temporary source (not actual data)

	Constraint_sol<T> res; res.reserve(sol_size);
	for (size_t i = 1; i <= sol_size; ++i) res.push_back(solution[i].distance);
	return res;
}

// can use to maximize profit of arbitrage (currency trading)
// each currency is a vertex, each edge is conversion
// R[i1,i2] * R[i2,i3] * R[i3,i4] * .. > 1
// iff 1/R[i1,i2] * 1/R[i2,i3] * 1/R[i3,i4] * .. < 1
// or lg(1/R[i1,i2]) + lg(1/R[i2,i3]) + lg(1/R[i3,i4]) .. < 0
// so edge weight is lg(1/R[i,j]) = -lg(R[i,j])

// if negative cycle exists, then there is profitable cycle
// to get cycle, relax all edges once more
// then follow parents until a vertex whose distance changed in the last cycle


}	// end namespace sal