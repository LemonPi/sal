#pragma once
#include <queue>
#include <unordered_map>
#include "adjacency_list.h"

namespace sal {

// assumes non-negative weighted edges, O((V+E)lgV) with binary heap (priority queue)
template <typename Graph, typename V>
size_t dijkstra(const Graph& g, V s, V x) {
	return 0;
}

// assumes unweighted graph, and assumes V is simple type (name of vertex)
// works with directed and undirected
enum class Status {UNEXPLORED, EXPLORING, EXPLORED};

template <typename V>
struct Vertex_property {
	// distance estimate of s to vertex, always >= distance
	// after completion, estimate is == distance
	size_t distance;
	// flattened tree to trace back path, unnecessary if only path weight needed
	V parent;
	Status status;
};

template <typename Graph, typename V>
size_t bfs(const Graph& g, V s, V x) {
	std::unordered_map<V, Vertex_property<V>> property;
	for (auto v = g.begin(); v != g.end(); ++v) {
		auto& v_p = property[*v];
		v_p.distance = std::numeric_limits<size_t>::max();
		v_p.status = Status::UNEXPLORED;
	}
	auto& s_p = property[s];
	s_p.distance = 0;
	s_p.parent = s;
	s_p.status = Status::EXPLORING;

	std::queue<V> exploring;
	exploring.push(s);
	while (!exploring.empty()) {
		V cur {exploring.front()};
		exploring.pop();

		// explore adjacent to current
		auto edges = g.adjacent(cur);
		for (auto neighbour = edges.first; neighbour != edges.second; ++neighbour) {
			auto& n_p = property[neighbour.dest()];
			if (n_p.status == Status::UNEXPLORED) {
				n_p.distance = property[cur].distance + 1;
				n_p.parent = cur;
				n_p.status = Status::EXPLORING;
				exploring.push(neighbour.dest());
			}
		}
		property[cur].status = Status::EXPLORED;
	}

	return property[x].distance;
}

}