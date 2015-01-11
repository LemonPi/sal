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


}	// end namespace sal