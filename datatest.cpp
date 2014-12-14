#include <iostream>
#include "matrix.h"
#include "heap.h"
#include "tree.h"

using namespace std;

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
	if (node != nil) std::cout << "FAILED...Tree insert\n";
	t.insert(5);
	t.print();
}

int main() {
	// Matrix<int> id3 {identity<int>(3)};
	// test_mul();
	// test_pow();
	// test_heap();
	test_tree();
}