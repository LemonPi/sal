#include <iostream>
#include <vector>
#include "../matrix.h"
#include "../vector.h"
#include "../../algo/utility.h"

using namespace std;
using namespace sal;
constexpr int test_size = 10000000;

// higher level framework
template <typename Indexable>
void profile_indexable(Indexable&& table) {
	Timer time;
	int sum {0};
	for (int i = 0; i < test_size; ++i)
		sum += table[randint(test_size-1)];
	cout << "random reads: " << time.tonow() / 1000.0 << " sum val " << sum << endl;

	time.restart();
	for (int i = 0; i < test_size; ++i)
		table[randint(test_size-1)] = 0;
	cout << "random writes: " << time.tonow() / 1000.0 << endl;
}

template <typename Vector>
void profile_vector(Vector&& vec) {
	Timer time;
	for (int i = 0; i < test_size; ++i)
		vec.push_back(i);
	cout << "sequential push back: " << time.tonow() / 1000.0 << endl;

	for (int i = 0; i < test_size; ++i)
		vec.emplace_back(i);
	cout << "sequential emplace back: " << time.tonow() / 1000.0 << endl;

	time.restart();
	for (auto& elem : vec) elem = 0;
	cout << "iteration: " << time.tonow() / 1000.0 << endl;
}


// specific implementations to test
void profile_persistent_vector() {
	cout << "persistent vector\n";
	Timer time;
	Persistent_vector<int> table {test_size};
	for (int i = 0; i < test_size; ++i)
		table.push_back(i);
	cout << "initialization: " << time.tonow() / 1000.0 << endl;	
	profile_indexable(table);
	profile_vector(table);
	cout << endl;
}

void profile_fixed_vector() {
	cout << "fixed vector\n";
	Timer time;
	Fixed_vector<int> table {test_size};
	for (int i = 0; i < test_size; ++i)
		table.push_back(i);
	cout << "initialization: " << time.tonow() / 1000.0 << endl;	
	profile_indexable(table);
	time.restart();
	table.reserve(3 * test_size);
	cout << "additional reservation time: " << time.tonow() / 1000.0 << endl;
	profile_vector(table);
	cout << endl;
}

void profile_std_vector() {
	cout << "std vector\n";
	Timer time;
	vector<int> table;
	table.reserve(test_size);
	for (int i = 0; i < test_size; ++i)
		table.push_back(i);
	cout << "initialization: " << time.tonow() / 1000.0 << endl;	
	profile_indexable(table);
	profile_vector(table);
	cout << endl;
}

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
	// profile_mat_mul();
	profile_persistent_vector();
	profile_fixed_vector();

	profile_std_vector();
}