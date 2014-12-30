#include <algorithm>
#include <iostream>
#include <list>
#include <string>
#include "../matrix.h"
#include "../heap.h"
#include "../tree.h"
#include "../list.h"
#include "../interval.h"
#include "../graph.h"
#include "../graph/search.h"
#include "../graph/utility.h"

using namespace std;


void test_list() {
	sal::Basic_list<int> l {1, 4, 2, 5, 3, 7, 6};
	cout << l;
}

void test_mul() {
	sal::Matrix<int> A {{2, 5, 6},
				   {3, 4, -3},
				   {7, 8, 0}};
	sal::Matrix<int> B {{-1, 1},
				   {5, -2},
				   {4, 2}};
	if (A*B != sal::Matrix<int>{{47, 4},{5,-11},{33,-9}}) 
		cout << "FAILED...Matrix multiplication\n";
	
}

void test_pow() {
	sal::Matrix<int> F {{1, 1},
				   {1, 0}};
	if (F.pow(5) != sal::Matrix<int>{{8, 5}, {5, 3}}) 
		cout << "FAILED...Matrix exponentiation\n";
}

void test_heap() {
	sal::Heap<int> h {3, 4, 6, 5, 1, 8, 11, 12};
	if (!h.is_maxheap()) cout << "FAILED...Heap construction\n";
}

void test_tree() {

	using Node = sal::Basic_node<int>;
	sal::Basic_tree<int> t {5, 3, 7, 1, 9, 4, 2, 0, 10, 8, 6};

	auto nil = t.end();
	if (static_cast<int>(nil->color) != 0) 
		cout << "FAILED...Basic_tree nil\n";

	auto node = t.find(4);
	if (node == t.end()) cout << "FAILED...Basic_tree find\n";

	t.erase(4);
	node = t.find(4);
	if (node != t.end()) cout << "FAILED...Basic_tree erase\n";

	t.insert(5);
	// testing iterators with standard library functions
	for_each(t.begin(), t.end(), 
		[](const Node& node){cout << node.key << ' ';});
}

void test_order_tree() {
	sal::Order_tree<int> t {5, 3, 7, 1, 9, 4, 2, 0, 10, 8, 6};

	auto node = t.find(4);
	if (node == t.end()) cout << "FAILED...Order tree find\n";

	t.erase(4);
	node = t.find(4);
	if (node != t.end()) cout << "FAILED...Order tree erase\n";

	t.insert(5);

	int rank {4};
	node = t.select(rank);
	if (node == t.end() || node->key != 3) cout << "FAILED...Order tree select\n";
}

void test_treap() {
	sal::Basic_treap<int> t {5, 3, 7, 1, 9, 4, 2, 0, 10, 8, 6};

	auto node = t.find(4);
	if (node == t.end()) cout << "FAILED...Tree find\n";

	t.erase(4);
	node = t.find(4);
	if (node != t.end()) cout << "FAILED...Tree erase\n";

	t.insert(5);
	// attempt to elevate 4's priority
	t.insert(4);
	for (int i = 0; i < 20; ++i) t.find(4);
}

void test_interval_set() {
	sal::Interval_set<int> t {{16,21}, {8,9}, {5,8}, {15,23}, {25,30}, {0, 3}, {6, 10}, {17,19}, {26,26}, {19,20}};

	auto interval = t.find({22, 25});
	if (*interval != sal::Interval<int>{15,23}) cout << "FAILED...Interval set find\n";

	interval = t.find(11, 14);
	if (interval != t.end()) cout << "FAILED...Interval set find\n";
}

void test_undirected_graph() {
	sal::graph<int> g {{5,1},{5,4},{5,10},{1,4},{4,10}};

	if (g.weight(5, 1) != 1 || g.weight(5, 2) != 0 || g.weight(4, 1) != 1) 
		cout << "FAILED...Undirected unweighted graph edge\n";

	if (g.degree(3) != 0 || g.degree(5) != 3 || g.degree(4) != 3 || g.degree(2) != 0)
		cout << "FAILED...Undirected unweighted graph degree\n";
		
	// weighted
	sal::graph<int> w {{5,1,2}, {5,4,1}, {5,10,3}, {1,4,6}, {4,10,5}};

	if (w.weight(5, 1) != 2 || w.weight(5, 2) != 0 || w.weight(4, 1) != 6) 
		cout << "FAILED...Undirected weighted graph edge\n";

	if (w.degree(3) != 0 || w.degree(5) != 3 || w.degree(4) != 3 || w.degree(2) != 0)
		cout << "FAILED...Undirected weighted graph degree\n";	

	// adjacency iterator
	auto edges = w.adjacent(5);
	// alter the weight
	for (auto v = edges.first; v != edges.second; ++v)
		v.weight() = 1;
	// constant iteration
	for (auto v = edges.first; v != edges.second; ++v)
		if (v.weight() != 1) cout << "FAILED...graph iteration\n";

}

void test_directed_graph() {
	sal::digraph<int> g {{5,1},{5,4},{5,10},{1,4},{4,10}};

	if (g.weight(5, 1) != 1 || g.weight(5, 2) != 0 || g.weight(4, 1) != 0) 
		cout << "FAILED...Directed unweighted graph edge\n";

	if (g.degree(3) != 0 || g.degree(5) != 3 || g.degree(4) != 1 || g.degree(10) != 0)
		cout << "FAILED...Directed unweighted graph degree\n";

	// weighted
	sal::digraph<int> w {{5,1,2}, {5,4,1}, {5,10,3}, {1,4,6}, {4,10,5}};

	if (w.weight(5, 1) != 2 || w.weight(5, 2) != 0 || w.weight(4, 1) != 0 || w.weight(1, 4) != 6 || w.weight(4, 10) != 5) 
		cout << "FAILED...Directed weighted graph edge\n";

	if (w.degree(3) != 0 || w.degree(4) != 1 || w.degree(10) != 0 || w.degree(1) != 1)
		cout << "FAILED...Directed weighted graph degree\n";	

	// adjacency iterator
	auto edges = w.adjacent(5);
	// alter the weight
	for (auto v = edges.first; v != edges.second; ++v)
		v.weight() = 1;
	// constant iteration
	for (auto v = edges.first; v != edges.second; ++v)
		if (v.weight() != 1) cout << "FAILED...Graph adjacent iteration\n";

	// non-existent edges
	edges = w.adjacent(2);
	bool ghost_print {false};
	for (auto v = edges.first; v != edges.second; ++v)
		ghost_print = true;
	if (ghost_print) cout << "FAILED...Graph adjacent iteration (non-existent edge)\n";

	// vertex iteration
	for (auto v = w.begin(); v != w.end(); ++v)
		if (!w.is_vertex(*v)) cout << "FAILED...Graph vertex iteration\n";

}

void test_bfs() {
	sal::graph<char> d {{'v','r'},{'r','s'},{'s','w'},{'w','t'},{'t','x'},{'w','x'},{'t','u'},
						{'x','u'},{'x','y'},{'u','y'}};

	// property breadth first tree linked by each element's parent attribute
	auto property = sal::bfs(d, 's');
	for (char parent = 'u'; ; parent = property[parent].parent) {
		cout << parent;
		if (parent == 's') {cout << endl; break;}
		cout << " <- ";
	}
	if (property['u'].distance != 3) cout << "FAILED...Breadth first search\n";
}

void test_dfs() {
	sal::digraph<char> e {{'u','v'},{'u','x'},{'x','v'},{'v','y'},{'y','x'},
						{'w','y'},{'w','z'},{'z','z'}};
	auto dfs_property = sal::dfs(e);
	map<char, pair<size_t, size_t>> correct_timestamps;
	correct_timestamps['u'] = {1, 8};
	correct_timestamps['v'] = {2, 7};
	correct_timestamps['w'] = {9, 12};
	correct_timestamps['x'] = {4, 5};
	correct_timestamps['y'] = {3, 6};
	correct_timestamps['z'] = {10, 11};
	for (auto& pair : dfs_property) {
		if (correct_timestamps[pair.first] !=  make_pair(pair.second.start, pair.second.finish))
			cout << "FAILED...Depth first search\n";
	}
	// self cycle of z <- z is a cycle
	if (!sal::has_cycle(e)) cout << "FAILED...Cycle testing (DFS)\n";

}

void test_topological_sort() {
	// directed edge (u,v) means u must happen before v
	sal::digraph<string> g {{"undershorts", "pants"}, {"undershorts", "shoes"}, {"pants", "belt"},
			{"pants", "shoes"}, {"socks", "shoes"}, {"shirt", "belt"}, {"shirt", "tie"}, {"tie", "jacket"},
			{"belt", "jacket"}};
	g.add_vertex("watch");
	if (sal::has_cycle(g)) cout << "FAILED...Cycle testing (DFS)\n";

	list<string> topo_order;
	// give a possible ordering of events
	sal::topological_sort(g, front_inserter(topo_order));

	for (const string& v : topo_order) cout << v << ' ';
	cout << endl;

	sal::digraph<char> h {{'m','q'},{'m','r'},{'m','x'},{'n','q'},{'n','u'},{'n','o'},{'o','r'},{'o','v'},{'o','s'},
						  {'p','o'},{'p','s'},{'p','z'},{'q','t'},{'r','u'},{'r','y'},{'s','r'},{'u','t'},{'v','x'},
						  {'v','w'},{'w','z'},{'y','v'}};
	if (sal::has_cycle(h)) cout << "FAILED...Cycle testing (DFS)\n";

	list<char> topo_order2;
	sal::topological_sort(h, front_inserter(topo_order2));
	for (char v : topo_order2) cout << v << ' ';
	cout << '\n';
}

void test_transpose() {
	sal::digraph<char> g {{'u','v'},{'u','x'},{'x','v'},{'v','y'},{'y','x'},
						{'w','y'},{'w','z'},{'z','z'}};	
	sal::digraph<char> g_t (sal::transpose(g));
	cout << g << endl;
	cout << g_t;
}

void test_strongly_connected() {
	sal::digraph<char> g {{'a','b'},{'b','c'},{'b','e'},{'b','f'},{'c','d'},{'c','g'},
						{'d','c'},{'d','h'},{'e','a'},{'e','f'},{'f','g'},{'g','f'},{'g','h'},{'h','h'}};

	// should be abe cd fg h
	auto connected_sets = strongly_connected(g);
	for (const auto& component: connected_sets) {
		for (char vertex : component) cout << vertex << ' ';
		cout << '\t';
	}
}

void test_matrix() {
	sal::Matrix<int> id3 {sal::identity<int>(3)};
	cout << id3 << endl;
	id3.resize(4,5);
	cout << id3 << endl;
	id3.resize(2,2);
	cout << id3 << endl;
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
	// test_bfs();
	// test_dfs();
	// test_topological_sort();
	// test_transpose();
	// test_strongly_connected();
}