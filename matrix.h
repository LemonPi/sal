#pragma once
#include <iostream>
#include <iomanip>
#include <vector>
#include <set>
#include <stdexcept>
#include <initializer_list>
#include <limits>	// numeric limits
#include "../algo/utility.h"	// Rand int
#include "../algo/macros.h"		// POS_INF

using namespace std;

namespace sal {

template <typename T>
class Matrix {
	vector<T> elems;
	size_t rows, cols;

	void rotate_copy();
public:
	Matrix() : rows{0}, cols{0} {};
	Matrix(size_t r, size_t c, T d = 0) : elems(r*c, d), rows{r}, cols{c}  {}	// default init to 0
	Matrix(size_t r, size_t c, vector<T>&& e) : elems{std::move(e)}, rows{r}, cols{c}  {}
	Matrix(initializer_list<initializer_list<T>> a);
	Matrix(const Matrix& a) : elems{a.elems}, rows{a.row()}, cols{a.col()}  {}
	Matrix(Matrix&& a) : elems{std::move(a.elems)}, rows{a.row()}, cols{a.col()}  {}
	Matrix<T>& operator=(const Matrix& a) {
		rows = a.rows; cols = a.cols;
		elems = a.elems;
		return *this;
	}
	Matrix<T>& operator=(Matrix&& a) {
		rows = a.rows; cols = a.cols;
		elems = move(a.elems);
		return *this;
	}
	size_t row() const { return rows; }
	size_t col() const { return cols; }

	void resize_rows(size_t new_rows, T def = 0) {
		elems.resize(new_rows, def);
	}

	void resize(size_t new_rows, size_t new_cols, T def = 0) {
		// simply resize elems
		if (new_cols == cols) return resize_rows(new_rows, def);
		size_t row_max {min(rows, new_rows)};
		size_t col_max {min(cols, new_cols)};
		vector<T> new_elems(new_rows * new_cols, def);
		for (size_t r = 0; r < row_max; ++r) {
			for (size_t c = 0; c < col_max; ++c) {
				// start of row to end of future column
				std::copy(elems.begin() + r*cols, elems.begin() + r*cols + col_max, 
					new_elems.begin() + r*new_cols);
			}
		}
		elems = std::move(new_elems);
		rows = new_rows;
		cols = new_cols;
	}


	T& get(size_t row, size_t col) { return elems[row*cols + col]; }
	const T& get (size_t row, size_t col) const {return elems[row*cols + col]; }

	void rotate(); // clockwise

	void clear_zero();

	void print() const {std::cout << *this;}

	// convenient row and col operations
	template <typename Op>
	T row_op(size_t row, Op&& op, T res = 0) {
		for (size_t col = 0; col < cols; ++col)
			op(res, elems[row*cols + col]);
		return res;
	}
	template <typename Op>
	T col_op(size_t col, Op&& op, T res = 0) {
		for (size_t row = 0; row < rows; ++row)
			op(res, elems[row*cols + col]);
		return res;
	}
	T row_sum(size_t row) const {
		row_op(row, [](T& res, const T& elem){res += elem;}, 0);
	}
	T col_sum(size_t col) const {
		col_op(col, [](T& res, const T& elem){res += elem;}, 0);
	}
	T row_prod(size_t row) const {
		row_op(row, [](T& res, const T& elem){res *= elem;}, 0);
	}
	T col_prod(size_t col) const {
		col_op(col, [](T& res, const T& elem){res *= elem;}, 0);
	}	

	// essential operators
	Matrix<T>& operator*=(T);
	Matrix<T>& operator*=(const Matrix&);
	Matrix<T>& operator+=(const Matrix&);
	Matrix<T>& operator-=(const Matrix&);
	Matrix<T>& pow(size_t exponent);

	bool operator==(const Matrix&) const;

	template <typename TT>
	friend ostream& operator<<(ostream&, const Matrix<TT>&);
};
// creation of matrices
template <typename T>
Matrix<T> identity(size_t size) {
	vector<T> elems(size*size, 0);
	Matrix<T> id {size, size, move(elems)};
	for (size_t i = 0; i < size; ++i) {
		id.get(i,i) = 1;
	}
	return id;
}
template <typename T>
Matrix<T> random_matrix(size_t row, size_t col, 
	T min = std::numeric_limits<T>::min(), 
	T max = std::numeric_limits<T>::max()) {
	Rand_int die{min, max};
    vector<T> elems;
    elems.reserve(row * col);
    for (int i = 0; i < row * col; ++i)
        elems.push_back(static_cast<T>(die()));

    return Matrix<T>(row, col, std::move(elems));
}

template <typename T>
Matrix<T>::Matrix(initializer_list<initializer_list<T>> a) : rows{a.size()}, cols{a.size() ? a.begin()->size() : 0} {
	elems.reserve(rows * cols);
	for (const auto& r : a) 
		for (const auto& c : r) 
			elems.push_back(c);
}

template <typename T>
void Matrix<T>::rotate() {	// clockwise
	if (rows != cols) {return rotate_copy();}	// else rotate in place for squares
	int n = rows;	// rows == cols
	for (int layer = 0; layer < n / 2; ++layer) {
		int first = layer;
		int last = n - 1 - layer;
		for (int i = first; i < last; ++i) {
			int offset = i - first;

			// save top
			T top = elems[first*cols + i];
			// left -> top
			elems[first*cols + i] = elems[(last - offset)*cols + first];
			// bot -> left
			elems[(last - offset)*cols + first] = elems[last*cols + (last - offset)];
			// right -> bot
			elems[last*cols + (last - offset)] = elems[i*cols + last];
			// top -> right
			elems[i*cols + last] = top;
		}
	}
}

template <typename T>
void Matrix<T>::rotate_copy() {	// clockwise
	std::vector<T> new_elems;
	new_elems.reserve(elems.size());
	// go up from the bottom of each column to become new row
	for (size_t c = 0; c < cols; ++c) 
		for (size_t r = rows; r != 0; --r)
			new_elems.push_back(elems[(r - 1)*cols + c]);
		
	std::swap(cols, rows);
	elems = std::move(new_elems);
}

template <typename T>
void Matrix<T>::clear_zero() {
	set<size_t> rows_to_clear;
	set<size_t> cols_to_clear;
	for (size_t i = 0; i < rows; ++i) {
		for (size_t j = 0; j < cols; ++j) {
			if (elems[i*cols + j] == 0) {
				rows_to_clear.insert(i);
				cols_to_clear.insert(j);
			}
		}
	}
	for (size_t row : rows_to_clear) {
		for (int j = 0; j < cols; ++j) elems[row*cols + j] = 0;
	}
	for (size_t col : cols_to_clear) {
		for (int i = 0; i < rows; ++i) elems[i*cols + col] = 0;
	}
}

template <typename T>
Matrix<T>& Matrix<T>::operator*=(T scalar) {
	for (T elem : elems) elems *= scalar;
	return *this;
}
template <typename T>
Matrix<T>& Matrix<T>::operator*=(const Matrix& a) {
	// m x n times n x p --> m x p
	if (cols != a.rows) throw runtime_error("Invalid dimensions for matrix multiplication");
	vector<T> newelems;
	newelems.reserve(rows*a.cols);
	// naive O(n^3) algorithm, which works well for small matrices
	for (size_t i = 0; i < rows; ++i) {
		for (size_t j = 0; j < a.cols; ++j) {
			T elem {0};
			for (size_t k = 0; k < cols; ++k) {
				elem += get(i, k) * a.get(k, j);
			}
			newelems.push_back(elem);
		}
	}
	elems = newelems;
	cols = a.cols;
	return *this;
}

template <typename T>
Matrix<T>& Matrix<T>::operator+=(const Matrix& a) {
	if (rows != a.rows || cols != a.cols) throw runtime_error("Invalid dimensions for matrix addition");
	for (int i = 0; i < elems.size(); ++i) elems[i] += a.elems[i];
	return *this;
}
template <typename T>
Matrix<T>& Matrix<T>::operator-=(const Matrix& a) {
	if (rows != a.rows || cols != a.cols) throw runtime_error("Invalid dimensions for matrix subtraction");
	for (int i = 0; i < elems.size(); ++i) elems[i] -= a.elems[i];
	return *this;
}
template <typename T>
Matrix<T>& Matrix<T>::pow(size_t exponent) {
	Matrix<T> res {identity<T>(rows)};
	while (exponent > 0) {
		if (exponent & 1)	// odd exponent
			res *= *this;
		exponent >>= 1;
		*this *= *this;
	}
	*this = res;
	return *this;
}


template <typename T>
bool Matrix<T>::operator==(const Matrix& a) const {
	if (rows != a.rows || cols != a.cols) return false;
	for (size_t i = 0; i < elems.size(); ++i)
		if (elems[i] != a.elems[i]) return false;
	return true;
}

template <typename T>
bool operator!=(const Matrix<T>& a, const Matrix<T>& b) {
	return !(a == b);
}

template <typename T>
Matrix<T> operator*(const Matrix<T>& a, const Matrix<T>& b) {
	Matrix<T> ret {a};
	ret *= b;
	return ret;
}
template <typename T>
Matrix<T> operator+(const Matrix<T>& a, const Matrix<T>& b) {
	Matrix<T> ret {a};
	ret += b;
	return ret;
}
template <typename T>
Matrix<T> operator-(const Matrix<T>& a, const Matrix<T>& b) {
	Matrix<T> ret {a};
	ret -= b;
	return ret;
}

template <typename T>
ostream& operator<<(ostream& os, const Matrix<T>& m) {
	for (size_t i = 0; i < m.rows; ++i) {
		for (size_t j = 0; j < m.cols; ++j) {
			os << setw(5);
			if (m.get(i,j) == POS_INF(T)) os << "inf";
			else os << m.get(i,j);
			os << ' ';
		}
		os << endl;
	}
	return os;
}


}