/* Written by Johnson Zhong unless otherwise noted
contact: johnsonzhong@hotmail.ca | johnsonzhong.me
------------------------
prime generation and manipulation

class: Sieve 
next_prime()         -> next prime, remembers last prime asked for
next_prime(guess)    -> next prime after guess
closest_prime(guess) -> prime closest to guess
cur_prime()          -> last prime asked for, or 2
nth_prime(n)         -> nth prime
primes_upto(n)       -> set of primes upto n
is_prime(guess)      -> true if guess is prime, else false
count()              -> number of primes below current prime
count(upper)         -> number of primes below upper 

*/
#pragma once
#include <algorithm>  // binary_search, lower_bound
#include <cmath>	// sqrt, log
#include <iostream>
#include <vector>


namespace sal {

using namespace std;

using small_int = unsigned char;
constexpr size_t L1D_CACHE_SIZE = 32768;
constexpr int APPEND = 1;
constexpr int COUNT = 2;

// modified segmented sieve from 
template <typename big_int = unsigned long long>
class Sieve {
	std::vector<big_int> primes;
	big_int limit;
	big_int n {3};	// next unsieved prime
	size_t segment_size;
	size_t nth_p {0};

	void sieve();
	size_t bit_sieve(big_int limit);

public:
	using value_type = big_int;
	
	Sieve(big_int init_limit = 0, size_t seg_size = L1D_CACHE_SIZE)
		: limit {init_limit}, segment_size {seg_size}
		{}

	void set_limit(big_int l) {limit = l;}

	big_int next_prime() {
		if (++nth_p < primes.size()) return primes[nth_p-1];
		else limit *= 2;	// need to resize
		sieve();
		return primes[nth_p-1];
	}
	// i in all algorithms that follows refers to the ith prime (index)
	// prime after guess
	big_int next_prime(big_int guess) {
		auto i = std::upper_bound(primes.begin(), primes.end(), guess);
		if (i != primes.end()) return *i;
		else if (guess < L1D_CACHE_SIZE) limit = L1D_CACHE_SIZE;
		else                             limit = static_cast<big_int>(guess * 1.2);
		sieve();
		return *std::upper_bound(primes.begin(), primes.end(), guess);
	}

	// prime closest to guess
 	big_int closest_prime(big_int guess) {
		auto i = std::upper_bound(primes.begin(), primes.end(), guess);
		if (i != primes.end()) return (*i - guess < guess - *(i-1))? *i : *(i - 1);
		else if (guess < L1D_CACHE_SIZE) limit = L1D_CACHE_SIZE;
		else                             limit = static_cast<big_int>(guess * 1.2);
		sieve();
		i = std::upper_bound(primes.begin(), primes.end(), guess);
		return (*i - guess < guess - *(i-1))? *i : *(i - 1);
	}

	big_int nth_prime(big_int nth) {
		if (nth < 6) switch(nth) {
			case 1: return 2; case 2: return 3; case 3: return 5; case 4: return 7; case 5: return 11; default: return 0;
		}
		if (nth >= (big_int) primes.size()) {
			limit = nth*(std::log(nth) + std::log(std::log(nth)));	// upper bound for p_n for nth > 5
			sieve();
		}
		return primes[nth-1];
	}

	big_int cur_prime() const { return (nth_p > 0)? primes[nth_p-1] : 2;}

	// reference to already found primes
	const std::vector<big_int>& primes_upto(big_int largest_prime) {
		if (primes.size() && largest_prime < primes.back()) // subset of sieved primes
			return primes;	// note that primes could exceed l
		limit = largest_prime;
		sieve();
		return primes;
	}

	// current prime
	size_t count() {return nth_p;}
	// primes below upper
	size_t count(big_int upper) {return bit_sieve(upper);}
	bool is_prime(big_int guess) {
		// check smooth filters
		if (guess < 8) return (guess == 2 || guess == 3 || guess == 5 || guess == 7);
		// filter out smooth numbers
		if (guess % 2 == 0 || guess % 3 == 0 || guess % 5 == 0 || guess % 7 == 0) return false;
		if (primes.size() && (big_int) guess < primes.back()) {
			return std::binary_search(primes.begin(), primes.end(), guess);
		}
		limit = guess;
		sieve();
		return primes.back() == (big_int) guess;
	}

};

template <typename big_int>
void Sieve<big_int>::sieve() {
	size_t sqrt_limit {static_cast<size_t>(sqrt(limit))};
	std::vector<char> sieve(segment_size);
	std::vector<size_t> next;
	std::vector<char> small_prime(sqrt_limit + 1, 1);
 	// generate small primes <= sqrt_limit
	for (size_t s = 2; s * s <= sqrt_limit; s++)	// start at last small prime left off
		if (small_prime[s])
			for (size_t msp = s * s; msp <= sqrt_limit; msp += s)
				small_prime[msp] = 0;

		size_t sp {2};	// small primes start at 2
		size_t num_sp {0};
		for (big_int low = 0; low <= limit; low += segment_size) {
	 		fill(sieve.begin(), sieve.end(), 1);

			// current segment = interval [low, high]
			big_int high = std::min((big_int)(low + segment_size - 1), limit);

			// store small primes needed to cross off multiples
			for (; (big_int) (sp * sp) <= high; ++sp) {
				if (small_prime[sp]) {
					++num_sp;	// next is kept to size of num_sp
					if (num_sp > primes.size()) primes.push_back(sp);
					next.push_back(sp * sp - low);
				}
			}
			// sieve the current segment
			for (size_t i = 1; i < num_sp; ++i) {
				size_t mul = next[i];
				for (size_t p = primes[i] * 2; mul < segment_size; mul += p)
					sieve[mul] = 0;
				next[i] = mul - segment_size;
			}

			// treat the sieved primes
			for (; n <= high; n += 2)
				if (sieve[n - low] && n > primes.back()) // n is a prime
					primes.push_back(n); 
		}
}

// bit segmented sieve for counting primes, borrowed from primesieve.org
const int unset_bit[16] = {
	~(1 << 0), ~(1 << 0),
	~(1 << 1), ~(1 << 1),
	~(1 << 2), ~(1 << 2),
	~(1 << 3), ~(1 << 3),
	~(1 << 4), ~(1 << 4),
	~(1 << 5), ~(1 << 5),
	~(1 << 6), ~(1 << 6),
	~(1 << 7), ~(1 << 7)
};
const int popcnt[256] = {
	0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4,
	1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
	1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
	2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
	1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
	2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
	2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
	3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
	1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
	2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
	2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
	3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
	2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
	3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
	3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
	4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8
};

template <typename big_int>
size_t Sieve<big_int>::bit_sieve(big_int limit) {
	size_t seg_size = segment_size * 16;
	size_t sqrt_limit = static_cast<size_t>(sqrt(limit));

	big_int count = (limit == 1) ? -1 : 0;
	big_int s {2};
	big_int n {0};
	std::vector<small_int> sieve(segment_size);

	// generate small primes <= sqrt_limit
	std::vector<bool> small_prime(sqrt_limit + 1, true);
	for (size_t i = 2; i * i <= sqrt_limit; i++)
	if (small_prime[i])
		for (size_t j = i * i; j <= sqrt_limit; j += i)
			small_prime[j] = false;

	std::vector<size_t> primes;
	std::vector<size_t> next;

	for (big_int low = 0; low <= limit; low += seg_size) {
		std::fill(sieve.begin(), sieve.end(), 0xff);

		// unset bits > limit
		if (low + seg_size > limit)
			sieve[(limit + 1 - low) >> 4] &= ~(0xff << ((limit + 1) % 16 / 2));

		// current segment = interval [low, high]
		big_int high = min(low + seg_size - 1, limit);

		// store small primes needed to cross off multiples
		for (; s * s <= high; s++) {
			if (small_prime[s]) {
				primes.push_back(s);
				next.push_back(s * s - low);
			}
		}
		// sieve the current segment
		for (size_t i = 1; i < primes.size(); i++) {
			size_t mul = next[i];
			for (size_t p = primes[i] * 2; mul < seg_size; mul += p)
				sieve[mul >> 4] &= unset_bit[mul & 15];
			next[i] = mul - seg_size;
		}

		for (; n <= high; n += 1 << 4)
			count += popcnt[sieve[(n - low) >> 4]];
	}
	return count;
}


} // end namespace sal