SAL - simple algorithms library
===

A header only library containing algorithms and data structures implemented <b>simply and efficiently</b>.

Features
---
- decoupled algorithms so most files are standalone (most functions are as well!)
  - contest friendly - can copy paste most functions in without pulling in an entire library
- simple implementation of efficient algorithms; not over engineeringed
  - learning friendly - approachable simplicity

Motivation
---
I enjoyed some features of the boost library (namely variant), but found the library to be too cumbersome as a whole.  
Using the library in a set up environment was fine, but it was impossible to understand conceptual slices of it.  
It wasn't the best resource to learn the concepts from, and you couldn't (in most cases) use parts of it for contests.  

In response, the focus for this library is on the concepts rather than the implementation

Table of contents
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
- bubble sort
- 
