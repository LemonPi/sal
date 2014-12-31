SAL - simple algorithms and datastructures library
===

A header only library containing efficient algorithms and data structures implemented simply.  
Simplicity here refers to how close the implementation matches the core concepts of each algorithm,
rather than the triviality of each algorithm.

<br>
Features
---
decoupled algorithms so most files are standalone 
- contest friendly 
- paste specific functions in without the rest of the library

<br>
simple implementation of efficient algorithms 
- learning friendly
- engineered for readability

<br>
Motivation
---
I enjoyed some features of the boost library (namely variant), but found the library too cumbersome as a whole.  
Using it in a set up environment was fine, but it was impossible to understand conceptual slices of it.  
It wasn't the best resource to learn the concepts from, and you couldn't (in most cases) use functions from it for contests.  

The focus for this library is on representing the concepts 

Table of contents
===
For examples, look at algotest.cpp and datatest.cpp
Algorithms
---
###### [sal/algo/numerics.h --- numeric](#numeric)
- modular exponentiation
- integer exponentiation
- fibonacci generation
- cyclic number generation and detection
- greatest common denominator
- checking for perfect square and powers of

###### [sal/algo/perm.h --- permutation and combination](#perm)
- kth permutation of an indexable sequence
- all permutations (non-distinct)
- all permutations (distinct)
- generate all possible combinations using each element in sequence
- count ways to generate a sum using a set number of elements of given value

###### [sal/algo/prime.h --- prime generation and manipulation](#prime)
- sequence of primes
- nth prime
- prime closest to a number
- prime after a number
- count primes below a number
- check if prime
- totient 

###### [sal/algo/search.h --- basic searching, substring matching, and finding longest common features](#search)
- binary search on sorted sequence
- intersection of a set of sets
- order statistic select 
- matching word inside sentence
- longest common substring
- longest common subsequence
- longest common subsequence length
- suffix array (data structure for substring searches)

###### [sal/algo/sort.h --- comparison, distributive, and hybrid sorts](#sort)
- partition
- bubble sort
- insertion sort
- merge sort
- quick sort
- heap sort
- counting sort
- radix sort
- tim sort
- patience sort

###### [sal/algo/string.h --- edit distances](#string)
- levenshtein distance

###### [sal/algo/factorize.h --- factorization of integers](#factorize)
- prime factorize
- number of total factors
- sum of total factors

###### [sal/algo/utility.h --- utility and testing functions](#utility)
- random integer generation
- timer
- perturb a sequence (soft random shuffle)


Data structures
---
###### [sal/data/list.h --- basic linked list](#list)

###### [sal/data/matrix.h --- 2D matrix](#matrix)
- square identities
- random matrices
- efficient multiplication of a sequence of matrices
- resizable

###### sal/data/infint.h --- infinity precision integers (Sercan Tutar's)

###### [sal/data/tree.h --- red black tree and augmentations of it](#tree)
- easily extensible base RB tree
- node iterator
- order statistic tree augmentation
- treap
- order statistic tree

###### [sal/data/interval.h --- interval tree](#interval)

###### [sal/data/graph.h --- directed and undirected graphs](#graph)
- adjacency matrix (not really supported)
- adjacency list
- vertex iterator
- adjacency iterator

###### [sal/data/graph/search.h --- graph searches](#graph_search)
- breadth first search
- depth first search
- recursive depth first search
- visitors for custom actions at certain points of DFS

###### [sal/data/utility.h --- important graph algorithms](#graph_utility)
- topological sort
- check for cycle
- transpose of graph
- strongly connected components

Example usage
===
```namespace sal``` will be used implicitely here
###### sal/algo/numerics.h --- <a name="numeric">numeric</a>
```C++
// 7^91 % 10
int a = modular_pow(7, 91, 10); 
// int 3


int_pow(5, 3);
// int 125


Infint c = fibonacci(1000);
// Infint 43466557686937456435688527675040625802564660517371780402481729089536555417949051890403879840079255169295922593080322634775209689623239873322471161642996440906533187938298969649928516003704476137795166849228875

// repeating part of 1/7 in base 10
make_cyclic(10, 7);
// Infint 142857

// length of repeating part of 1/7 in base 10
cycle_length(10, 7);
// int 6

// greatest common denominator
gcd(56, 91);
// unsigned int 7


// multiplies matrices using optimal parenthesization to minimize work
std::vector<Matrix<int>> mats;
size_t row {30}, col {35};
for (int i = 0; i != 100; ++i) {
	// generate sequence of random matrices
	mats.push_back(random_matrix<int>(row, col, 0, 50));
	// next matrix's row must be prev matrix's col
	row = col;
	col = rand() % 100 + 5;	// between 5 - 105
}
mul(mats);
// Matrix<int> 

// check for perfect square
is_square(21489798124);
// bool false
```
###### sal/algo/perm.h --- <a name="perm">permutation and combination</a>
```C++
std::string words {"Hello"};
// modifies the sequence without return
// algorithms work on indexable sequences
// k is from 0 to s.size()! - 1
perm(words, 1);
// string "oHell"


llperms(words);
// vector<string> (120 permutations of "Hello")

allperms_distinct(words);
// set<string> (60 distinct permutations of "Hello")



std::vector<int> ints {1,2,3,4,5,6};

std::set<int> int_add {combine(ints, 
	[](int a, int b){return a + b;})};
// 2 3 4 5 6 7 8 9 10 11 12 handshake by adding

std::set<int> int_add_odd {combine(ints,
	[](int a, int b){return a + b;},
	[](int a, int b){return (a & 1) && (b & 1);})};
// 2 4 6 8 10 handshake by adding odd elements



std::vector<int> coins {1, 2, 5, 10, 20, 50, 100, 200};
int combos = count_combos(coins, 200);
// size_t 73682 (ways to sum up to 200 using coins)
```
###### sal/algo/prime.h --- <a name="prime">prime generation and manipulation</a>
```C++
// keep a sieve object if continuous work with primes is needed
using big_int = size_t;
Sieve<big_int> sieve;

while (true) sieve.next_prime();
// generate infinite stream of primes (each of big_int)


sieve.nth_prime(1000);
// big_int 7919 (1000th prime)


sieve.is_prime(sieve.nth_prime(420000));
// bool true (the 4200000th prime is a prime!)


sieve.count(1000000);
// size_t 78498 primes below a million 


sieve.closest_prime(50000);
// big_int 49999
sieve.next_prime(50000);
// big_int 50021,


sieve.primes_upto(1000000);
// vector<big_int> (78498 primes that are under one million)


totient(500);
phi(500);
// big_int 200 (200 numbers below 500 that are coprime with it)
```
###### sal/algo/search.h --- <a name="search">basic searching, substring matching, and finding longest common features</a>
```C++
std::vector<int> seq {1,3,12,14,15,18,20};
// bin_search assumes sequence is in order and elements are comparable
bin_search(seq.begin(), seq.end(), 12);
// iterator to element 12

bin_search(seq.begin(), seq.end(), 17);
// iterator seq.end()


std::vector<int> seq2 {1,3,5,6,7,8,20,32};
std::vector<int> seq3 {2,3,6,9,20,32,45,55};
intersection(std::set<vector<int>>{seq, seq2, seq3});
// unordered_set {3, 20} (elements shared by all 3 sequences)


std::string a {"It was the best of times..."};
std::string b {"That's the best orange juice!"};
lc_substr(a, b);
// string "s the best o"

lc_subseq(a, b);
// string "as the best o ie"
lc_subseq_len(a, b);
// size_t 16


sub_match(a, std::string{"the best"});
// const_iterator to 't' in a


// find ith smallest element (1 is smallest)
std::vector<int> v {632, 32, 31, 50, 88, 77, 942, 5, 23};
select(v.begin(), v.end(), 4);
// iterator to 4th element (50)
```
###### sal/algo/sort.h --- <a name="sort">comparison, distributive, and hybrid sorts</a>
```C++
std::vector<int> u {632, 32, 31, 88, 77, 942, 5, 23};
partition(u.begin(), u.end());
// iterator to pivot 77
// 23 32 31 5 77 942 88 632 (all elements < 77 on left and > 77 on right)
 

std::vector<int> v {randgen(1048576, 100000)};	// 2^20

bub_sort(v.begin(), v.end());

// need to know maximum for counting sort, else wastes one pass finding maximum
cnt_sort(v.begin(), v.end(), 1048577);

// binary insertion sort
ins_sort(v.begin(), v.end());

heap_sort(v.begin(), v.end());

// linear insertion sort
lin_sort(v.begin(), v.end());

mer_sort(v.begin(), v.end());

// patience sort, really slow in practice
pat_sort(v.begin(), v.end());

qck_sort(v.begin(), v.end());

// need to know maximum for counting sort, else uses the maximum bit of size
rdx_sort(v.begin(), v.end(), 20);	// 20 bits needed for 2^20 max

tim_sort(v.begin(), v.end());
```

###### sal/algo/string.h --- <a name="string">edit distances</a>
```C++
// # edit actions including insertion, deletion, or substitution to turn source into new
levenshtein("Saturday", "Sunday");
// size_t 3 
```

###### sal/algo/factorize.h --- <a name="factorize">factorization of integers</a>
```C++
size_t num = 421412;

// prime factorize
factorize(421412);
// vector<size_t> 2 2 137 769 (in order)


num_factors(num);
// size_t 12 (1 2 4 137 274 548 769 1538 3076 105353 210706 421412)

sum_factors(num);
// size_t 743820 (1 + 2 + 4 + 137 + ... + 421412)
```

###### sal/algo/utility.h --- <a name="utility">utility and testing functions</a>

###### sal/data/list.h --- <a name="list">basic linked list</a>

###### sal/data/matrix.h --- <a name="matrix">2D matrix</a>

###### sal/data/tree.h --- <a name="tree">red black tree and augmentations of it</a>

###### sal/data/interval.h --- <a name="interval">interval tree</a>

###### sal/data/graph.h --- <a name="graph">directed and undirected graphs</a>

###### sal/data/graph/search.h --- <a name="graph_search">graph searches</a>

###### sal/data/utility.h --- <a name="graph_utility">important graph algorithms</a>


