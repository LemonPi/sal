#pragma once

#include <cassert>
#include <memory>	// unintialized_copy, get_temporary_buffer, return_temporary_buffer
#include <vector>
#include "simple_sorts.h"	// lin_sort

#define A_RUN (pending[mid - 1].len)
#define B_RUN (pending[mid].len)
#define C_RUN (pending[mid + 1].len)

namespace sal {

// Timsort, much better than other sorts on nearly sorted
// O(nlgn) time

template <typename Iter>
class Timsort {
	using T = typename Iter::value_type;
	using D = typename Iter::difference_type;

	// Run characterized by start and length
	struct Run {
		Iter start;
		D len;
		Run(Iter s, D l) : start{s}, len{l} {}
		friend std::ostream& operator<<(std::ostream& os, const Run& r) {
			os << *r.start << '(' << r.len << ')';
			return os;
		} 
	};

	// constant parameters and state ------
	std::vector<T> temp;			// temporary used to hold smaller of A and B during merge
	std::vector<Run> pending;		// runs awaiting merge
	D min_gallop;				// local gallop limit, init to MIN_GALLOP
	static constexpr D MIN_MERGE {32};
	static constexpr D MIN_GALLOP {7};	// initial threshold for galloping

	// main sort --------------------------
	static void sort(Iter begin, Iter end) {
		assert(begin <= end);
		D elems_left {end - begin};
		if (elems_left < 2) return;	// 0 or 1 elements base case already sorted
		if (elems_left < MIN_MERGE) {	// becomes glorified insertion sort
			const D init_run {find_run(begin, end)};
			lin_sort(begin, end, begin + init_run);	// init_run already sorted
			return;
		}
		// else multiple runs exist
		Timsort<Iter> sorter {};
		const D min_run {compute_minrun(elems_left)};
		auto cur = begin;
		do {
			D cur_run {find_run(cur, end)};
			// force into a min_run
			if (cur_run < min_run) {
				D left {std::min(elems_left, min_run)};
				lin_sort(cur, cur + left, cur + cur_run);
				cur_run = left; 
			}

			sorter.push_run(cur, cur_run);
			sorter.collapse();	// merge according to invariants

			cur 	   += cur_run;
			elems_left -= cur_run;
		} while (elems_left != 0);

		assert(cur == end);
		// collapse all remaining runs
		sorter.force_collapse();
		assert(sorter.pending.size() == 1);
	}

	// helpers ----------------------------
	// want minrun size st. N/minrun is <= pow2 for even merging
	static D compute_minrun(D n) {
		assert(n >= 0);
		D r {};	// become 1 if any 1 bit below top 8 bits
		while (n >= MIN_MERGE) {
			r |= n & 1;
			n >>= 1;
		}
		return n + r;
	}

	// find length of next run and make ascending if descending
	static D find_run(const Iter begin, const Iter end) {
		assert(begin < end);
		auto run_end = begin + 1;
		if (run_end == end) return 1;
		if (*run_end < *begin) {	// descending run
			while (run_end != end && *run_end < *(run_end - 1)) ++run_end;
			std::reverse(begin, run_end);	// make ascending 
		}
		else 	// ascending run
			while (run_end != end && *run_end >= *(run_end - 1)) ++run_end;
		
		return run_end - begin;
	}

	void push_run(const Iter start, D length) { pending.emplace_back(start, length); }

	// checks pending for runs waiting to be merged, merge adjacent runs until invariants re-established 
	void collapse() {
		while (pending.size() > 1) {
			D mid = pending.size() - 2;	// prepare to select 3 runs, A B C, mid is middle index
			// A <= B + C  -->  A > B + C
			if (mid > 0 && A_RUN <= (B_RUN + C_RUN)) {
				if (A_RUN < C_RUN) --mid;
				merge_run(mid);
			}
			// B < C       -->  B > C
			else if (B_RUN <= C_RUN) 
				merge_run(mid);
			else break;	// satisfied both invariants
		}
	}
	void force_collapse() {
		while (pending.size() > 1) {
			D mid = pending.size() - 2;
			if (mid > 0 && A_RUN < C_RUN) --mid;
			merge_run(mid);
		}
	}

	// ------- Merging
	// compare A[0] to B[0], placing smaller one to the merge area
	// keep track of how many consecutive picks from one run, indicative of clustering
	// enter gallop mode when count reaches MIN_GALLOP

	// gallop mode search B for where A[0] belongs,
	// move B elems before that point to the merge area, then move A[0] to merge area
	// then alternate search A for where B[0] belongs,
	// move slice of A into merge area, etc...
	// stay in gallop mode until slices to copy < MIN_GALLOP elements
	// MIN_GALLOP can be adjusted higher when it's not paying off, or down when it is
	// merge runs at indices run and run+1 (adjacent) 
	void merge_run(const D run) {
		
		assert(pending.size() >= 2);
		assert(run >= 0);
		assert(run == (D)pending.size() - 2 || run == (D)pending.size() - 3);

		Iter start_a {pending[run].start};
		D len_a {pending[run].len};
		Iter start_b {pending[run+1].start};
		D len_b {pending[run+1].len};

		assert(len_a > 0 && len_b > 0);
		assert(start_a + len_a == start_b);

		pending[run].len = len_a + len_b;
		if (run == (D)pending.size() - 3)	// merging A and B
			pending[run + 1] = pending[run + 2];	// slide over last run, uninvolved in merge
		pending.pop_back();

		// find where B[0] starts in A, elements in A before that are already in place
		const D cross_start {gallop_r(*start_b, start_a, len_a, 0)};
		assert(cross_start >= 0);
		start_a += cross_start;
		len_a 	-= cross_start;
		if (len_a == 0) return;
		
		// find where A[-1] ends in B, elements in B after that are already in place
		len_b = gallop_l(*(start_a + (len_a - 1)), start_b, len_b, len_b - 1);
		assert(len_b >= 0);
		if (len_b == 0) return;

		// merge remaining runs using temp with size = min(len_a, len_b)
		if (len_a <= len_b) merge_low(start_a, len_a, start_b, len_b);
		else merge_high(start_a, len_a, start_b, len_b);
	}

	// A <= B
	// copy A to temp array, then merge left to right back into where A lived
	// if B is empty first, or exception raised, copy rest of temp back
	void merge_low(const Iter start_a, D len_a, const Iter start_b, D len_b) {
		
		assert(len_a > 0 && len_b > 0 && (start_a + len_a == start_b));
		make_temp(start_a, len_a);
		Iter cur_a {temp.begin()};
		Iter cur_b {start_b};
		Iter dest {start_a};

		*(dest++) = *(cur_b++);
		if (--len_b == 0) {
			std::copy(cur_a, cur_a + len_a, dest);
			return;
		}
		if (len_a == 1) {
			std::copy(cur_b, cur_b + len_b, dest);
			dest[len_b] = *cur_a;	// last element of A is last
			return;
		}

		while (true) {
			// consecutive wins
			size_t score_a {0};
			size_t score_b {0};
			// merge one pair at a time until min_gallop reached
			do {
				assert(len_a > 1 && len_b > 0);
				if (*cur_b < *cur_a) { // B win
					*(dest++) = *(cur_b++);
					++score_b;
					score_a = 0;
					if (--len_b == 0) goto release;
				}
				else {	// A win
					*(dest++) = *(cur_a++);
					++score_a;
					score_b = 0;
					if (--len_a == 1) goto release;
				}
			} while ((D)(score_a | score_b) < min_gallop);
			
			// gallop mode
			++min_gallop;
			do {
				assert(len_a > 1 && len_b > 0);
				--min_gallop;
				// gallop for B[0] in A
				
				score_a = gallop_r(*cur_b, cur_a, len_a, 0);
				if (score_a) {
					std::copy_backward(cur_a, cur_a + score_a, dest + score_a);
					dest  += score_a;
					cur_a += score_a;
					len_a -= score_a;
					if (len_a <= 1) goto release;
				}
				*(dest++) = *(cur_b++);
				if (--len_b == 0) goto release;
				// gallop for A[0] in B
				
				score_b = gallop_l(*cur_a, cur_b, len_b, 0);
				if (score_b) {
					std::copy(cur_b, cur_b + score_b, dest);
					dest  += score_b;
					cur_b += score_b;
					len_b -= score_b;
					if (len_b == 0) goto release;
				}
				*(dest++) = *(cur_a++);
				if (--len_a == 1) goto release;

			} while (score_a >= MIN_GALLOP || score_b >= MIN_GALLOP);
			++min_gallop;		// penalty for leaving gallop
			if (min_gallop < 1) min_gallop = 1;
		}

		release:
		if (len_a == 1) {
			assert(len_b > 0);
			std::copy(cur_b, cur_b + len_b, dest);
			dest[len_b] = *cur_a;
		}
		else {	// error, copies over rest of A
			assert(len_a > 1);
			assert(len_b == 0);
			std::copy(cur_a, cur_a + len_a, dest);
		}
	}

	// merge_hi deals with B < A
	// merge right to left, copy B into temp and start stores at right end of where B lived
	void merge_high(const Iter start_a, D len_a, const Iter start_b, D len_b) {
        assert( len_a > 0 && len_b > 0 && start_a + len_a == start_b );
        
		make_temp(start_b, len_b);	// b is temp since it's smaller
		Iter cur_a {start_a + (len_a - 1)};
		Iter cur_b {temp.begin() + (len_b - 1)};
		Iter dest {start_b + (len_b - 1)};

		*(dest--) = *(cur_a--);
		if (--len_a == 0) {
			std::copy(temp.begin(), temp.begin() + len_b, dest - (len_b - 1));
			return; 
		}
		if (len_b == 1) {
			dest  -= len_a;
			cur_a -= len_a;
			std::copy_backward(cur_a + 1, cur_a + (len_a + 1), dest + (len_a + 1));
			*dest = *cur_b;	// last element of B is last
			return;
		}

		while (true) {
			// consecutive wins
			size_t score_a {0};
			size_t score_b {0};
			
			// merge one pair at a time until min_gallop reached
			do {
				assert(len_a > 0 && len_b > 1);
				
				if (*cur_b < *cur_a) {	// A win
					*(dest--) = *(cur_a--);
					++score_a;
					score_b = 0;
					if (--len_a == 0) goto release; // success
				}
				else { // B win
					*(dest--) = *(cur_b--);
					++score_b;
					score_a = 0;
					if (--len_b == 1) goto release; // copy A over
				}
			} while ((D)(score_a | score_b) < min_gallop);
			// gallop mode
			
			++min_gallop;
			do {
				assert(len_a > 0 && len_b > 1);
				--min_gallop;
				// gallop for B[0] in A
				
				score_a = len_a - gallop_r(*cur_b, start_a, len_a, len_a - 1);
				if (score_a) {
					std::copy_backward(cur_a - score_a + 1, cur_a + 1, dest + 1);
					dest  -= score_a;
					cur_a -= score_a;
					len_a -= score_a;
					if (len_a == 0) goto release;// success
				}
				*(dest--) = *(cur_b--);
				if (--len_b == 1) goto release; // copy A over
				// gallop for A[0] in B
				
				score_b = len_b - gallop_l(*cur_a, temp.begin(), len_b, len_b - 1);  
				if (score_b) {
					dest  -= score_b;
					cur_b -= score_b;
					len_b -= score_b;
					std::copy(cur_b + 1, cur_b + (score_b + 1), dest + 1);
					if (len_b <= 1) goto release;
				}
				*(dest--) = *(cur_a--);
				if (--len_a == 0) goto release;
			} while (score_a >= MIN_GALLOP || score_b >= MIN_GALLOP);
			++min_gallop;		// penalty for leaving gallop
			if (min_gallop < 1) min_gallop = 1;
		}

		release:
		if (len_b == 1) {
			assert(len_a > 0);
			dest  -= len_a;
			cur_a -= len_a;
			std::copy_backward(cur_a + 1, cur_a + (len_a + 1), dest + (len_a + 1));
			*dest = *cur_b;
		}
		else {	// error, copies over rest of B
			assert(len_a == 0);
			assert(len_b > 1);
			std::copy(temp.begin(), temp.begin() + len_b, dest - (len_b - 1));
		}
	}

	// remember to std::return_temporary_buffer after
	void make_temp(const Iter start, D len) {
		temp.clear();
		temp.reserve(len);
		std::copy(start, start+len, std::back_inserter(temp));
	}

	// ------- Galloping
	// compare A[0] to B[0], B[1], B[3] .. B[2**j - 1] until B[2**(k-1) - 1] < A[0] <= B[2**k - 1]
	// lg(B) comparisons, favours finding right spot in B
	// uncertain region reduced to 2**(k-1) - 1
	// at most 2*lg(B) comparisons vs ceil(lg(B+1))
	// cuts comparisons from O(B) to O(lgB) for clustered data due to long winning runs
	// galloping wins when consecutive run >= 6
	// therefore init MIN_GALLOP = 7
	void gallop_shift(D& offset, D& offset_prev, const D& offset_max) {
		offset_prev = offset;
		offset = (offset << 1) + 1;	// pow2 + 1
		if (offset <= 0) offset = offset_max;	// overflow		
	}

	// finds index s.t. A[index-1] < key <= A[index], start is start of A (a run)
	D gallop_l(T key, Iter start, D len, D hint) {		
		assert(len > 0 && hint >= 0 && hint < len);
		D offset {1};
		D offset_prev {};
		start += hint; 
		if (key > *start) {
			// A[hint] < key, gallop right until a[offset + offset_prev] < key <= A[hint + offset] 
			const D offset_max {len - hint};
			while (offset < offset_max && key > *(start + offset)) 
				gallop_shift(offset, offset_prev, offset_max);
			if (offset > offset_max) offset = offset_max;	// overshot
			// translate by hint
			offset_prev += hint;
			offset	 	+= hint;
		}
		else {
			// A[hint] >= key, gallop left until a[hint - offset] < key <= A[hint - offset_prev] 
			const D offset_max {hint + 1};
			while (offset < offset_max && key <= *(start - offset))
				gallop_shift(offset, offset_prev, offset_max);
			if (offset > offset_max) offset = offset_max;
			// translating by hint
			D t {offset_prev};
			offset_prev = hint - offset;
			offset 		= hint - t;
		}
		start -= hint;
		// A[offset_prev] < key <= A[offset], binary search
		assert(-1 <= offset_prev && offset_prev < offset && offset <= len);
		return std::lower_bound(start + (offset_prev+1), start + offset, key) - start;
	}
	// like gallop_l but A[index-1] <= key < A[index], so use upper bound
	D gallop_r(T key, Iter start, D len, D hint) {
		assert(len > 0 && hint >= 0 && hint < len);
		D offset {1};
		D offset_prev {};
		start += hint;
		if (key < *start) {
			// A[hint] > key, gallop left until a[hint - offset] <= key < A[hint - offset_prev] 
			const D offset_max {hint + 1};
			while (offset < offset_max && key < *(start - offset)) 
				gallop_shift(offset, offset_prev, offset_max);
			if (offset > offset_max) offset = offset_max;
			// translating by hint
			D t {offset_prev};
			offset_prev = hint - offset;
			offset 		= hint - t;
		}
		else {
			// A[hint] <= key, gallop right until a[offset + offset_prev] <= key < A[hint + offset] 
			const D offset_max {len - hint};
			while (offset < offset_max && key >= *(start + offset))
				gallop_shift(offset, offset_prev, offset_max);
			if (offset > offset_max) offset = offset_max;	// overshot
			// translate by hint
			offset_prev += hint;
			offset	 	+= hint;
		}
		start -= hint;
		// A[offset_prev] < key <= A[offset], binary search
		assert(-1 <= offset_prev && offset_prev < offset && offset <= len);
		return std::upper_bound(start + (offset_prev+1), start + offset, key) - start;
	}

	// constructor min_gallop
	Timsort() : min_gallop{MIN_GALLOP} {}

	// interface  -------------------------
	template <typename Iter_>
	friend void tim_sort(Iter_ begin, Iter_ end);
};

template <typename Iter>
void tim_sort(const Iter begin, const Iter end) {
	Timsort<Iter>::sort(begin, end);
}

}