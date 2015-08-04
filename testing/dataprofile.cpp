#include <iostream>
#include <vector>
#include "../matrix.h"
#include "../../algo/utility.h"

using namespace std;
constexpr size_t test_size = 1000;

void profile_mat_mul() {
	// square matrices
	size_t N = 50;
	sal::Matrix<int> A {N, N, sal::randgen(0, (int)N*N, N*N)};
	sal::Matrix<int> B {N, N, sal::randgen(0, (int)N*N, N*N)};

	sal::Timer time;
	for (size_t i = 0; i < test_size; ++i)
		A *= B;
	cout << "Matrix multiplication: " << time.tonow() / 1000.0 << "ms\n";
}

int main() {
	profile_mat_mul();
}