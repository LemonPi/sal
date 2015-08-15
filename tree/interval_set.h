#pragma once
#include <limits>	// lowest()
#include <vector>
#include "tree.h"
#include "treap.h"

namespace sal {

template <typename T>
struct Interval {
	T low, high;
	friend bool operator==(const Interval<T> a, const Interval<T> b) {
		return a.low == b.low && a.high == b.high;
	}
	friend bool operator!=(const Interval<T> a, const Interval<T> b) {
		return !(a == b);
	}
};

// interval tree operation
template <typename Node>
no_overlap(const Node* interval, typename Node::key_type low, typename Node::key_type high) {
	return (low > interval->high || interval->key > high);
}
template <typename Node>
bool overlap(const Node* interval, typename Node::key_type low, typename Node::key_type high) {
	return !no_overlap(interval, low, high);
}
template <typename Node>
void update_max(Node* interval) {
	interval->max = std::max(interval->high, std::max(interval->left->max, interval->right->max));
}
// either finds an overlapping interval or nil - O(lgn) but usually orders of magnitude faster than minimum or exact interval search
template <typename Node>
Node* interval_search(Node* interval, typename Node::key_type low, typename Node::key_type high) {
	while (interval != Node::nil && no_overlap(interval, low, high)) {
		// left side's entire subtree overlaps with interval to find
		if (interval->left != Node::nil && interval->left->max >= low)
			interval = interval->left;
		// can't possibly be found in the left subtree, by elimination look at right
		// [left->min...  left->high...     left->max]      [low     high]
		else interval = interval->right; 
	}
	return interval;
}

// first (smallest key/low) interval that matches O(lgn)
template <typename Node>
Node* interval_min_search(Node* interval, typename Node::key_type low, typename Node::key_type high) {
	// check if left subtree overlaps
	if (interval->left != Node::nil && interval->left->max >= low) {
		auto min = interval_min_search(interval->left, low, high);
		// there exists an overlapping interval with lower low
		if (min != Node::nil) return min;
		else if (overlap(interval, low, high)) return interval;
		else return Node::nil;
	}
	// if left subtree doesn't overlap, then check if current one does
	else if (overlap(interval, low, high)) return interval;
	else return interval_min_search(interval->right, low, high);
}

// all intervals that match, much slower
template <typename Node>
void interval_all_search(Node* interval, typename Node::key_type low, typename Node::key_type high, std::vector<Node*>& matched_intervals) {
	if (overlap(interval, low, high)) matched_intervals.push_back(interval);
	if (interval->left != Node::nil && interval->left->max >= low) {
		// might have intersecting interval on left
		interval_all_search(interval->left, low, high, matched_intervals);
	} 
	if (interval->right != Node::nil && interval->right->key <= low) {
		// might have intersecting interval on right
		interval_all_search(interval->right, low, high, matched_intervals);
	}
}
template <typename Node>
std::vector<Node*> interval_all_search(Node* interval, typename Node::key_type low, typename Node::key_type high) {
	std::vector<Node*> res;
	interval_all_search(interval, low, high, res);
	return res;
}

template <typename Node>
Node* interval_exact_search(Node* start, typename Node::key_type low, typename Node::key_type high) {
	// low and high must exactly match the interval
	Node* interval = tree_find(start, low);
	// if found interval shares same low but different high, can continually search on same subtree
	while (interval != Node::nil && high != interval->high) interval = tree_find(interval, low);
	return interval;
}
// fixups to maintain balance
template <typename Node>
void propagate_max_down(Node* start, const Node* node) {
	start->max = std::max(start->max, node->max);
}

template <typename Node>
void restore_ancestor_max(Node* moved) {
	while (moved != Node::nil) {
		update_max(moved);
		moved = moved->parent;
	}
}

// interval trees augmented from base RB-tree
template <typename Node>
class Interval_augment : public Tree<Node> {

	using NP = Node*;
	using T = typename Node::key_type;
	using Tree<Node>::root;
	using Tree<Node>::rb_insert;
	using Tree<Node>::rb_delete;
	using Tree<Node>::transplant;
	using Tree<Node>::rb_insert_fixup;

	// rotations, augmented by changing child and node's sizes
	virtual void rotate_left(NP node) override {
		// child always becomes the parent of node
		NP child {node->right};
		Tree<Node>::rotate_left(node);
		// additional fixup for interval sets
		update_max(node);
		update_max(child);
	}
	virtual void rotate_right(NP node) override {
		NP child {node->left};
		Tree<Node>::rotate_right(node);
		update_max(node);
		update_max(child);
	}

public:
	using pointer = Node*;
	using const_pointer = const Node*;
	using iterator  = typename Tree<Node>::iterator;
	using const_iterator = typename Tree<Node>::const_iterator;

	using Tree<Node>::size;
	using Tree<Node>::empty;

	Interval_augment() = default;
	Interval_augment(std::initializer_list<Interval<T>> l) {
		for (const auto& v : l) insert(v);
	}

	void insert(T low, T high) {
		NP node {new Node(low, high)};
		rb_insert(node, propagate_max_down<Node>);
	};
	void insert(Interval<T> interval) {
		NP node {new Node(interval.low, interval.high)};
		rb_insert(node, propagate_max_down<Node>);
	}

	void erase(T low, T high) {
		NP interval {interval_exact_search(root, low, high)};
		if (interval != Node::nil) rb_delete(interval, restore_ancestor_max<Node>);
	}
	void erase(Interval<T> interval) {
		NP candidate {interval_exact_search(root, interval.low, interval.high)};
		if (candidate != Node::nil) rb_delete(candidate, restore_ancestor_max<Node>);
	}

	// interval tree operation - find overlapping interval
	iterator find(T low, T high) 					{return iterator{interval_search(root, low, high)};}
	iterator find(Interval<T> interval) 			{return iterator{interval_search(root, interval.low, interval.high)};}
	iterator find_first(T low, T high) 				{return iterator{interval_min_search(root, low, high)};}
	iterator find_first(Interval<T> interval) 		{return iterator{interval_min_search(root, interval.low, interval.high)};}
	iterator find_exact(T low, T high) 				{return iterator{interval_exact_search(root, low, high)};}
	iterator find_exact(Interval<T> interval) 		{return iterator{interval_exact_search(root, interval.low, interval.high)};}
	// const versions
	const_iterator find(T low, T high) const				{return const_iterator{interval_search(root, low, high)};}
	const_iterator find(Interval<T> interval) const			{return const_iterator{interval_search(root, interval.low, interval.high)};}
	const_iterator find_first(T low, T high) const			{return const_iterator{interval_min_search(root, low, high)};}
	const_iterator find_first(Interval<T> interval) const	{return const_iterator{interval_min_search(root, interval.low, interval.high)};}
	const_iterator find_exact(T low, T high) const			{return const_iterator{interval_exact_search(root, low, high)};}
	const_iterator find_exact(Interval<T> interval) const	{return const_iterator{interval_exact_search(root, interval.low, interval.high)};}
	std::vector<NP> find_all(T low, T high) const 			{return interval_all_search(root, low, high);}
	std::vector<NP> find_all(Interval<T> interval) const 	{return interval_all_search(root, interval.low, interval.high);}

	void print() const {
		walk_and_print_indented(root, [](const Node* node){
			std::cout << '[' << std::setw(6) << node->key << ',' << std::setw(6) << node->high << ']' << '(' << std::setw(6) << node->max << ')' << " -> ";},
			27);
	}
};


// interval trees augmented from base Treap (random)
template <typename Node>
class Interval_treap : public Treap<Node> {

	using NP = Node*;
	using T = typename Node::key_type;
	using Treap<Node>::root;
	using Treap<Node>::treap_insert;
	using Treap<Node>::treap_delete;
	using Treap<Node>::transplant;


	// rotations, augmented by changing child and node's sizes
	virtual void rotate_left(NP node) override {
		// child always becomes the parent of node
		NP child {node->right};
		Treap<Node>::rotate_left(node);
		// additional fixup for interval sets
		update_max(node);
		update_max(child);
	}
	virtual void rotate_right(NP node) override {
		NP child {node->left};
		Treap<Node>::rotate_right(node);
		update_max(node);
		update_max(child);
	}

public:
	using pointer = Node*;
	using const_pointer = const Node*;
	using iterator  = typename Tree<Node>::iterator;
	using const_iterator = typename Tree<Node>::const_iterator;

	using Treap<Node>::size;
	using Treap<Node>::empty;

	Interval_treap() = default;
	Interval_treap(std::initializer_list<Interval<T>> l) {
		for (const auto& v : l) insert(v);
	}

	void insert(T low, T high) {
		NP node {new Node(low, high)};
		treap_insert(node, propagate_max_down<Node>);
	};
	void insert(Interval<T> interval) {
		NP node {new Node(interval.low, interval.high)};
		treap_insert(node, propagate_max_down<Node>);
	}

	void erase(T low, T high) {
		NP interval {interval_exact_search(root, low, high)};
		if (interval != Node::nil) treap_delete(interval, restore_ancestor_max<Node>);
	}
	void erase(Interval<T> interval) {
		NP candidate {interval_exact_search(root, interval.low, interval.high)};
		if (candidate != Node::nil) treap_delete(candidate, restore_ancestor_max<Node>);
	}

	// interval tree operation - find overlapping interval
	iterator find(T low, T high) 					{return iterator{interval_search(root, low, high)};}
	iterator find(Interval<T> interval) 			{return iterator{interval_search(root, interval.low, interval.high)};}
	iterator find_first(T low, T high) 				{return iterator{interval_min_search(root, low, high)};}
	iterator find_first(Interval<T> interval) 		{return iterator{interval_min_search(root, interval.low, interval.high)};}
	iterator find_exact(T low, T high) 				{return iterator{interval_exact_search(root, low, high)};}
	iterator find_exact(Interval<T> interval) 		{return iterator{interval_exact_search(root, interval.low, interval.high)};}
	// const versions
	const_iterator find(T low, T high) const				{return const_iterator{interval_search(root, low, high)};}
	const_iterator find(Interval<T> interval) const			{return const_iterator{interval_search(root, interval.low, interval.high)};}
	const_iterator find_first(T low, T high) const			{return const_iterator{interval_min_search(root, low, high)};}
	const_iterator find_first(Interval<T> interval) const	{return const_iterator{interval_min_search(root, interval.low, interval.high)};}
	const_iterator find_exact(T low, T high) const			{return const_iterator{interval_exact_search(root, low, high)};}
	const_iterator find_exact(Interval<T> interval) const	{return const_iterator{interval_exact_search(root, interval.low, interval.high)};}
	std::vector<NP> find_all(T low, T high) const 			{return interval_all_search(root, low, high);}
	std::vector<NP> find_all(Interval<T> interval) const 	{return interval_all_search(root, interval.low, interval.high);}

	void print() const {
		walk_and_print_indented(root, [](const Node* node){
			std::cout << '[' << std::setw(6) << node->key << ',' << std::setw(6) << node->high << ']' << '(' << std::setw(6) << node->max << ')' << " -> ";},
			27);
	}
};

template <typename T>
struct Internode {
	static Internode* nil;
	using key_type = T;

	Internode *parent, *left, *right;
	T key, high, max;	// low is the key
	Color color;
	Internode() : max{std::numeric_limits<T>::lowest()}, color{Color::BLACK} {}	// sentinel construction
	Internode(const T& l, const T& h) : parent{nil}, left{nil}, right{nil}, key{l}, high{h}, max{h},  color{Color::RED} {}
	// convert to interval
	operator Interval<T>() {return {key, high};}
};


template <typename T>
struct Intertreap_node {
	static Intertreap_node* nil;
	using key_type = T;

	Intertreap_node *parent, *left, *right;
	T key, high, max;
	int priority;
	Intertreap_node() : priority{std::numeric_limits<int>::max()} {}	// sentinel construction
	Intertreap_node(const T& l, const T& h) : parent{nil}, left{nil}, right{nil}, key{l}, high{h}, max{h}, priority{rand()} {}
	// convert to interval
	operator Interval<T>() {return {key, high};}
};
template <typename T>
std::ostream& operator<<(std::ostream& os, const Internode<T>& interval) {
	os << '[' << interval.key << ',' << interval.high << ']';
	return os;
}
template <typename T>
std::ostream& operator<<(std::ostream& os, const Internode<T>* interval) {
	os << '[' << interval->key << ',' << interval->high << ']';
	return os;
}

template <typename T>
std::ostream& operator<<(std::ostream& os, const Intertreap_node<T>& interval) {
	os << '[' << interval.key << ',' << interval.high << ']';
	return os;
}
template <typename T>
std::ostream& operator<<(std::ostream& os, const Intertreap_node<T>* interval) {
	os << '[' << interval->key << ',' << interval->high << ']';
	return os;
}


// nils, explicitely allocated
template <typename T>
Internode<T>* Internode<T>::nil {new Internode{}};
template <typename T>
Intertreap_node<T>* Intertreap_node<T>::nil {new Intertreap_node{}};

// template <typename T>
// using Interval_set = Interval_augment<Internode<T>>;
// treap as base implementation is 4 times faster than RB for insertion
template <typename T>
using Interval_set = Interval_treap<Intertreap_node<T>>;

}