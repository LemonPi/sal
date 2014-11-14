#pragma once
#include <iostream>
#include <iomanip>
#include <vector>
#include <set>
#include <stdexcept>
#include <initializer_list>

using namespace std;


template <typename T>
class Matrix {
	size_t rows, cols;
	vector<T> elems;
public:
	Matrix(size_t r, size_t c, vector<T>&& e) : rows{r}, cols{c}, elems{move(e)} {}
	Matrix(initializer_list<initializer_list<T>> a);
	Matrix(const Matrix& a) : rows{a.row()}, cols{a.col()}, elems{a.elems} {}
	Matrix(Matrix&& a) : rows{a.row()}, cols{a.col()}, elems{move(a.elems)} {}
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

	T& get(size_t row, size_t col) { return elems[row*cols + col]; }
	const T& get (size_t row, size_t col) const {return elems[row*cols + col]; }

	void rotate(); // clockwise

	void clear_zero();

	void print();

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
Matrix<T>::Matrix(initializer_list<initializer_list<T>> a) : rows{a.size()}, cols{a.size() ? a.begin()->size() : 0} {
	elems.reserve(rows * cols);
	for (const auto& r : a) 
		for (const auto& c : r) 
			elems.push_back(c);
}

template <typename T>
void Matrix<T>::rotate() {	// clockwise
	if (rows != cols) return;	// no arbitrary rotation yet
	int n = rows;	// assume rows == cols
	for (int layer = 0; layer < n / 2; ++layer) {
		int first = layer;
		int last = n - 1 - layer;
		for (int i = first; i < last; ++i) {
			int offset = i - first;

			// save top
			int top = elems[first*cols + i];
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
			T elem {};
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
	for (int i = 0; i < elems.size(); ++i)
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
	for (int i = 0; i < m.rows; ++i) {
		for (int j = 0; j < m.cols; ++j)
			os << setw(5) << m.get(i,j) << ' ';
		os << endl;
	}
	return os;
}

template <typename T>
void Matrix<T>::print() {
	cout << *this;
}

