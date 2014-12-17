#include <iostream>
#include "matrix.h"
#include "heap.h"
#include "tree.h"
#include "order_tree.h"
#include "interval.h"

using namespace std;
using namespace sal;

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
	auto nil = sal::Tree<int>::get_nil();
	if (static_cast<int>(nil->color) != 0) 
		std::cout << "FAILED...Tree nil\n";

	sal::Tree<int> t {5, 3, 7, 1, 9, 4, 2, 0, 10, 8, 6};
	t.print();

	auto node = t.find(4);
	if (node == nil) std::cout << "FAILED...Tree find\n";

	t.erase(4);
	t.print();
	node = t.find(4);
	if (node != nil) std::cout << "FAILED...Tree erase\n";

	t.insert(5);
	t.print();
}

void test_order_tree() {
	auto nil = sal::Order_tree<int>::get_nil();
	sal::Order_tree<int> t {5, 3, 7, 1, 9, 4, 2, 0, 10, 8, 6};
	t.print();

	auto node = t.find(4);
	if (node == nil) std::cout << "FAILED...Order tree find\n";

	t.erase(4);
	t.print();
	node = t.find(4);
	if (node != nil) std::cout << "FAILED...Order tree erase\n";

	t.insert(5);
	t.print();

	int rank {4};
	node = t.select(rank);
	if (node == nil || node->key != 3) std::cout << "FAILED...Order tree select\n";
}

void test_interval_set() {
	auto nil = Interval_set::get_nil();
	sal::Interval_set t {{16,21}, {8,9}, {5,8}, {15,23}, {25,30}, {0, 3}, {6, 10}, {17,19}, {26,26}, {19,20}};
	t.print();


	auto interval = t.find({22, 25});
	if (*interval != Interval{15,23}) std::cout << "FAILED...Interval set find\n";

	interval = t.find(11, 14);
	if (interval != nil) std::cout << "FAILED...Interval set find\n";
}

int main() {
	// Matrix<int> id3 {identity<int>(3)};
	// test_mul();
	// test_pow();
	// test_heap();
	// test_tree();
	// test_order_tree();
	test_interval_set();
}