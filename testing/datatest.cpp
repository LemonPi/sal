#include <algorithm>
#include <iostream>
#include <list>
#include <string>
#include "../../algo/macros.h"
#include "../matrix.h"
#include "../heap.h"
#include "../tree.h"
#include "../list.h"
#include "../interval.h"
#include "../graph.h"
#include "../graph/search.h"
#include "../graph/utility.h"
#include "../graph/shortest.h"
#include "../graph/linear.h"
#include "../vector.h"

using namespace std;


void test_list(bool print) {
	sal::Basic_list<int> l {1, 4, 2, 5, 3, 7, 6};
	if (print) cout << l;
}

void test_mul(bool print) {
	sal::Matrix<int> A {{2, 5, 6},
				   		{3, 4, -3},
				   		{7, 8, 0}};
	sal::Matrix<int> B {{-1, 1},
				  		{5, -2},
				   		{4, 2}};
	if (print) {
		cout << B << endl;
		cout << B.transpose() << endl;
	}
	if (A*B != sal::Matrix<int>{{47, 4},{5,-11},{33,-9}}) {
		cout << A*B << endl;
		cout << "FAILED...Matrix multiplication\n";
	}
	
}

void test_pow(bool print) {
	sal::Matrix<int> F {{1, 1},
				   		{1, 0}};
	if (F.pow(5) != sal::Matrix<int>{{8, 5}, {5, 3}}) 
		cout << "FAILED...Matrix exponentiation\n";
}

void test_heap(bool print) {
	sal::Heap<int> h2 {3, 4, 6, 5, 1, 8, 11, 12};
	if (print) sal::print(h2);
	if (!h2.is_maxheap()) cout << "FAILED...Heap construction\n";
	if (h2.key(12) != 1 || h2.key(11) != 3) cout << "FAILED...Heap find key\n";

	sal::Heap<int> h3 {13,1,3,4,5,2,3,4,5,1,7};
	if (print) sal::print(h3);
	if (!h3.is_maxheap()) cout << "FAILED...Heap construction\n";
	if (print) {
		while (!h3.empty()) cout << h3.extract_top() << ' ';
		cout << '\n';
	}


	std::make_heap(h2.begin(), h2.end());
	if (print) sal::print(h2);
	// should be the same after trying to makeheap

	if (!h2.is_maxheap()) cout << "FAILED...Heap construction\n";
	// constructing non-POD
	sal::Heap<std::string> h2o {"probably not POD", "another string", "is this actually string?"};
	if (print) PRINTLINE(std::is_pod<std::string>::value);

	using SPM = std::map<int, sal::Shortest_vertex<int>>;
	using Cmp = sal::Shortest_cmp<SPM>;

	SPM property;

	// random testing value % 6
	std::vector<int> order {3, 4, 6, 5, 1, 8, 11, 12};
	for (int v : order) { property[v] = sal::Shortest_vertex<int>{v}; property[v].distance = v % 6; }

	// create comparator
	sal::Heap<int, Cmp> h {Cmp{property}};
	
	if (print) cout << "batch inserting\n";
	h.batch_insert(order.begin(), order.end());

	size_t prechange_key {h.key(6)};
	property[6].distance = 10;
	if (print) {
		sal::print(h);
		cout << "sifting up 6 with index " << prechange_key << endl;
	}
	h.sift_down(prechange_key);

	if (print) cout << "binary heap print\n";
	while (!h.empty()) {
		auto top = h.extract_top();
		if (print) std::cout << top << ':' << property[top].distance << endl;
	}
	cout << endl;
}

void test_tree(bool print) {

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
	if (print) for_each(t.begin(), t.end(), 
		[](const Node& node){std::cout << node.key << ' ';});
}

void test_order_tree(bool print) {
	sal::Order_tree<int> t {5, 3, 7, 1, 9, 4, 2, 0, 10, 8, 6};

	size_t rank {4};
	auto node = t.select(rank);
	if (node == t.end() || node->key != 3) cout << "FAILED...Order tree select\n";

	if (t.rank(node.get()) != rank) cout << "FAILED...Order tree rank\n";
}

void test_treap(bool print) {
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

void test_interval_set(bool print) {
	sal::Interval_set<int> t {{16,21}, {8,9}, {5,8}, {15,23}, {25,30}, {0, 3}, {6, 10}, {17,19}, {26,26}, {19,20}};

	auto interval = t.find({22, 25});
	if (*interval.get() != sal::Interval<int>{15,23}) cout << "FAILED...Interval set find\n";

	interval = t.find(11, 14);
	if (interval != t.end()) cout << "FAILED...Interval set find\n";
}

void test_undirected_graph(bool print) {
	sal::graph<int> g {{5,1},{5,4},{5,10},{1,4},{4,10}};

	if (g.num_vertex() != 4) cout << "FAILED...Undirected graph vertex number\n";
	if (g.num_edge() != 5) cout << "FAILED...Undirected graph edge number\n";

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

void test_directed_graph(bool print) {
	sal::digraph<int> g {{5,1},{5,4},{5,10},{1,4},{4,10}};

	if (g.num_edge() != 5) cout << "FAILED...Directed graph edge number\n";

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

void test_bfs(bool print) {
	sal::graph<char> d {{'v','r'},{'r','s'},{'s','w'},{'w','t'},{'t','x'},{'w','x'},{'t','u'},
						{'x','u'},{'x','y'},{'u','y'}};

	// property breadth first tree linked by each element's parent attribute
	auto property = sal::bfs(d, 's');
	if (print) for (char parent = 'u'; ; parent = property[parent].parent) {
		cout << parent;
		if (parent == 's') {cout << endl; break;}
		cout << " <- ";
	}
	if (property['u'].distance != 3) cout << "FAILED...Breadth first search\n";
}

void test_dfs(bool print) {
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

void test_topological_sort(bool print) {
	// directed edge (u,v) means u must happen before v
	sal::digraph<string> dress {{"undershorts", "pants"}, {"undershorts", "shoes"}, {"pants", "belt"},
			{"pants", "shoes"}, {"socks", "shoes"}, {"shirt", "belt"}, {"shirt", "tie"}, {"tie", "jacket"},
			{"belt", "jacket"}};
	dress.add_vertex("watch");
	if (sal::has_cycle(dress)) cout << "FAILED...Cycle testing (DFS)\n";

	list<string> dress_order;
	// give a possible ordering of events
	sal::topological_sort(dress, front_inserter(dress_order));

	if (print) for (const string& item : dress_order) cout << item << ' ';
	if (print) cout << endl;

	sal::digraph<char> h {{'m','q'},{'m','r'},{'m','x'},{'n','q'},{'n','u'},{'n','o'},{'o','r'},{'o','v'},{'o','s'},
						  {'p','o'},{'p','s'},{'p','z'},{'q','t'},{'r','u'},{'r','y'},{'s','r'},{'u','t'},{'v','x'},
						  {'v','w'},{'w','z'},{'y','v'}};
	if (sal::has_cycle(h)) cout << "FAILED...Cycle testing (DFS)\n";

	list<char> topo_order2;
	sal::topological_sort(h, front_inserter(topo_order2));
	if (print) for (char v : topo_order2) cout << v << ' ';
	if (print) cout << '\n';
}

void test_transpose(bool print) {
	sal::digraph<char> g {{'u','v'},{'u','x'},{'x','v'},{'v','y'},{'y','x'},
						{'w','y'},{'w','z'},{'z','z'}};	
	sal::digraph<char> g_t (sal::transpose(g));
	if (print) cout << g << endl;
	if (print) cout << g_t;
}

void test_strongly_connected(bool print) {
	sal::digraph<char> g {{'a','b'},{'b','c'},{'b','e'},{'b','f'},{'c','d'},{'c','g'},
						{'d','c'},{'d','h'},{'e','a'},{'e','f'},{'f','g'},{'g','f'},{'g','h'},{'h','h'}};

	// should be abe cd fg h
	auto connected_sets = strongly_connected(g);
	if (print) for (const auto& component: connected_sets) {
		for (char vertex : component) cout << vertex << ' ';
		cout << '\t';
	}
}

void test_mst(bool print) {
	sal::graph<char> circuit {{'a','b',4},{'a','h',8},{'b','h',11},{'b','c',8},{'c','i',2},
					{'c','f',4},{'c','d',7},{'d','f',14},{'d','e',9},{'e','f',10},{'f','g',2},
					{'i','h',7},{'i','g',6},{'h','g',1}};
	// minimum spanning tree
	auto mst = sal::min_span_tree(circuit);
	
	if (print) for (char v : circuit) PRINTLINE(v << " <- " << mst[v].parent);
	// to actually create the tree, simply iterate over vertex and add edge between parent and child
	sal::graph<char> min_circuit {sal::pm_to_tree(mst)};

	if (print) std::cout << min_circuit;

	sal::graph<char> h {{'a','b',6},{'a','c',12},{'b','c',5},{'b','e',14},{'b','h',8},
					{'c','d',9},{'c','f',7},{'e','h',3},{'f','h',10},{'f','g',15}};
	mst = sal::min_span_tree(h);
	if (print) std::cout << std::endl;
	if (print) for (char v : h) PRINTLINE(v << " <- " << mst[v].parent);
}

void test_matrix(bool print) {
	sal::Matrix<int> id3 {sal::identity<int>(3)};
	if (print) cout << id3 << endl;
	id3.resize(4,5);
	if (print) cout << id3 << endl;
	id3.resize(2,2);
	if (print) cout << id3 << endl;
	test_mul(print);
	test_pow(print);
}

void test_bellman_ford(bool print) {
	sal::digraph<char> g {{'s','t',6},{'s','y',7},{'t','y',8},{'t','x',5},{'t','z',-4},
						{'x','t',-2},{'y','x',-3},{'y','z',9},{'z','s',2},{'z','x',7}};

	auto shortest_path = sal::bellman_ford(g, 's');
	if (print) for (char v : g) PRINTLINE(v << " <- " << shortest_path[v].parent << '\t' << shortest_path[v].distance);
	if (shortest_path.empty()) PRINTLINE("FAILED...Bellman ford shortest path");
	if (!sal::is_shortest(shortest_path, g, 's')) PRINTLINE("FAILED...Bellman ford shortest path");
}

void test_shortest_dag(bool print) {
	sal::digraph<char> g {{'r','s',5},{'r','t',3},{'s','t',2},{'s','x',6},{'t','x',7},
						{'t','y',4},{'t','z',2},{'x','y',-1},{'x','z',1},{'y','z',-2}};
	auto topo_shortest = sal::shortest_dag(g, 's');
	if (print) for (char v : g) PRINTLINE(v << " <- " << topo_shortest[v].parent << '\t' << topo_shortest[v].distance);
	if (topo_shortest['r'].distance != POS_INF(int) || topo_shortest['s'].distance != 0 || topo_shortest['t'].distance != 2 ||
		topo_shortest['x'].distance != 6 || topo_shortest['y'].distance != 5 || topo_shortest['z'].distance != 3)
		PRINTLINE("FAILED...DAG shortest path");
	if (!sal::is_shortest(topo_shortest, g, 's')) PRINTLINE("FAILED...DAG shortest path");

	auto topo_critical = sal::critical_dag(g, 'r');
	if (print) for (char v : g) PRINTLINE(v << " <- " << topo_critical[v].parent << '\t' << topo_critical[v].distance);
}

void test_dijkstra(bool print) {
	sal::digraph<char> g {{'s','t',10},{'s','y',5},{'t','y',2},{'t','x',1},{'x','z',4},{'y','t',3},
						{'y','x',9},{'y','z',2},{'z','s',7},{'z','x',6}};
	auto non_neg_shortest = sal::dijkstra(g, 's');
	if (print) for (char v : g) PRINTLINE(v << " <- " << non_neg_shortest[v].parent << '\t' << non_neg_shortest[v].distance);
	if (non_neg_shortest['t'].distance != 8 || non_neg_shortest['x'].distance != 9 || non_neg_shortest['y'].distance != 5 || 
		non_neg_shortest['z'].distance != 7) PRINTLINE("FAILED...Djikstra non-negative shortest path");

	if (!sal::is_shortest(non_neg_shortest, g, 's')) PRINTLINE("FAILED...Djikstra non-negative shortest path");
}

void test_difference_constraint(bool print) {
	// solution size is # of cols for constraint matrix A
	sal::Constraint_sys<int> system {{1,2,0},{1,5,-1},{2,5,1},{3,1,5},{4,1,4},{4,3,-1},
									{5,3,-3},{5,4,-3}};
	sal::Constraint_sol<int> solution {sal::feasible(system, 5)};
	if (print) sal::print(solution);
	for (auto constraint : system) 
		if (solution[constraint.j-1] - solution[constraint.i-1] > constraint.limit) 
			PRINTLINE("FAILED...Difference constraint feasibility with Bellman-Ford");
}

void test_adjacency_matrix(bool print) {
	// std::vector<sal::UEdge<size_t>> edges {{0,1},{0,2},{1,2},{3,2}};
	// sal::graph_mat<int> g {edges.begin(), edges.end(), 4};

	sal::graph_mat<int> g {{0,1},{0,2},{1,2},{3,2}};
	if (print) std::cout << g;

	sal::digraph_mat<int> h {{0,1},{0,2},{1,2},{3,2}};
	if (print) std:: cout << h;
}

void test_vector(bool print) {
	std::vector<int> stdvec;
	sal::Persistent_vector<int> persvec;
	sal::Fixed_vector<int> fixedvec;
	fixedvec.reserve(100);
	for (int i = 100; i != 0; --i) {
		stdvec.push_back(i);
		persvec.push_back(i);
		fixedvec.push_back(i);
	}
	if (stdvec.size() != persvec.size())
		PRINTLINE("FAILED...Size of stdvec and persistent vector different after populating " << stdvec.size() << ' ' << persvec.size());
	if (stdvec.size() != persvec.size())
		PRINTLINE("FAILED...Size of stdvec and fixed vector different after populating " << stdvec.size() << ' ' << fixedvec.size());

	std::sort(begin(stdvec), end(stdvec));
	std::sort(begin(persvec), end(persvec));
	std::sort(begin(fixedvec), end(fixedvec));
	if (print) {
		cout << "stdvec: "; sal::print(stdvec);
		cout << "persvec: "; sal::print(persvec);
		cout << "fixedvec: "; sal::print(fixedvec);
	}
	for (int i = 0; i < 100; ++i) {
		if (stdvec[i] != persvec[i])
			PRINTLINE("FAILED...Sorted element different for stdvec and persvec " << stdvec[i] << ' ' << persvec[i]);
		if (stdvec[i] != fixedvec[i])
			PRINTLINE("FAILED...Sorted element different for stdvec and fixed " << stdvec[i] << ' ' << fixedvec[i]);
	}
}

int main(int argc, char** argv) {
	bool print {false};
	// give p or -p argument for printing out results
	if (argc > 1 && (argv[1][0] == 'p' || argv[1][1] == 'p')) print = true; 
	// test_heap(print);
	// test_tree(print);
	// test_order_tree(print);
	// test_interval_set(print);
	// test_treap(print);
	// test_list(print);
	// test_undirected_graph(print);
	// test_directed_graph(print);
	// test_matrix(print);
	// test_bfs(print);
	// test_dfs(print);
	// test_topological_sort(print);
	// test_transpose(print);
	// test_strongly_connected(print);
	// test_mst(print);
	// test_bellman_ford(print);
	// test_shortest_dag(print);
	// test_dijkstra(print);
	// test_difference_constraint(print);
	// test_adjacency_matrix(print);
	test_vector(print);
}