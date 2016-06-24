#include <algorithm>
#include <cmath>    // log2
#include <cstdlib>  // strtol
#include <fstream>
#include <iterator>  // istream_iterator
#include <iostream>
#include <iomanip>  // setw
#include <map>
#include <set>
#include <string>
#include <unordered_set>
#include <vector>
#include "../numeric.h"
#include "../perm.h"
#include "../prime.h"
#include "../search.h"
#include "../sort.h"
#include "../string.h"
#include "../utility.h"


extern std::ostringstream out;

namespace sal {

static int list_num{1};
static int list_size{1};
static int range{20000};
static std::string fname{"testdata.txt"};

enum Function_index {
    RANDGEN = 0,
    SHUFGEN,
    SELECT,
    BIN_SEARCH,
    SUB_MATCH,
    INTERSECTION,
    PERM,
    ALLPERMS,
    FACTORIZE,
    FACTORIZE_ROUGH,
    TOTIENT,
    GCD,
    LCM,
    FIBONACCI,
    MEAN,
    VARIANCE,
    INF_PRIME,
    NTH_PRIME,
    IS_PRIME,
    COUNT_PRIME,
    NEXT_PRIME,
    CLOSEST_PRIME,
    BUB_SORT,
    CNT_SORT,
    INS_SORT,
    HEAP_SORT,
    LIN_SORT,
    MER_SORT,
    PAT_SORT,
    QCK_SORT,
    RDX_SORT,
    TIM_SORT,
    SORT,
    LEVENSHTEIN,
    SA_LC_SUBSTRING,
    LC_SUBSTRING,
    LC_SUBSEQ,
    LC_SUBSEQ_LEN,
    CNT_INV,
    EXPERIMENT
};

const std::vector<pair<std::string, std::string>> names_in_order{
    {"0", "NUM SIZE RANGE generate random numbers into testdata.txt"},
    {"1", "NUM SIZE RANGE generate nearly sorted numbers (off by at most range) into testdata.txt"},
    {"select", "NUM ... INDEX  quickselect of ith smallest from unsorted sequence"},
    {"bin_search", "               binary search"},
    {"sub_match", "WORD WORD      KMP substd::string finding"},
    {"intersection", "WORD WORD..    intersection"},
    {"perm", "WORD INDEX     all permutations of a word"},
    {"allperms", "WORD           all permutations of a word"},
    {"factorize", "[NUM]          prime factorization"},
    {"factorize_rough", "[NUM]          rough prime factorization"},
    {"totient", "NUM            Euler's totient (# pos ints < n that is coprime with it)"},
    {"gcd", "NUM NUM        greatest common demonimator"},
    {"lcm", "NUM NUM..      lowest common multiple using prime factorization"},
    {"fibonacci", "NTH            nth-fibonacci term with matrices"},
    {"mean", "SEQ            mean across a sequence"},
    {"variance", "SEQ            variance across a sequence"},

    {"sieve.next_prime..", "NO ARG         infinite prime generation"},
    {"sieve.nth_prime", "NTH            nth prime"},
    {"sieve.is_prime", "NUM i          check if prime"},
    {"sieve.count", "NUM c          count primes below"},
    {"sieve.next_prime", "NUM n          next prime after NUM"},
    {"sieve.closest_prime", "NUM a          closest prime to NUM"},

    {"bub_sort", "               bubble sort"},
    {"cnt_sort", "               counting sort"},
    {"ins_sort", "               binary insertion sort"},
    {"heap_sort", "               heap sort"},
    {"lin_sort", "               linear insertion sort"},
    {"mer_sort", "               merge sort"},
    {"pat_sort", "               patience sort"},
    {"qck_sort", "               quick sort"},
    {"rdx_sort", "               radix sort"},
    {"tim_sort", "               timsort"},
    {"sort", "               standard C++ library sort"},

    {"levenshtein", "WORD WORD      Levenshtein distance with dynamic programming"},
    {"sa.lc_substr", "SEQ SUBSTR     LC_SUBSTRING using a suffix array"},
    {"lc_substr", "SEQ SUBSTR     LC_SUBSTRING using a suffix array (wrapper)"},
    {"lc_subseq", "SEQ SUBSEQ     lc_subsequence with dynamic programming"},
    {"lc_subseq_len", "SEQ SUBSEQ     lc_subsequence length"},

    {"count_inversions", "SEQ            count number of inversions in sequence in O(nlgn)"},
    {"experiment", "               experimental algorithm"}};

const std::map<std::string, Function_index> algonames{{"0", RANDGEN},
                                            {"1", SHUFGEN},
                                            {"select", SELECT},
                                            {"bin_search", BIN_SEARCH},
                                            {"sub_match", SUB_MATCH},
                                            {"intersection", INTERSECTION},
                                            {"perm", PERM},
                                            {"allperms", ALLPERMS},
                                            {"factorize", FACTORIZE},
                                            {"factorize_rough", FACTORIZE_ROUGH},
                                            {"totient", TOTIENT},
                                            {"gcd", GCD},
                                            {"lcm", LCM},
                                            {"fibonacci", FIBONACCI},
                                            {"mean", MEAN},
                                            {"variance", VARIANCE},

                                            {"sieve.next_prime..", INF_PRIME},
                                            {"sieve.nth_prime", NTH_PRIME},
                                            {"sieve.is_prime", IS_PRIME},
                                            {"sieve.count", COUNT_PRIME},
                                            {"sieve.next_prime", NEXT_PRIME},
                                            {"sieve.closest_prime", CLOSEST_PRIME},

                                            {"bub_sort", BUB_SORT},
                                            {"cnt_sort", CNT_SORT},
                                            {"ins_sort", INS_SORT},
                                            {"heap_sort", HEAP_SORT},
                                            {"lin_sort", LIN_SORT},
                                            {"mer_sort", MER_SORT},
                                            {"pat_sort", PAT_SORT},
                                            {"qck_sort", QCK_SORT},
                                            {"rdx_sort", RDX_SORT},
                                            {"tim_sort", TIM_SORT},
                                            {"sort", SORT},

                                            {"levenshtein", LEVENSHTEIN},
                                            {"sa.lc_substr", SA_LC_SUBSTRING},
                                            {"lc_substr", LC_SUBSTRING},
                                            {"lc_subseq", LC_SUBSEQ},
                                            {"lc_subseq_len", LC_SUBSEQ_LEN},

                                            {"count_inversions", CNT_INV},

                                            {"experiment", EXPERIMENT}};

void listalgos() {
    out << "Add -p as last argument to print out sample result\n";
    for (auto& algo : names_in_order)
        out << setfill('-') << setw(20) << algo.first << ": " << algo.second << std::endl;
}

template <typename T, typename Op>
std::vector<std::vector<T>> data_store(const std::string& fname, Op vlist_gen, int l_num = list_num,
                             int l_size = list_size, int r = range) {
    ofstream f{fname};
    f << l_num << ' ' << l_size << ' ' << r << std::endl;
    std::vector<std::vector<T>> vlist{vlist_gen(l_num, l_size, r)};
    for (auto v : vlist) print(v, f);
    return vlist;
}

void load_data(std::vector<std::vector<int>>& vlist) {
    vlist.reserve(list_num);
    try {  // try ints
        vlist = ftovec<int>(fname, list_num, list_size, range);
    } catch (...) {
        vlist = data_store<int>(fname, rand_vlist<int>);
    }
}

int algotest(std::vector<std::string> args) {
    if (args.size() < 1 || args[0].empty() || args[0][0] == '\n' || args[0][0] == 'h' ||
        args[0] == "help") {
        out << "Proper use expects at least a std::string argument specifying which algorithm to test\n";
        listalgos();
        return 1;
    }

    // out << "args(" << args.size() << "):";
    // print(args,out);

    std::string name(args[0]);

    bool to_print{args.back().size() > 1 && args.back()[0] == '-' && args.back()[1] == 'p'};
    // not a legitimate argument
    if (to_print) args.pop_back();

    auto algopair = algonames.find(name);
    // incorrect entry, suggest the closest entry (by Levenshtein distance)
    if (algopair == algonames.end()) {
        std::string suggested_name;
        size_t lowest_distance{POS_INF(size_t)};
        for (auto algo = algonames.begin(); algo != algonames.end(); ++algo) {
            size_t cur_distance{levenshtein(name, algo->first)};
            // update algopair as well since its name exiting loop will be the suggested name
            if (cur_distance < lowest_distance) {
                lowest_distance = cur_distance;
                algopair = algo;
            }
        }
        suggested_name = algopair->first;
        out << "did you mean: " << suggested_name << std::endl;
        return 2;
    }

    std::vector<std::vector<int>> vlist;
    Function_index algo_index{algopair->second};

    Timer time;
    switch (algo_index) {
        // data generation algorithms
        case RANDGEN:
        case SHUFGEN:
            if (args.size() < 4)
                out << "Random generation needs l_num l_size range as argument\n";
            else {
                list_num = tol(args[1]);
                list_size = tol(args[2]);
                range = tol(args[3]);
                if (name[0] == '0')
                    data_store<int>(fname, rand_vlist<int>, list_num, list_size, range);
                if (name[0] == '1')
                    data_store<int>(fname, shuffled_vlist<int>, list_num, list_size, range);
            }
            break;
        case SELECT: {
            if (args.size() < 4)
                out << "Number sequence needs to be longer and followed by an index (1 = "
                       "smallest)\n";
            else {
                std::vector<long> nums;
                long i{tol(args.back())};
                args.pop_back();
                for (size_t n = 1; n < args.size(); ++n) nums.push_back(tol(args[n]));
                print(nums, out);
                auto ith_elem = sal::select(nums, i);
                out << i << "th element: " << *ith_elem << '\n';
                // print partial sorting of sequence afterwards
                if (to_print) print(nums, out);
            }
        }
        case BIN_SEARCH: {
            if (args.size() < 4) {
                load_data(vlist);
                time.restart();
                for (auto& v : vlist) bin_search(v.begin(), v.end(), randint(v.size()));
            } else
                bin_search(args.begin() + 2, args.end(), args[1], Strcmp{});
            break;  // random element as key
        }
        case SUB_MATCH:
            if (args.size() < 3)
                out << "Need a SENTENCE and a WORD to match against\n";
            else {
                auto i = sub_match(args[1], args[2]);
                if (i != args[1].end())
                    out << "matched at position " << i - args[1].begin() << std::endl;
                else
                    out << args[2] << " not found in " << args[1] << std::endl;
            }
            break;
        // intersection of 1 or more containers (std::strings in test)
        case INTERSECTION: {
            set<std::string> s;
            for (size_t i = 1; i < args.size(); ++i) s.insert(std::string(args[i]));
            print(intersection(s), out);
            break;
        }
        // generate all permutations of std::string
        case PERM:
            if (args.size() < 3)
                out << "Need to specify word and permutation index\n";
            else {
                perm(args[1], tol(args[2]));
                out << args[1] << std::endl;
            }
            break;
        case ALLPERMS:
            if (args.size() < 2)
                out << "Need to specify word\n";
            else
                print(allperms(args[1]), out);
            break;
        // get all prime factors of number
        case FACTORIZE:
            if (args.size() < 2) {
                load_data(vlist);
                time.restart();
                for (auto& v : vlist)
                    for (auto i : v) factorize(i);
            } else
                print(factorize(tol(args[1])), out);
            break;
        // factor semi primes
        case FACTORIZE_ROUGH:
            if (args.size() < 2) {
                load_data(vlist);
                time.restart();
                for (auto& v : vlist)
                    for (auto i : v) factorize_rough(i);
            } else
                print(factorize_rough(tol(args[2])), out);
            break;
        // substd::string finding
        case TOTIENT:
            if (args.size() < 2)
                out << "Totient takes 1 number and returns # of coprime ints under it\n";
            else
                out << totient(tol(args[1])) << std::endl;
            break;
        case GCD:
            if (args.size() < 3)
                out << "GCD takes 2 numbers\n";
            else
                out << gcd(tol(args[1]), tol(args[2])) << std::endl;
            break;
        case LCM:
            if (args.size() < 3)
                out << "LCM takes at least 2 numbers\n";
            else {
                std::vector<long> nums;
                for (auto s = args.begin() + 1; s != args.end(); ++s) nums.push_back(tol(*s));
                out << lcm(nums.begin(), nums.end()) << std::endl;
            }
            break;
        case FIBONACCI:  // nth fibonacci term
            if (args.size() < 2)
                out << "Need to specify n\n";
            else
                out << fibonacci<unsigned long long>(tol(args[1])) << std::endl;
            break;
        case MEAN:
        case VARIANCE: {
            std::vector<double> elems;
            for (size_t i = 1; i < args.size(); ++i) {
                auto num = std::stod(args[i]);
                elems.push_back(num);
            }
            if (algo_index == MEAN) {
                out << "mean " << mean(elems.begin(), elems.end()) << std::endl;
            }
            else if (algo_index == VARIANCE) {
                out << "variance " << variance(elems.begin(), elems.end()) << std::endl;
            }    

            break;
        }

        // by prime number theorem (PNT) count is ~ n/ln(n) and nth prime is ~ nln(n) -> gap is
        // limited by n < p < 2n or n < p < 1.2n for n > 100
        case INF_PRIME:
        case NTH_PRIME:
        case IS_PRIME:
        case COUNT_PRIME:
        case NEXT_PRIME:
        case CLOSEST_PRIME: {  // prime algorithms
            Sieve<> sieve{10000};
            switch (args.size()) {
                // infinite generation
                case 1:
                    if (algo_index == INF_PRIME)
                        while (true) out << sieve.next_prime() << " \n";
                // nth prime where n is args[1] (keep matching if argument length mismatches)
                case 2: {
                    long num{tol(args[1])};
                    if (algo_index == NTH_PRIME)
                        out << sieve.nth_prime(num) << '\n';
                    else if (algo_index == IS_PRIME)
                        if (sieve.is_prime(num))
                            out << num << " is prime\n";
                        else
                            out << num << " is not prime\n";
                    else if (algo_index == COUNT_PRIME)
                        out << sieve.count(num) << '\n';
                    else if (algo_index == NEXT_PRIME)
                        out << sieve.next_prime(num) << '\n';
                    else if (algo_index == CLOSEST_PRIME)
                        out << sieve.closest_prime(num) << '\n';
                    break;
                }
                // check all input numbers for prime
                default:
                    for (size_t i = 1; i < args.size(); ++i) {
                        long int num{tol(args[i])};
                        if (sieve.is_prime(num))
                            out << num << " is prime\n";
                        else
                            out << num << " is not prime\n";
                    }
            }
            break;
        }
        case BUB_SORT:
        case CNT_SORT:
        case INS_SORT:
        case HEAP_SORT:
        case LIN_SORT:
        case MER_SORT:
        case PAT_SORT:
        case QCK_SORT:
        case RDX_SORT:
        case TIM_SORT:
        case SORT: {  // sorts
            load_data(vlist);
            int bit_num{static_cast<int>(ceil(log2(range)))};
            time.restart();
            switch (algo_index) {
                case BUB_SORT:
                    for (auto& v : vlist) bub_sort(v.begin(), v.end());
                    break;
                case CNT_SORT:
                    for (auto& v : vlist) cnt_sort(v.begin(), v.end(), range + 1);
                    break;
                case INS_SORT:
                    for (auto& v : vlist) ins_sort(v.begin(), v.end());
                    break;
                case HEAP_SORT:
                    for (auto& v : vlist) heap_sort(v.begin(), v.end());
                    break;
                case LIN_SORT:
                    for (auto& v : vlist) lin_sort(v.begin(), v.end());
                    break;
                case MER_SORT:
                    for (auto& v : vlist) mer_sort(v.begin(), v.end());
                    break;
                case PAT_SORT:
                    for (auto& v : vlist) pat_sort(v.begin(), v.end());
                    break;
                case QCK_SORT:
                    for (auto& v : vlist) qck_sort(v.begin(), v.end());
                    break;
                case RDX_SORT:
                    for (auto& v : vlist) rdx_sort(v.begin(), v.end(), bit_num);
                    break;  // # bits is log2 of maximum value
                case TIM_SORT:
                    for (auto& v : vlist) tim_sort(v.begin(), v.end());
                    break;
                case SORT:
                    for (auto& v : vlist) sort(v.begin(), v.end());
                    break;
                default:
                    break;
            }
            if (to_print) print(vlist.back(), out);
            break;
        }
        // longest common algorithms
        case LEVENSHTEIN:
        case SA_LC_SUBSTRING:
        case LC_SUBSTRING:
        case LC_SUBSEQ:
        case LC_SUBSEQ_LEN: {
            if (args.size() < 3)
                out << "Need 2 words to compare\n";
            else if (algo_index == LEVENSHTEIN)
                out << "levenshtein dist: " << levenshtein(args[1], args[2]) << std::endl;
            else if (algo_index == SA_LC_SUBSTRING) {
                args[1] += '$';
                append(args[1], args[2]);
                Suffix_array<> sa{args[1]};
                out << "longest common substd::string: " << lc_substr(args[1], args[2]) << std::endl;
                if (to_print) sa.print();
            } else if (algo_index == LC_SUBSTRING)
                out << "longest common substd::string: " << lc_substr(args[1], args[2]) << std::endl;
            else if (algo_index == LC_SUBSEQ)
                out << "longest common subsequence: " << lc_subseq(args[1], args[2]) << std::endl;
            else if (algo_index == LC_SUBSEQ_LEN)
                out << "longest common subsequence length: " << lc_subseq_len(args[1], args[2])
                    << std::endl;
        }
        case CNT_INV: {
            std::vector<int> elems;
            for (size_t i = 1; i < args.size(); ++i) {
                int num = tol(args[i]);
                elems.push_back(num);
            }
            out << "number of inversions: " << count_inversions(elems.begin(), elems.end()) << std::endl;
        }
        case EXPERIMENT: {  // testing for new algorithms
            std::vector<std::string> test{ftostr(fname)};
            sort(test.begin(), test.end());
            if (to_print) print(test, out);
            break;
        }
        default:
            listalgos();
    }

    auto done_time = time.tonow();

    out << "after: " << list_num << " lists with " << list_size << " elements: " << setw(10)
        << done_time << " us " << setw(10) << done_time / 1000 << " ms  ("
        << squish_whitespace(ltrim(names_in_order[algo_index].second)) << ")\n";

    return 0;
}

}