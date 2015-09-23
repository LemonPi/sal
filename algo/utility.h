/* Written by Johnson Zhong unless otherwise noted
contact: johnsonzhong@hotmail.ca | johnsonzhong.me
------------------------
testing utilty functions

print(sequence, os)  -> prints space-separated elements of sequence to outstream os

append(sequence, sequence)   -> appends the second sequence to the first 

tol(string)          -> turns string to long

randgen(min, max, num)          -> set holding num random integers from min to max
randgen(min, max, num, granularity, seed) -> set holding num random numbers from min to max (granularity corresponds to how many numbers exist between integers (granularity = 2 produces 0.5, 1.5, etc))
randint(min, max)               -> random integer from min to max
randint(max)                    -> random integer from 0 to max
natural()                       -> quick random natural number

perturb(begin, end, range)   -> random shuffle of sequence probabilistically limited by a range

ftovec(name, lines, elements, range) -> vector lines number of vectors each holding
                                        elements from 0 to range
ftostr(name)         -> vector holding each line of the file as a string

class: Timer        benchmarking elapsed time with chrono
restart()     automatically restarts on creation, restart again to reset
tonow() ->  elapsed time from last start in microseconds

*/

// Testing utilty functions
#pragma once
#include <algorithm>    // shuffle
#include <random>
#include <vector>
#include <iostream>
#include <fstream>
#include <iterator> // iterator_trait
#include <chrono>
#include <cstdlib>  // strtol, rand
#include <typeinfo>
#include <string>
#include <sstream>  // istringstream
#include "macros.h" // Iter_value

namespace sal {

using namespace std::chrono;
template <typename T>
using vector_2d = std::vector<std::vector<T>>;

template <typename Iter>
void print(Iter begin, Iter end, std::ostream& os = std::cout) {
    for (; begin != end; ++begin)
        os << *begin << ' ';
    os << '\n';
}
template <typename Sequence>
void print(const Sequence& v, std::ostream& os = std::cout) {
    for (const auto& x : v)
        os << x << ' ';
    os << '\n';
}

// appending two sequences (assumes insert exists and iterators are defined)
template <typename Sequence>
void append(Sequence& a, const Sequence& b) {
    a.insert(a.end(), b.begin(), b.end());
}

long tol(const char* s) {
    return std::strtol(s, nullptr, 10);
}
long tol(const std::string& s) {
    return std::strtol(s.c_str(), nullptr, 10);
}

// random number generation
class Rand_int {
public:
    Rand_int(int low, int high) : distribution{low, high} {}
    Rand_int(int low, int high, long long seed) : engine(seed), distribution{low, high} {}
    int operator()() {return distribution(engine);}

private:
    std::default_random_engine engine;
    std::uniform_int_distribution<> distribution;
};

std::vector<int> randgen(int min, int max, size_t num) {
    static Rand_int die{min, max, system_clock::now().time_since_epoch().count()};
    std::vector<int> res;
    res.reserve(num);
    for (int i = 0; i < (int)num; ++i)
        res.push_back(die());
    return res;
}

// types other than int needs additional granularity (divisor) to create floats
template <typename T>
std::vector<T> randgen(T min, T max, size_t num, int granularity = 1, long long seed = system_clock::now().time_since_epoch().count()) {
    max *= granularity;
    min *= granularity;
    static Rand_int die{(int)min, (int)max, seed};
    std::vector<T> res;
    res.reserve(num);
    for (int i = 0; i < (int)num; ++i)
        res.push_back((T)die()/granularity);
    return res;   
}

int randint(int min, int max) {
    static Rand_int die{min, max, system_clock::now().time_since_epoch().count()};
    return die();
}
int randint(int max) {
    static Rand_int die{0, max, system_clock::now().time_since_epoch().count()};
    return die();
}
// return natural # (0 to int max)
int natrual() {
    return rand();
}

template <typename Iter>
void perturb(const Iter begin, const Iter end, size_t range) {  // randomly swap elements with another at most range distances away 
    for (auto cur = begin; cur != end; ++cur) {
        int swap_by {randint(-range, range)};
        if (cur + swap_by >= end || cur + swap_by < begin) continue;
        std::swap(*cur, *(cur + swap_by));
    }
}

// benchmarking
#ifdef GOMP
class Timer {
    double init, end;
public:
    Timer() : init{omp_get_wtime()} {}
    void restart() { init = omp_get_wtime(); }
    double tonow() { 
        end = omp_get_wtime();
        return end - init;
    }
};
#else
class Timer {
    time_point<system_clock> init, end;
    microseconds elapsed = duration_cast<microseconds>(end - init);
public:
    Timer() : init{system_clock::now()} {}
    void restart() { init = system_clock::now(); }
    double tonow() { 
        end = system_clock::now();
        elapsed = duration_cast<microseconds>(end - init);
        return elapsed.count();
    }
};
#endif
 // reads from file and load into vec of vecs
template <typename T>
vector_2d<T> ftovec(const std::string& name, int& n_line, int& w_line, int& range) {   
    std::ifstream f {name};
    f >> n_line >> w_line >> range; // meta information
    std::string line;
    std::getline(f, line);  // get end of line to finish header
    vector_2d<T> res;
    while (getline(f, line)) {
        std::istringstream is {line};
        res.push_back(std::vector<T>(std::istream_iterator<T>(is), std::istream_iterator<T>()));
    }
    return res;
}

template <typename T>
vector_2d<T> rand_vlist(int l_num, int l_size, int r) {
    vector_2d<T> v_list;
    v_list.reserve(l_num);
    for (int i = 0; i < l_num; ++i) v_list.push_back(randgen(-r/2, r/2, l_size));
    return v_list;
}

template <typename T>
vector_2d<T> shuffled_vlist(int l_num, int l_size, int r) {
    vector_2d<T> v_list;
    v_list.reserve(l_num);
    for (int i = 0; i < l_num; ++i) {
        std::vector<T> v;
        v.reserve(l_size);
        for (int i = 0; i < l_size; ++i) v.push_back(i);
        perturb(v.begin(), v.end(), r);
        v_list.push_back(v);
    }
    return v_list;
}

std::vector<std::string> ftostr(const std::string& name) {
    std::vector<std::string> res;
    std::ifstream f {name};
    std::string line;
    while (std::getline(f, line)) {
        if (line.back() == '\r') line.pop_back();
        res.push_back(line);
    }
    return res;
}

// fast pointer hash from egur on Stackoverflow
template<typename T>
struct Pointer_hash {
    size_t operator()(const T* val) const {
        static const size_t shift = (size_t)log2(1 + sizeof(T));
        return (size_t)(val) >> shift;
    }
};


// compile time string hashing for switch statements, from http://stackoverflow.com/a/16388610/3368722
constexpr size_t str2int(const char* str, int h = 0) {
    return !str[h] ? 5381 : (str2int(str, h+1)*33) ^ str[h];
}

#ifdef RELAXED_CONSTEXPR
// compile time testing
template <typename T, T x>
constexpr bool test_constexpr(bool print) {
    if (print) std::cout << typeid(x).name() << " is constexpr\n";
    return true;
}
#endif

} // end namespace sal
