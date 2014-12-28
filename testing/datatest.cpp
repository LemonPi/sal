#include <algorithm>
#include <iostream>
#include "data/matrix.h"
#include "data/heap.h"
#include "data/tree.h"
#include "data/list.h"
#include "data/interval.h"
#include "data/graph.h"
#include "data/graph/search.h"

using namespace std;
using namespace sal;
void test_list() {
	Basic_list<int> l {1, 4, 2, 5, 3, 7, 6};
	std::cout << l;
}

void test_mul() {
	Matrix<int> A {{2, 5, 6},
				   {3, 4, -3},
				   {7, 8, 0}};
	Matrix<int> B {{-1, 1},
				   {5, -2},
				   {4, 2}};
	if (A*B != Matrix<int>{{47, 4},{5,-11},{33,-9}}) 
		cout << "FAILED...Matrix multiplication\n";
	
}

void test_pow() {
	Matrix<int> F {{1, 1},
				   {1, 0}};
	if (F.pow(5) != Matrix<int>{{8, 5}, {5, 3}}) 
		cout << "FAILED...Matrix exponentiation\n";
}

void test_heap() {
	Heap<int> h {3, 4, 6, 5, 1, 8, 11, 12};
	if (!h.is_maxheap()) cout << "FAILED...Heap construction\n";
}

void test_tree() {

	using Node = sal::Basic_node<int>;
	sal::Basic_tree<int> t {5, 3, 7, 1, 9, 4, 2, 0, 10, 8, 6};

	auto nil = t.end();
	if (static_cast<int>(nil->color) != 0) 
		std::cout << "FAILED...Basic_tree nil\n";

	auto node = t.find(4);
	if (node == t.end()) std::cout << "FAILED...Basic_tree find\n";

	t.erase(4);
	node = t.find(4);
	if (node != t.end()) std::cout << "FAILED...Basic_tree erase\n";

	t.insert(5);
	// testing iterators with standard library functions
	for_each(t.begin(), t.end(), 
		[](const Node& node){std::cout << node.key << ' ';});
}

void test_order_tree() {
	sal::Order_tree<int> t {5, 3, 7, 1, 9, 4, 2, 0, 10, 8, 6};

	auto node = t.find(4);
	if (node == t.end()) std::cout << "FAILED...Order tree find\n";

	t.erase(4);
	node = t.find(4);
	if (node != t.end()) std::cout << "FAILED...Order tree erase\n";

	t.insert(5);

	int rank {4};
	node = t.select(rank);
	if (node == t.end() || node->key != 3) std::cout << "FAILED...Order tree select\n";
}

void test_treap() {
	sal::Basic_treap<int> t {5, 3, 7, 1, 9, 4, 2, 0, 10, 8, 6};

	auto node = t.find(4);
	if (node == t.end()) std::cout << "FAILED...Tree find\n";

	t.erase(4);
	node = t.find(4);
	if (node != t.end()) std::cout << "FAILED...Tree erase\n";

	t.insert(5);
	// attempt to elevate 4's priority
	t.insert(4);
	for (int i = 0; i < 20; ++i) t.find(4);
}

void test_interval_set() {
	sal::Interval_set<int> t {{16,21}, {8,9}, {5,8}, {15,23}, {25,30}, {0, 3}, {6, 10}, {17,19}, {26,26}, {19,20}};

	auto interval = t.find({22, 25});
	if (*interval != Interval<int>{15,23}) std::cout << "FAILED...Interval set find\n";

	interval = t.find(11, 14);
	if (interval != t.end()) std::cout << "FAILED...Interval set find\n";
}

void test_undirected_graph() {
	using Edge = sal::Dedge<int>;
	vector<Edge> edge_list {{5,1},{5,4},{5,10},{1,4},{4,10}};
	sal::graph<int> g {edge_list.begin(), edge_list.end()};

	if (g.weight(5, 1) != 1 || g.weight(5, 2) != 0 || g.weight(4, 1) != 1) 
		std::cout << "FAILED...Undirected unweighted graph edge\n";

	if (g.degree(3) != 0 || g.degree(5) != 3 || g.degree(4) != 3 || g.degree(2) != 0)
		std::cout << "FAILED...Undirected unweighted graph degree\n";
		
	// weighted
	vector<sal::Dwedge<int>> wedge_list {{5,1,2}, {5,4,1}, {5,10,3}, {1,4,6}, {4,10,5}};
	sal::graph<int> w {wedge_list.begin(), wedge_list.end()};

	if (w.weight(5, 1) != 2 || w.weight(5, 2) != 0 || w.weight(4, 1) != 6) 
		std::cout << "FAILED...Undirected weighted graph edge\n";

	if (w.degree(3) != 0 || w.degree(5) != 3 || w.degree(4) != 3 || w.degree(2) != 0)
		std::cout << "FAILED...Undirected weighted graph degree\n";	

	// adjacency iterator
	auto edges = w.adjacent(5);
	// alter the weight
	for (auto v = edges.first; v != edges.second; ++v)
		v.weight() = 1;
	// constant iteration
	for (auto v = edges.first; v != edges.second; ++v)
		if (v.weight() != 1) std::cout << "FAILED...graph iteration\n";

	// adjacency matrix
	sal::graph_alt w_alt {wedge_list.begin(), wedge_list.end(), 4};
	if (w_alt.weight(5, 1) != 2 || w_alt.weight(5, 2) != 0 || w_alt.weight(4, 1) != 6) 
		std::cout << "FAILED...Alternative undirected weighted graph edge\n";

	if (w_alt.degree(3) != 0 || w_alt.degree(5) != 3 || w_alt.degree(4) != 3 || w.degree(2) != 0)
		std::cout << "FAILED...Alternative undirected weighted graph degree\n";		
}

void test_directed_graph() {
	using Edge = sal::Dedge<int>;
	vector<Edge> edge_list {{5,1},{5,4},{5,10},{1,4},{4,10}};
	sal::digraph<int> g {edge_list.begin(), edge_list.end()};

	if (g.weight(5, 1) != 1 || g.weight(5, 2) != 0 || g.weight(4, 1) != 0) 
		std::cout << "FAILED...Directed unweighted graph edge\n";

	if (g.degree(3) != 0 || g.degree(5) != 3 || g.degree(4) != 1 || g.degree(10) != 0)
		std::cout << "FAILED...Directed unweighted graph degree\n";

	// weighted
	vector<sal::Dwedge<int>> wedge_list {{5,1,2}, {5,4,1}, {5,10,3}, {1,4,6}, {4,10,5}};
	sal::digraph<int> w {wedge_list.begin(), wedge_list.end()};

	if (w.weight(5, 1) != 2 || w.weight(5, 2) != 0 || w.weight(4, 1) != 0 || w.weight(1, 4) != 6 || w.weight(4, 10) != 5) 
		std::cout << "FAILED...Directed weighted graph edge\n";

	if (w.degree(3) != 0 || w.degree(4) != 1 || w.degree(10) != 0 || w.degree(1) != 1)
		std::cout << "FAILED...Directed weighted graph degree\n";	

	// adjacency iterator
	auto edges = w.adjacent(5);
	// alter the weight
	for (auto v = edges.first; v != edges.second; ++v)
		v.weight() = 1;
	// constant iteration
	for (auto v = edges.first; v != edges.second; ++v)
		if (v.weight() != 1) std::cout << "FAILED...Graph adjacent iteration\n";

	// non-existent edges
	edges = w.adjacent(2);
	bool ghost_print {false};
	for (auto v = edges.first; v != edges.second; ++v)
		ghost_print = true;
	if (ghost_print) std::cout << "FAILED...Graph adjacent iteration (non-existent edge)\n";

	// vertex iteration
	for (auto v = w.begin(); v != w.end(); ++v)
		if (!w.vertex(*v)) std::cout << "FAILED...Graph vertex iteration\n";

}

void test_bfs() {
	sal::graph<char> d {{'v','r'},{'r','s'},{'s','w'},{'w','t'},{'t','x'},{'w','x'},{'t','u'},
						{'x','u'},{'x','y'},{'u','y'}};

	// property breadth first tree linked by each element's parent attribute
	auto property = sal::bfs(d, 's');
	for (char parent = 'u'; ; parent = property[parent].parent) {
		std::cout << parent;
		if (parent == 's') {std::cout << std::endl; break;}
		std::cout << " <- ";
	}
	if (property['u'].distance != 3) std::cout << "FAILED...Breadth first search\n";
}

void test_dfs() {
	sal::digraph<char> e {{'u','v'},{'u','x'},{'x','v'},{'v','y'},{'y','x'},
						{'w','y'},{'w','z'},{'z','z'}};
	auto dfs_property = sal::dfs(e);
	std::map<char, std::pair<size_t, size_t>> correct_timestamps;
	correct_timestamps['u'] = {1, 8};
	correct_timestamps['v'] = {2, 7};
	correct_timestamps['w'] = {9, 12};
	correct_timestamps['x'] = {4, 5};
	correct_timestamps['y'] = {3, 6};
	correct_timestamps['z'] = {10, 11};
	for (auto& pair : dfs_property) {
		if (correct_timestamps[pair.first] !=  std::make_pair(pair.second.start, pair.second.finish))
			std::cout << "FAILED...Depth first search\n";
	}
}

void test_matrix() {
	Matrix<int> id3 {identity<int>(3)};
	std::cout << id3 << std::endl;
	id3.resize(4,5);
	std::cout << id3 << std::endl;
	id3.resize(2,2);
	std::cout << id3 << std::endl;
	// test_mul();
	// test_pow();
}

int main() {
	// test_heap();
	// test_tree();
	// test_order_tree();
	// test_interval_set();
	// test_treap();
	// test_list();
	// test_undirected_graph();
	// test_directed_graph();
	// test_matrix();
	test_bfs();
	test_dfs();
}