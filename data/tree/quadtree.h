#pragma once
#include "tree.h"
// each node is a bounding box covering some space, with root node covering entire area
// leaf nodes contain points while internal nodes contains 4 children (1 per quadrant) 
// this particular implementation requires a grid unit size and only stores the PRESENCE of a point in that unit
// multiple points within the same grid will be treated as 1 point, so only use this tree in emptiness queries