#include <vector>
#include <algorithm>
#include <iostream>
#include "../prime.h"
#include "../utility.h"
#include "../sort/partition.h"

using namespace std;
using namespace sal;

constexpr size_t test_size = 100000000;

void profile_prime_generation(size_t largest_prime) {
	Timer time;
	Sieve<> sieve {largest_prime};
	sieve.primes_upto(largest_prime);
	cout << "primes upto " << largest_prime << " in " << time.tonow() / 1000.0 << " ms\n";
}

void profile_partition() {
	vector<int> vals;
	for (size_t i = 0; i < test_size; ++i) vals.push_back(i);
	int pivot = vals[vals.size() / 2];
	auto comparator = [pivot](int val){return val < pivot;};
	random_shuffle(begin(vals), end(vals));

	vector<int> temp_vals {vals};
	Timer time;
	cout << "std partition" << endl;
	std::partition(begin(temp_vals), end(temp_vals), comparator);
	cout << time.tonow() / 1000.0 << " ms\n";

	temp_vals = vals;
	time.restart();
	cout << "partition with <" << endl;
	sal::partition(begin(temp_vals), end(temp_vals));
	cout << time.tonow() / 1000.0 << " ms\n";	

	temp_vals = vals;
	time.restart();
	cout << "partition with general unary comparator" << endl;
	sal::partition(begin(temp_vals), end(temp_vals), comparator);
	cout << time.tonow() / 1000.0 << " ms\n";		
}

int main() {
	// profile_prime_generation(test_size);	// 10^8

	// std partition fastest; partition with < takes 1.1 more time, general partition 1.25 more time
	profile_partition();
}