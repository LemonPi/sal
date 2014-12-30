SAL - simple algorithms library
===

A header only library containing algorithms and data structures implemented <b>simply and efficiently</b>.

<br>
Features
---
### decoupled algorithms so most files are standalone 
#### contest friendly - paste specific functions in without the rest of the library

<br>
### simple implementation of efficient algorithms 
#### not over engineeringed
#### learning friendly - approachable simplicity

<br>
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
###### [sal/algo/numerics.h --- numerics](#numerics)
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
###### sal/algo/numerics.h --- <a name="numerics">numerics</a>
###### sal/algo/perm.h --- <a name="perm">permutation and combination</a>

###### sal/algo/prime.h --- <a name="prime">prime generation and manipulation</a>

###### sal/algo/search.h --- <a name="search">basic searching, substring matching, and finding longest common features</a>

###### sal/algo/sort.h --- <a name="sort">comparison, distributive, and hybrid sorts</a>

###### sal/algo/string.h --- <a name="string">edit distances</a>

###### sal/algo/factorize.h --- <a name="factorize">factorization of integers</a>

###### sal/algo/utility.h --- <a name="utility">utility and testing functions</a>

###### sal/data/list.h --- <a name="list">basic linked list</a>

###### sal/data/matrix.h --- <a name="matrix">2D matrix</a>

###### sal/data/tree.h --- <a name="tree">red black tree and augmentations of it</a>

###### sal/data/interval.h --- <a name="interval">interval tree</a>

###### sal/data/graph.h --- <a name="graph">directed and undirected graphs</a>

###### sal/data/graph/search.h --- <a name="graph_search">graph searches</a>

###### sal/data/utility.h --- <a name="graph_utility">important graph algorithms</a>


