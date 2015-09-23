#include <iostream>
#include <vector>
#include <set>
#include "../matrix.h"
#include "../vector.h"
#include "../tree.h"
#include "../interval.h"
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

	time.restart();
	for (int i = 0; i < test_size; ++i)
		vec.emplace_back(i);
	cout << "sequential emplace back: " << time.tonow() / 1000.0 << endl;

	time.restart();
	for (auto& elem : vec) elem = 0;
	cout << "iteration write: " << time.tonow() / 1000.0 << endl;
}

template <typename Set>
void profile_set(Set& single_set) {
	Timer time;
	for (int i = 0; i < test_size; ++i)
		single_set.insert(i);
	cout << "sequential insert: " << time.tonow() / 1000.0 << endl;

	time.restart();
	for (auto& elem : single_set) (void)elem;
	cout << "iteration: " << time.tonow() / 1000.0 << endl;	

	time.restart();
	single_set.clear();
	cout << "clear: " << time.tonow() / 1000.0 << endl;

	time.restart();
	for (int i = test_size - 1; i >= 0; --i)
		single_set.insert(i);
	cout << "reverse order insert: " << time.tonow() / 1000.0 << endl;

	time.restart();
	int find_start = randint(test_size - 1);
	for (int i = 0; i < test_size; ++i)
		single_set.find((find_start + i) % test_size);
	cout << "find: " << time.tonow() / 1000.0 << endl;
	
	time.restart();
	for (int i = 0; i < test_size; i += 5) {
		for (int j = 0; j < 5; ++j) single_set.find((find_start + i) % test_size);
	}
	cout << "find nearby: " << time.tonow() / 1000.0 << endl;

	time.restart();
	for (int i = 0; i < test_size; ++i)
		single_set.erase(i);
	cout << "erase: " << time.tonow() / 1000.0 << endl;
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



void profile_basic_tree() {
	cout << "basic tree\n";
	Basic_tree<int> tree_set;
	profile_set(tree_set);
	cout << endl;
}

void profile_std_set() {
	cout << "std set\n";
	std::set<int> std_set;
	profile_set(std_set);
	cout << endl;
}

void profile_treap() {
	cout << "treap\n";
	Basic_treap<int> treap_set;
	profile_set(treap_set);
	cout << endl;
}

void profile_interval_set() {
	cout << "interval set\n";
	Interval_set<int> interval_set;
	Timer time;
	for (int i = 0; i < test_size; ++i) {
		int low {i};
		int width {2*i};
		interval_set.insert({low, low + width});
	}
	cout << "sequential interval insert: " << time.tonow() / 1000.0 << endl;

	time.restart();
	for (int i = 0; i < test_size; ++i) {
		int low {i};
		int width {10};
		interval_set.find(low, low + width);
	}
	cout << "sequential find any overlapping: " << time.tonow() / 1000.0 << endl;

	time.restart();
	for (int i = 0; i < test_size; ++i) {
		int low {i};
		int width {10};
		interval_set.find_first(low, low + width);
	}
	cout << "sequential find smallest overlapping: " << time.tonow() / 1000.0 << endl;

	time.restart();
	for (int i = 0; i < test_size; ++i) {
		int low {i};
		int width {2*i};
		interval_set.find_exact(low, low + width);
	}
	cout << "sequential find exact interval: " << time.tonow() / 1000.0 << endl;

	time.restart();
	for (int i = 0; i < test_size; ++i) {
		int low {i};
		int width {2*i};
		interval_set.erase({low, low + width});
	}
	cout << "sequential erase: " << time.tonow() / 1000.0 << endl;
	if (!interval_set.empty()) {cout << "FAILED...Interval set erase " << interval_set.size() << endl;}
}

struct Rect {
	int xl, xh, yl, yh;
};

void profile_plane_set() {
	std::vector<Rect> lines;
	static constexpr int half_width = 5;
	// the bounding box size of the plane also impacts performance (significant in lookup)
	for (int i = 0; i < test_size/100; ++i) {
		int x_center {randint(test_size / 100)};
		int y_center {randint(test_size / 100)};
		// odd and even decide whether line will be vertical or horizontal
		if (i & 1) lines.push_back({x_center - half_width, x_center + half_width, y_center, y_center});
		else 	   lines.push_back({x_center, x_center , y_center - half_width, y_center + half_width});
	}
	cout << "plane set\n";
	Timer time;
	sal::Plane_set<int> planes;
	for (const auto& line : lines) {
		planes.insert(line.xl, line.xh, line.yl, line.yh);
	}
	cout << "random insert: " << time.tonow() / 1000.0 << endl;

	time.restart();
	for (const auto& line : lines) {
		planes.find(line.xl, line.xh, line.yl, line.yh);	// every one is a hit
	}
	cout << "random queries (always hit): " << time.tonow() / 1000.0 << endl;

	time.restart();
	for (const auto& line : lines) {
		planes.find(line.yl, line.yh, line.xl, line.xh);	// hits will be very rare
	}
	cout << "random queries (rare hit): " << time.tonow() / 1000.0 << endl;
}

int main() {
	// profile_mat_mul();

	// profile_persistent_vector();
	// profile_fixed_vector();
	// profile_std_vector();

	// compared to std set
	// 1.1 times slower insert (for all cases), 1.12 times faster iteration, same clear speed, 1.3 times faster find
	// 1.05 times slower erase
	// profile_basic_tree();
	// 4 times faster insert (for all cases), 1.532 times slower iteration (!?), same clear speed, 2 times faster find
	// 2 times faster erase
	// profile_treap();
	// profile_std_set();

	// treap version is 4 times faster than RB version as with sets - use Treaps!
	// finding any overlapping interval is 2 orders of magnitude faster than finding smallest and exact
	// finding all is much slower
	// profile_interval_set();

	profile_plane_set();
}