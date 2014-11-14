#include <iostream>
#include "data/matrix.h"

using namespace std;

void test_mul() {
	Matrix<int> A {{2, 5, 6},
				   {3, 4, -3},
				   {7, 8, 0}};
	Matrix<int> B {{-1, 1},
				   {5, -2},
				   {4, 2}};
	if (A*B != Matrix<int>{{47, 4},{5,-11},{33,-9}}) 
		cout << "FAILED...Multiplication\n";
	else cout << "PASSED...Multiplication\n";
}

void test_pow() {
	Matrix<int> F {{1, 1},
				   {1, 0}};
	if (F.pow(5) != Matrix<int>{{8, 5}, {5, 3}}) 
		cout << "FAILED...Exponentiation\n";
	else cout << "PASSED...Exponentiation\n";
}
int main() {
	Matrix<int> id3 {identity<int>(3)};
	test_mul();
	test_pow();
}