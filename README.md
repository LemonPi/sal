SAL - simple algorithms library
===

A header only library containing algorithms and data structures implemented <b>simply and efficiently</b>.

Features
---
### decoupled algorithms so most files are standalone 
#### contest friendly - paste specific functions in without the rest of the library

### simple implementation of efficient algorithms 
#### not over engineeringed
#### learning friendly - approachable simplicity

Motivation
---
I enjoyed some features of the boost library (namely variant), but found the library to be too cumbersome as a whole.  
Using the library in a set up environment was fine, but it was impossible to understand conceptual slices of it.  
It wasn't the best resource to learn the concepts from, and you couldn't (in most cases) use parts of it for contests.  

In response, the focus for this library is on the concepts rather than the implementation

Table of contents
===
For examples, look at algotest.cpp and datatest.cpp
Algorithms
---
###### sal/algo/numerics.h --- numerics
- modular exponentiation
- integer exponentiation
- fibonacci generation
- cyclic number generation and detection
- greatest common denominator
- checking for perfect square and powers of

###### sal/algo/perm.h --- permutation and combination
- kth permutation of an indexable sequence
- all permutations (non-distinct)
- all permutations (distinct)
- generate all possible combinations using each element in sequence
- count ways to generate a sum using a set number of elements of given value

###### sal/algo/prime.h --- prime generation and manipulation
- sequence of primes
- nth prime
- prime closest to a number
- prime after a number
- count primes below a number
- check if prime
- totient 

###### sal/algo/search.h --- basic searching, substring matching, and finding longest common features
- binary search on sorted sequence
- intersection of a set of sets
- order statistic select 
- matching word inside sentence
- longest common substring
- longest common subsequence
- longest common subsequence length
- suffix array (data structure for substring searches)

###### sal/algo/sort.h --- comparison, distributive, and hybrid sorts
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

###### sal/algo/string.h --- edit distances
- levenshtein distance

###### sal/algo/factorize.h --- factorization of integers
- prime factorize
- number of total factors
- sum of total factors

###### sal/algo/utility.h --- utility and testing functions
- random integer generation
- timer
- perturb a sequence (soft random shuffle)


Data structures
---
###### sal/data/list.h --- basic linked list

###### sal/data/matrix.h --- 2D matrix
- square identities
- random matrices
- efficient multiplication of a sequence of matrices
- resizable

###### sal/data/infint.h --- infinity precision integers (Sercan Tutar's)

###### sal/data/tree.h --- red black tree and augmentations of it
- easily extensible base RB tree
- node iterator
- order statistic tree augmentation
- treap
- order statistic tree

###### sal/data/interval.h --- interval tree

###### sal/data/graph.h --- directed and undirected graphs
- adjacency matrix (not really supported)
- adjacency list
- vertex iterator
- adjacency iterator

###### sal/data/graph/search.h --- graph searches
- breadth first search
- depth first search
- recursive depth first search
- visitors for custom actions at certain points of DFS

###### sal/data/utility.h --- important graph algorithms
- topological sort
- check for cycle
- transpose of graph
- strongly connected components




