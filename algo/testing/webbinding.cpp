#include <string>
#include <vector>
#include "../string.h"
#include "algotest.h"
#include "emscripten/bind.h"
using namespace emscripten;

std::ostringstream out;

std::string algo_str(std::string input) {
	std::vector<std::string> args {ssplit(input, ' ')};
	// reset output
	out = std::ostringstream();
	algotest(args);
	return out.str();
}

EMSCRIPTEN_BINDINGS(my_module) {
	emscripten::function("algo_str", &algo_str);
}