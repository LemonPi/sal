// pretty printing of trees, taken from http://articles.leetcode.com/2010/09/how-to-pretty-print-binary-tree.html
#pragma once
#include <iostream>
#include <deque>
#include <iomanip>
#include <sstream>
#include <cmath>
#include <string>
#include "tree.h"


namespace sal {

using std::setw;
using std::setfill;


// Convert an integer value to string
std::string itos(int val) {
    std::ostringstream ss;
    ss << val;
    return ss.str();
}

// Print the arm branches (eg, /    \ ) on a line
template <typename Node>
void print_branches(int branchLen, int node_space_len, int startLen, int nodes_in_level, const std::deque<Node*>& nodes_queue, std::ostream& out) {
    auto iter = nodes_queue.begin();
    for (int i = 0; i < nodes_in_level / 2; i++) {  
        out << ((i == 0) ? setw(startLen-1) : setw(node_space_len-2)) << "" << ((*iter++) ? "/" : " ");
        out << setw(2*branchLen+2) << "" << ((*iter++) ? "\\" : " ");
    }
    out << endl;
}

// Print the branches and node (eg, ___10___ )
template <typename Node>
void print_nodes(int branchLen, int node_space_len, int startLen, int nodes_in_level, const std::deque<Node*>& nodes_queue, std::ostream& out) {
    auto iter = nodes_queue.begin();
    for (int i = 0; i < nodes_in_level; i++, iter++) {
        out << ((i == 0) ? setw(startLen) : setw(node_space_len)) << "" << ((*iter && (*iter)->left) ? setfill('_') : setfill(' '));
        out << setw(branchLen+2) << ((*iter) ? (*iter)->to_string() : "");
        out << ((*iter && (*iter)->right) ? setfill('_') : setfill(' ')) << setw(branchLen) << "" << setfill(' ');
    }
    out << endl;
}

// Print the leaves only (just for the bottom row)
template <typename Node>
void print_leaves(int left_margin, int level, int nodes_in_level, const std::deque<Node*>& nodes_queue, std::ostream& out) {
    auto iter = nodes_queue.begin();
    for (int i = 0; i < nodes_in_level; i++, iter++) {
        out << ((i == 0) ? setw(left_margin+2) : setw(2*level+2)) << ((*iter) ? (*iter)->to_string() : "");
    }
    out << endl;
}

// Pretty formatting of a binary tree to the output stream
// @ param
// level  Control how wide you want the tree to sparse (eg, level 1 has the minimum space between nodes, while level 2 has a larger space between nodes)
// left_margin  Change this to add some indent space to the left (eg, left_margin of 0 means the lowest level of the left node will stick to the left margin)
template <typename Node>
void pretty_print(Node* root, int level = 1, int left_margin = 0, std::ostream& out = std::cout) {
    int h = max_height(root);
    int nodes_in_level = 1;

    int branchLen = 2*((int)std::pow(2.0,h)-1) - (3-level)*(int)std::pow(2.0,h-1);  // eq of the length of branch for each node of each level
    int node_space_len = 2 + (level+1)*(int)std::pow(2.0,h);  // distance between left neighbor node's right arm and right neighbor node's left arm
    int startLen = branchLen + (3-level) + left_margin;  // starting space to the first node to print of each level (for the left most node of each level only)

    std::deque<Node*> nodes_queue;
    nodes_queue.push_back(root);
    for (int r = 1; r < h; r++) {
        print_branches(branchLen, node_space_len, startLen, nodes_in_level, nodes_queue, out);
        branchLen = branchLen/2 - 1;
        node_space_len = node_space_len/2 + 1;
        startLen = branchLen + (3-level) + left_margin;
        print_nodes(branchLen, node_space_len, startLen, nodes_in_level, nodes_queue, out);

        for (int i = 0; i < nodes_in_level; i++) {
            Node* current = nodes_queue.front();
            nodes_queue.pop_front();
            if (current) {
                nodes_queue.push_back(current->left);
                nodes_queue.push_back(current->right);
            } else {
                nodes_queue.push_back(NULL);
                nodes_queue.push_back(NULL);
            }
        }
        nodes_in_level *= 2;
    }
    print_branches(branchLen, node_space_len, startLen, nodes_in_level, nodes_queue, out);
    print_leaves(left_margin, level, nodes_in_level, nodes_queue, out);
}

}   // end namespace sal