#pragma once

// interface of all graphs (abstract class)
template <typename V>
class Graph {
public:
	virtual size_t vertex() const = 0;
	virtual int edge(const V, const V) const = 0;
	virtual size_t degree(const V) const = 0;
	virtual void add_vertex(const V) = 0;
	virtual void add_edge(const V, const V, int) = 0;
};