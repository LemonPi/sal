/* Written by Johnson Zhong unless otherwise noted
contact: johnsonzhong@hotmail.ca | johnsonzhong.me
------------------------
numerical functions

mean(begin, end) 	-> mean across a sequence
variance(begin, end)-> variance across a sequence

modular_pow(base, exp, mod)   -> base^exp % mod, O(lgexp) operations
int_pow(base, exp) 		      -> base^exp

fibonacci(n)	-> nth fibonacci (requires matrix and infint) in O(lgn) operations

choose(n, k)	-> n choose k

make_cyclic(base, prime) 	-> makes a cyclic number from 1/prime in the base (ex. decimal base 10)
cyclic_length(base, prime)  -> length of cyclic number of 1/prime in the base, 0 if noncyclic

is_pow(guess, base) -> returns true if guess is a pow of base, else false

gcd(a, b) 			-> greatest common denominator of a and b (binary implementation)
gcd_euclidean(a, b) -> euclidean (naive) implementation
gcd_alt(a, b)		-> alternating implementation

totient(num)        -> number of integers less than n that is relatively prime with n 

is_square(num)		-> true if num is a perfect square


factorization of integer numbers

factorize(num)   -> vector holding prime factors of num in sorted ascending order
num_factors(num) -> total number of factors of num, including composites
sum_factors(num) -> sum of all factors of num, including composites

lcm(begin, end)  -> lowest common multiple of a group of numbers

catalan(n) 		 -> nth catalan number 1(0), 1(1), 2(2), 5(3), 14(4), 42(5), 132(6), ...

*/

#pragma once
#include <cmath>	// sqrt, pow, ceil
#include <vector>
#include <iterator>
#include <unordered_map>
#include "../data/matrix.h"
#include "prime.h"	// for totient

namespace sal {

/**
 * Statistics for a sequence for arithmetic value types.
 */
template <typename Iter, bool simpleValueType>
struct SequenceStatsImpl {
    static double mean(Iter begin, Iter end) {
        if (begin == end) {
            return 0;
        }
        double sum = 0;
        for (Iter current = begin; current != end; ++current) {
            sum += (*current);
        }
        return sum / std::distance(begin, end);
    }

    static double variance(Iter begin, Iter end) {
        if (begin == end) {
            return 0;
        }
        double m     = mean(begin, end);
        double sqSum = 0;

        for (Iter current = begin; current != end; ++current) {
            sqSum += (*current) * (*current);
        }
        return sqSum / std::distance(begin, end) - m * m;
    }
};

/**
 * Statistics implementation for non-arithmetic value types.
 * For now, assume the value type has ->second to point to arithmetic value.
 */
template <typename Iter>
struct SequenceStatsImpl<Iter, false> {
    static double mean(Iter begin, Iter end) {
        if (begin == end) {
            return 0;
        }
        double sum = 0;
        for (Iter current = begin; current != end; ++current) {
            sum += current->second;
        }
        return sum / std::distance(begin, end);
    }

    static double variance(Iter begin, Iter end) {
        if (begin == end) {
            return 0;
        }
        double m     = mean(begin, end);
        double sqSum = 0;

        for (Iter current = begin; current != end; ++current) {
            sqSum += current->second * current->second;
        }
        return sqSum / std::distance(begin, end) - m * m;
    }
};

/**
 * Find the mean across a sequence
 *
 * @param[in] begin Start of a sequence
 * @param[in] end One past the last element of a sequence
 */
template <typename Iter>
double mean(Iter begin, Iter end) {
    // call an implementation based on the type that we get by dereferencing iterator
    return SequenceStatsImpl<
        Iter,
        std::is_arithmetic<typename std::iterator_traits<Iter>::value_type>::value>::mean(begin,
                                                                                          end);
}

/**
 * Find the variance across a sequence
 *
 * @param[in] begin Start of a sequence
 * @param[in] end One past the last element of a sequence
 */
template <typename Iter>
double variance(Iter begin, Iter end) {
    // call an implementation based on the type that we get by dereferencing iterator
    return SequenceStatsImpl<
        Iter,
        std::is_arithmetic<typename std::iterator_traits<Iter>::value_type>::value>::variance(begin,
                                                                                              end);
}


// Θ(lg(exponent)) complexity
int modular_pow(int base, int exponent, int modulus) {
    int result {1};
    while (exponent > 0) {
        if (exponent & 1)
            result = (result * base) % modulus;
        exponent >>= 1;
        base = (base * base) % modulus;
    }
    return result;
}

// Θ(lg(exponent)) complexity
int int_pow(int base, int exponent) {
	int result {1};
	while (exponent > 0) {	// doesn't treat negative exponents
		if (exponent & 1)
			result *= base;
		exponent >>= 1;
		base *= base;
	}
	return result;
}

// returns nth fibonacci number in Θ(lg(n))` time using matrix exponentiation
template <typename T>
T fibonacci(size_t n) {	
	Matrix<T> f {{1, 1},{1, 0}};
	f.pow(n);
	return f.get(0,1);	// either upper right or lower left is Fib_n
}

size_t choose(size_t n, size_t k) {
	// (n, k) = (n, n - k) allows a lower number of loops
	if (k > n - k) k = n -k;

	// start from (n - k + 1, 1) using the identity (n,k) = n/k (n-1, k-1)
	n = n - k + 1;
	// (n, 1) = n
	size_t res {n};
	++n;
	for (size_t prev_k = 2; prev_k <= k; ++prev_k, ++n) {
		res = (n * res) / prev_k;
	}
	return res;
}

// O(prime) complexity
size_t make_cyclic(int base, int prime) {
	int rem {1};
	size_t num {0};
	int turn {1};
	for (;turn < prime; ++turn) {
		// previous remainder advances 1 digit (ex. 7 in base 10 becomes 70)
		int block = rem * base;
		int digit = block / prime;
		rem = block % prime;
		num = num*base + digit;	// collect digit
		if (rem == 1) break;
	}
	if (turn == prime - 1) return num;
	return 0;	// not cyclic
}

// same as make_cyclic
size_t cyclic_length(int base, int prime) {
	int rem {1};
	size_t length {};
	int turn {1};
	for (;turn < prime; ++turn) {
		int block = rem * base;
		rem = block % prime;
		++length;
		if (rem == 1) break;
	}
	if (turn == prime - 1) return length;
	return 0;	// not cyclic	
}

// O(lg(a)/lg(b))
bool is_pow(int guess, int base) {	// true if guess is an integer power of base
	while (guess % base == 0) guess /= base;
	return guess == 1;
}
// O(M(n)) where M(n) is the complexity of the multiplication algorithm used
// taken from maaartinus from stackoverflow, great algorithm!
constexpr long long square_mask = 0xC840C04048404040;

bool is_square(long guess) {
    // This tests if the 6 least significant bits are right.
    // Moving the to be tested bit to the highest position saves us masking.
    if ((square_mask << guess) >= 0) return false;
    int trailing_zeroes = __builtin_ctzll(guess);
    // Each square ends with an even number of zeros.
    if ((trailing_zeroes & 1) != 0) return false;
    guess >>= trailing_zeroes;
    // Now guess is either 0 or odd.
    // In binary each odd square ends with 001.
    // Postpone the sign test until now; handle zero in the branch.
    if (((guess & 7) != 1) | (guess <= 0)) return guess == 0;
    // Do it in the classical way.
    // The correctness is not trivial as the conversion from long to double is lossy!
    long tst = static_cast<long>(std::sqrt(guess));
    return tst * tst == guess;
}


// gcd(a,b) is the smallest positive element of {ax + by | x,y E Z}
// (linear combination of a and b)
unsigned int gcd(int a, int b) { // binary implementation
	if (!a) return b;
	if (!b) return a;
	if (a < 0) a = -a;
	if (b < 0) b = -b;
	unsigned char shift {};
	// find greatest power of 2 dividing both a and b
	while (((a | b) & 1) == 0) {
		++shift;
		a >>= 1;
		b >>= 1;
	}
	while ((a & 1) == 0) a >>= 1;
	// a is now odd
	do {
		while ((b & 1) == 0) b >>= 1;	// remove all powers of 2
		// both odd now
		if (b < a) std::swap(a, b);	// keep a < b
		b -= a;
	} while (b != 0);

	return a << shift;
}
// O(lg(ab)) complexity
unsigned int gcd_euclidean(int a, int b) {	
	while (a & b) {
		if (a < b) a %= b;
		else 	   b %= a;
	}
	return a + b;
}
// also using Euclidean equation, but recursive
unsigned int gcd_alt(int a, int b) {
	if (b == 0) return a;
	return gcd_alt(b, a % b);
}

template <typename Item, typename Indexable>
struct mul_impl {
	static Item mul(const Indexable& items) {
		Item res {items[0]};
		for (size_t i = 1; i != items.size(); ++i) 
			res *= items[i];
		return res;
	}
};

// specialization of mul for matrices, matrix chain multiplication ordering
// Θ(n^3)
template <typename Indexable, typename T>
struct mul_impl<Matrix<T>, Indexable> {
	static Matrix<T> mul(const Indexable& mats) {
		size_t n {mats.size()};
		size_t min[n][n];
		std::vector<size_t> split_at(n*n);

		for (size_t trivial = 0; trivial < n; ++trivial)
			min[trivial][trivial] = 0;

		for (size_t chain_len = 2; chain_len <= n; ++chain_len) {
			for (size_t start = 0; start < n - chain_len+1; ++start) {
				size_t end {start + chain_len - 1};
				min[start][end] = std::numeric_limits<size_t>::max();

				for (size_t split = start; split < end; ++split) {
					size_t cost {min[start][split] + min[split + 1][end] + 
						mats[start].row() * mats[split].col() * mats[end].col()};

					if (cost < min[start][end]) {
						min[start][end] = cost;
						split_at[start*n + end] = split;
					}
				}
			}
		}

		// min holds # of scalar multiples
		// can use split_at to find the actual product
		return mul_trace(mats, split_at, 0, n-1);
	}

	template <typename Splits>
	static Matrix<T> mul_trace(const Indexable& mats, const Splits& split_at, size_t start, size_t end) {
		if (start == end) return mats[start];
		if (end - start == 1) return mats[start] * mats[end];
		else return mul_trace(mats, split_at, start, split_at[start*mats.size() + end]) * 
					mul_trace(mats, split_at, split_at[start*mats.size() + end] + 1, end);
	}

};

template <typename Indexable>
typename Indexable::value_type mul(const Indexable& items) {
	return mul_impl<typename Indexable::value_type, Indexable>::mul(items);
}



// useful for ex. finding ones digit of 7^222 aka 7^222 mod 10
// 7 and 10 coprime, so 7^phi(10) = 7^4 = 1 mod 10
// 7^222 = (7^4)^55 * 7^2 = 1^55 * 7^2 = 9 mod 10
using big_int = unsigned long long;
big_int totient(big_int n) {
	if (n < 2) return 0;  // base case
	Sieve<> s {(big_int) n};
	if (s.is_prime(n)) return n-1;  // Lehmer's conjecture

	// even number
	if ((n & 1) == 0) {
		big_int nn {(big_int) n >> 1};
		return (nn & 1)? totient(nn) : totient(nn) << 1; 
	}

	while (true) {
		big_int prime = s.next_prime();
		if (n % prime) continue;  
		// prime factor for n
		big_int left {n / prime};
		big_int divisor {gcd(prime, left)}; 
		// coprime to whatever's left?
		// ex. t(36) = t(2^2) * t(3^2) = t(2) * t(3)
		return (divisor == 1)? totient(prime)*totient(left) :
		totient(prime)*totient(left) * divisor/totient(divisor);
	}
}

auto phi = totient; // alias for totient


template <typename T>
std::vector<T> factorize(T num) {	// works great for smooth numbers (small factors)
	std::vector<T> factors;
	if (num < 4) { factors.push_back(num); return factors; }
	while ((num & 1) == 0) { factors.push_back(2); num >>= 1; }	// remove all factors of 2 
	T d {3};
	while (num >= d * d) {
		while (num % d == 0) {	// remove all repeats of this divisor
			factors.push_back(d);
			num /= d;
		}
		d += 2;
		if (d * d > num && num > 1) { factors.push_back(num); return factors; }
	}
	return factors;
}

// prime factorization of numbers with large prime factors
template <typename T>
std::vector<T> factorize_rough(T num) {
	std::vector<T> factors;

	if (num < 4) { factors.push_back(num); return factors; }
	while ((num & 1) == 0) { factors.push_back(2); num >>= 1; }	// remove all factors of 2 

	// prime factors can be at most sqrt(num)
	T max_prime {static_cast<T>((std::ceil(std::sqrt(num))))};
	static Sieve<T> sieve;

	const auto& primes = sieve.primes_upto(max_prime);

	// skip 2
	for (size_t i = 1; primes[i] <= max_prime;) {
		while (num % primes[i] == 0) {
			factors.push_back(primes[i]);
			num /= primes[i];
		}
		++i;
		if ((i == primes.size() || primes[i] > max_prime) && num > 1) { factors.push_back(num); return factors; }
	}
	return factors;
}

size_t num_factors(size_t num) {	// total number of factors (including composites)
	// product of the power of every unique factor + 1 (for power of 0)
	std::vector<size_t> p_factors {factorize(num)};
    if (p_factors.size() == 0) return 1;    // is 0 or 1 (or negative)
	size_t tot_factors {1};

	for (auto i = p_factors.begin(); i != p_factors.end();) {
		size_t factor {*i};		
		size_t powers_of {2};	// ex. 2^0, 2^1, 2^2 would be 3, +1 to account for ^0
		while (++i != p_factors.end() && factor == *i) ++powers_of;

		tot_factors *= powers_of;
	}
	return tot_factors;
}

size_t sum_factors(size_t num) {
	std::vector<size_t> p_factors {factorize(num)};
	if (p_factors.size() == 0) return (num)? 1 : 0;	// is 0 or 1
	size_t sum_factors {1};
	// sum of factors is product of the factor sums
	// ex. 120 = 2^3 * 3^1 * 5^1
	// sum factors (1 + 2 + 2^2 + 2^3) * (1 + 3) * (1 + 5)
	for (auto i = p_factors.begin(); i != p_factors.end();) {
		size_t factor {*i};
		size_t factor_sum {1 + factor};	// (1 + 3) would end here and not go through loop
		size_t powers_of {1};
		while (++i != p_factors.end() && factor == *i) factor_sum += std::pow(factor, ++powers_of);
		sum_factors *= factor_sum;
	}
	return sum_factors;
}

// lowest common multiple of a group of numbers
template <typename Iter>
typename std::iterator_traits<Iter>::value_type lcm(Iter begin, Iter end) {
	using T = typename std::iterator_traits<Iter>::value_type;
	std::unordered_map<T,size_t> factors_union;

	for (;begin != end; ++begin) {
		vector<T> factors {factorize(*begin)};
		for (auto i = factors.begin(); i != factors.end();) {
			size_t conseq {1};
			T first_elem {*i};
			while (++i != factors.end() && *i == first_elem) ++conseq;
			// 0 if it wasn't created yet
			factors_union[first_elem] = std::max(conseq, factors_union[first_elem]);
		}
	}
	// gather up all factors
	T least_multiple {1};
	for (const auto& factor : factors_union) {
		// key is factor, value is max number of occurrances
		least_multiple *= std::pow(factor.first, factor.second);
	}
	return least_multiple;
}

// occurs in counting problems like number of possible BST with n unique keys, valid parenthesis pairing counting
// C_0 = 1, C_n+1 = sum(i=0 to n) {C_i * C_n-i}, alternatively C_n = 1/(n + 1) * (2n choose n)
// alternatively C_n+1 = 2(2n + 1)/(n + 2) * Cn
size_t catalan(size_t n) {
	size_t res = 1;	// C_0 = 1
	for (size_t i = 1; i < n; ++i)
		res = (2 * (2*i + 1) * res) / (i + 2);
	return res;
}

}