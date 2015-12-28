#include <cassert>
namespace sal {

// disjoint set with path compression and union by rank
// nodes have integers as their key; can externally map that to actual data via vector
class Disjoint_set {
	struct Node {
		int parent;		// index of parent inside nodes; allows trace up to root
		int key;		// index inside nodes; also allows users to map to actual data
		int rank;		// upper bound on height of node in subtree
		Node(int p, int k, int r) : parent(p), key(k), rank(r) {}	// constructor only because compilers do not support POD emplace 
	};
	std::vector<Node> nodes;

	void link(int x, int y) {	// link 2 representatives
		if (x == -1 || y == -1 || x == y)
			return;

		Node& set_x = nodes[x];
		Node& set_y = nodes[y];

		if (set_x.rank > set_y.rank) {
			set_y.parent = x;
		}
		else {
			set_x.parent = y;
			if (set_x.rank == set_y.rank)
				set_y.rank += 1;
		}
	}

	// recursive path compressed find
	int find_helper(int x) {
		if (nodes[x].key != nodes[x].parent) {
			nodes[x].parent = find_helper(nodes[x].parent);
		}
		return nodes[x].parent;
	}

public:
	using value_type = int;
	using node_type = Node;


	Disjoint_set() = default;
	Disjoint_set(int elems) {	// elems number of sets with single element
		assert(elems >= 0);

		nodes.reserve(elems);
		for (int i = 0; i < elems; ++i) {
			nodes.emplace_back(i, i, 0);	// key is index and parent is itself; 0 rank
		}
	}	
	~Disjoint_set() = default;	// data is stored inside nodes

	// if x is not a set, return -1; else return key of representative of the set x is in
	int findset(int x) {	// not const since path compression changes state
		if (x < 0 || x >= (int)nodes.size())
			return -1;

		return find_helper(x);
	}

	void join(int x, int y) {	// union/join 2 sets (cannot use union since it is a C++ keyword)
		link(findset(x), findset(y));
	}

	// insert elems number of new elements to the end
	void insert(int elems) {
		assert(elems >= 0);
		size_t prev_size = nodes.size();
		for (int i = 0; i < elems; ++i) {
			nodes.emplace_back(prev_size + i, prev_size + i, 0);
		}
	}

	size_t size() const {	// number of nodes, not number of sets
		return nodes.size();
	}
};

}	// end namespace sal