#include <ios>
#include <string>
#include <vector>
#include <sstream>
#include "algotest.h"

using namespace std;

ostringstream out;

int main(int argc, char** argv) {
	ios_base::sync_with_stdio(false);
    vector<string> args;
    for (int i = 1; i < argc; ++i) {
        args.emplace_back(argv[i]);
    }
    algotest(args);
    cout << out.str();
}