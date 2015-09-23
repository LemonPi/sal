#pragma once
#include "interval_set.h"	
// 2D interval set
namespace sal {

// currently support only insert and query
template <typename Node>
class Plane_treap : public Treap<Node> {	// interval set of interval sets (2D plane for overlapping rectangles)
protected:
	using T = typename Node::key_type;
	using Treap<Node>::treap_insert;
	using Treap<Node>::treap_delete;
	using Treap<Node>::root;
	// rotations, augmented by changing child and node's sizes
	virtual void rotate_left(Node* node) override {
		// child always becomes the parent of node
		Node* child {node->right};
		Treap<Node>::rotate_left(node);
		// additional fixup for interval sets
		update_max(node);
		update_max(child);
	}
	virtual void rotate_right(Node* node) override {
		Node* child {node->left};
		Treap<Node>::rotate_right(node);
		update_max(node);
		update_max(child);
	}

public:
	using pointer = Node*;
	using key_type = T;
	// iterator over the individual elements rather than x ranges
	using element_node = typename Node::node_type;
	using iterator  = typename Treap<element_node>::iterator;
	using const_iterator = typename Treap<element_node>::const_iterator;

	using Treap<Node>::size;
	using Treap<Node>::empty;

	Plane_treap() = default;

	// modifiers
	// key_type should be easily copyable, so use copy instead of reference
	void insert(T x_low, T x_high, T y_low, T y_high) {
		auto x_interval = interval_exact_search(root, x_low, x_high);
		// no interval exists, need to create interval
		if (x_interval == Node::nil) {
			x_interval = new Node{x_low, x_high};	// x is our first dimension
			treap_insert(x_interval, propagate_max_down<Node>);
		}
		x_interval->insert(y_low, y_high);
	}

	// query
	iterator find(T x_low, T x_high, T y_low, T y_high) {
		// find any rectangle that intersects with the given one, returns end if not found
		// first find intervals on the x dimension
		auto x_range = interval_all_search(root, x_low, x_high);
		for (auto& x_intersected_node : x_range) {
			auto xy_intersected_node = x_intersected_node->find(y_low, y_high);
			if (xy_intersected_node != end()) return xy_intersected_node;
		}
		return end();
	}
	const_iterator find(T x_low, T x_high, T y_low, T y_high) const {
		// const version
		auto x_range = interval_all_search(root, x_low, x_high);
		for (auto& x_intersected_node : x_range) {
			auto xy_intersected_node = x_intersected_node->find(y_low, y_high);
			if (xy_intersected_node != end()) return xy_intersected_node;
		}
		return end();
	}

	// don't try to iterator through planes, just use end to check against find
	iterator end() 				{return iterator{element_node::nil};}
	const_iterator end() const 	{return iterator{element_node::nil};}


	void print() const {
		// for each x range, print out its y ranges
		sal::inorder_walk(root, [](const Node* x_range){
			x_range->inorder_walk([&](const element_node* y_range){
				std::cout << '(' << x_range->key << ',' << x_range->high << ',' << y_range->key << ',' << y_range->high << ")\n";
			});});
	}
};

// T is supposed to be easy to copy
template <typename T>
struct Planetreap_node {
	static Planetreap_node* nil;
	using key_type = T;
	using node_type = Intertreap_node<key_type>;
	using set_type = Interval_treap<node_type>;
	using iterator = typename set_type::iterator;
	using const_iterator = typename set_type::const_iterator;

	set_type y_range;

	Planetreap_node *parent, *left, *right;
	T key, high, max;
	int priority;
	Planetreap_node() : priority{std::numeric_limits<int>::max()} {}	// sentinel construction
	Planetreap_node(T l, T h) : parent{nil}, left{nil}, right{nil}, key{l}, high{h}, max{h}, priority{rand()} {}

	// forward to the y_range interval set
	iterator find(T low, T high) {return y_range.find(low, high);}
	const_iterator find(T low, T high) const {return y_range.find(low, high);}
	void insert(T y_low, T y_high) {y_range.insert(y_low, y_high);}
	template <typename Op>
	void inorder_walk(Op&& op) {y_range.inorder_walk(std::forward<Op>(op));}
	template <typename Op>
	void inorder_walk(Op&& op) const {y_range.inorder_walk(std::forward<Op>(op));}
	// convert to interval
	operator Interval<T>() {return {key, high};}
};

// convenient printing
template <typename T>
std::ostream& operator<<(std::ostream& os, const Planetreap_node<T>& interval) {
	os << '[' << interval.key << ',' << interval.high << ']';
	return os;
}
template <typename T>
std::ostream& operator<<(std::ostream& os, const Planetreap_node<T>* interval) {
	os << '[' << interval->key << ',' << interval->high << ']';
	return os;
}

// nils explicitely allocated
template <typename T>
Planetreap_node<T>* Planetreap_node<T>::nil {new Planetreap_node{}};


template <typename T>
using Plane_set = Plane_treap<Planetreap_node<T>>;


}