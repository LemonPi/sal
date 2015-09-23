#pragma once
#include <vector>

namespace sal {

// represent a 2D grid and the existence of entities
// grid units must be integral (so grids are discrete)
// supports efficient (constant time) lookup and inserts
class Bitgrid {
	std::vector<bool> elems;
	size_t xs, ys;
public:
	using value_type = bool;
	using reference = std::vector<bool>::reference;
	using const_reference = bool;
	Bitgrid() = delete;
	// initialize with the NUMBER of grids in each direction; index with [0, x_blocks-1] and [0, y_blocks-1]
	// grid size is fixed at the start 
	Bitgrid(size_t x_blocks, size_t y_blocks) : elems(x_blocks * y_blocks, 0), xs{x_blocks}, ys{y_blocks} {}

	// element access
	reference at(size_t x, size_t y) {
		return elems[x*ys + y];
	}
	const_reference at(size_t x, size_t y) const {
		return elems[x*ys + y];
	}

	// coordinate inputs are ALWAYS INDICES starting from 0
	// modifying entities on a bitwise level
	void set(size_t x, size_t y) {
		elems[x*ys + y] = 1;
	}
	void clear(size_t x, size_t y) {
		elems[x*ys + y] = 0;
	}

	// returns whether any entity exists in the bounding box given
	bool find(size_t xl, size_t xh, size_t yl, size_t yh) const {
		for (size_t x = xl; x <= xh; ++x) 
			for (size_t y = yl; y <= yh; ++y)
				if (at(x,y)) return true;

		return false;
	}

	// sets all bits to 0
	void reset() {
		for (reference entity : elems) entity = 0;
	}

	void print() const {
		for (size_t x = 0; x < xs; ++x) {
			for (size_t y = 0; y < ys; ++y) {
				std::cout << at(x,y);
			}
			std::cout << std::endl;
		}
	}
};


}	// end namespace sal