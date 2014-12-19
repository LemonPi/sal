#pragma once
#include <limits>	// lowest()
#include <vector>
#include "tree.h"

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

// interval trees augmented from base RB-tree
template <typename T>
class Interval_set {
	struct Internode {
		T low, high, max;	// low is the key
		Internode *parent, *left, *right;
		Color color;
		Internode() : max{std::numeric_limits<T>::lowest()}, color{Color::BLACK} {}	// sentinel construction
		Internode(T l, T h, Color col = Color::RED) : low{l}, high{h}, max{h}, parent{nil}, left{nil}, right{nil}, color{col} {}
		// convert to interval
		operator Interval<T>() {return {low, high};}
	};

	using NP = typename Interval_set::Internode*;
	NP root {nil};
	static NP nil;

	// interval tree operation
	bool no_overlap(NP interval, T low, T high) const {
		return (low > interval->high || interval->low > high);
	}
	bool overlap(NP interval, T low, T high) const {
		return !no_overlap(interval, low, high);
	}
	void update_max(NP interval) {
		interval->max = max(interval->high, max(interval->left->max, interval->right->max));
	}
	// either finds an overlapping interval or nil
	NP interval_search(NP interval, T low, T high) const {
		while (interval != nil && no_overlap(interval, low, high)) {
			if (interval->left != nil && interval->left->max >= low)
				interval = interval->left;
			else interval = interval->right; 
		}
		return interval;
	}
	NP interval_min_search(NP interval, T low, T high) const {
		// check if left subtree overlaps
		if (interval->left != nil && interval->left->max >= low) {
			NP min {interval_min_search(interval->left, low, high)};
			// there exists an overlapping interval with lower low
			if (min != nil) return min;
			else if (overlap(interval, low, high)) return interval;
			else return nil;
		}
		// if left subtree doesn't overlap, then check if current one does
		else if (overlap(interval, low, high)) return interval;
		else return interval_min_search(interval->right, low, high);
	}
	std::vector<NP> interval_all_search(NP interval, T low, T high) const {
		std::vector<NP> res;
		if (overlap(interval, low, high)) res.push_back(interval);
		if (interval->left != nil && interval->left->max >= low) {
			std::vector<NP> left {interval_all_search(interval->left, low, high)};
			res.insert(res.end(), left.begin(), left.end());
		} 
		if (interval->right != nil && interval->right->low <= low) {
			std::vector<NP> right {interval_all_search(interval->right, low, high)};
			res.insert(res.end(), right.begin(), right.end());
		}
		return res;
	}
	NP interval_exact_search(NP start, T low, T high) {
		NP interval {tree_find(start, low)};
		// if found interval shares same low but different high, can continually search on same subtree
		while (interval != nil && high != interval->high) interval = tree_find(interval, low);
		return interval;
	}


	// core rb utilities
	NP tree_find(NP start, T low) const {
		while (start != nil && start->low != low) {
			if (low < start->low) start = start->left;
			else start = start->right;
		}
		return start;
	}
	NP tree_min(NP start) const {
		while (start->left != nil) start = start->left;
		return start;
	}
	NP tree_max(NP start) const {
		while (start->right != nil) start = start->right;
		return start;
	}
	// successor is node with smallest low greater than start
	NP tree_successor(NP start) const {
		if (start->right) return tree_min(start->right);
		// else go up until a node that's the left child of parent
		NP parent {start->parent};
		while (parent != nil && start == parent->right) {
			start = parent;
			parent = parent->parent;
		}
		return parent;
	}
	NP tree_predecessor(NP start) const {
		if (start->left) return tree_max(start->left);
		// else go up until a node that's the right child of parent
		NP parent {start->parent};
		while (parent != nil && start == parent->left) {
			start = parent;
			parent = parent->parent;
		}
		return parent;
	}

	// assume node is colored RED
	void rb_insert(NP node) {
		tree_insert(root, node);
		rb_insert_fixup(node);
	}
	void rb_insert_fixup(NP node) {
		// node is red, either its parents are red or it is the root
		while (node->parent->color == Color::RED) {
			NP parent {node->parent};
			// grandparent is black since parent is red
			if (parent == parent->parent->left) {
				NP uncle {parent->parent->right};
				// case 1, simply recolor so parents and uncle are black while grandparent red
				if (uncle->color == Color::RED) {
					parent->color = Color::BLACK;
					uncle->color = Color::BLACK;
					parent->parent->color = Color::RED;
					// now violation (if any) is in grandparent layer
					node = parent->parent;	
				}
				// case 2 and 3, uncle is black, cannot recolor layers
				else { 
					if (node == parent->right) {
						node = parent;
						rotate_left(node);
					}
					// case 2 reduces to case 3
					parent->color = Color::BLACK;
					parent->parent->color = Color::RED;
					rotate_right(parent->parent);
					// properties after rotation must be restored
				}
			}
			// else parent is a right child, mirror the cases above
			else {
				// uncle is now the other child of grandparent
				NP uncle {parent->parent->left};
				if (uncle->color == Color::RED) {
					parent->color = Color::BLACK;
					uncle->color = Color::BLACK;
					parent->parent->color = Color::RED;
					node = parent->parent;	
				}
				else { 
					if (node == parent->left) {
						node = parent;
						rotate_right(node);
					}
					parent->color = Color::BLACK;
					parent->parent->color = Color::RED;
					rotate_left(parent->parent);
				}				
			}
		}
		// fix the terminating case of a red root, which can always be turned black
		root->color = Color::BLACK;
	}

	void rb_delete_fixup(NP successor) {
		// successor starts black-black, always has 1 extra black
		// move extra black up tree until 
		// 1. successor is red-black
		// 2. successor is root, where extra black is removed
		while (successor != root && successor->color == Color::BLACK) {
			NP parent {successor->parent};
			if (successor == parent->left) {
				// sibling cannot be nil since successor is black (so bh is at least 1)
				NP sibling {parent->right};
				// case 1, red sibling is made black, and reduces to other cases
				if (sibling->color == Color::RED) {
					// parent must be black to avoid parent-sibling both red
					sibling->color = Color::BLACK;
					parent->color = Color::RED;
					rotate_left(parent);
					sibling = parent->right;	// previously sibling's left child
				}
				// cases 2,3,4 with sibling black, depending on its children's color
				// case 2, remove a level of black off both sibling and successor
				if (sibling->left->color == Color::BLACK && sibling->right->color == Color::BLACK) {
					sibling->color = Color::RED;
					successor = parent;	// parent now double black
				}
				else {
					// case 3, sibling's left is red, switch sibling and its left child's color
					if (sibling->right->color == Color::BLACK) {
						sibling->left->color = Color::BLACK;
						sibling->color = Color::RED;
						rotate_right(sibling);
						sibling = parent->right;	// previously sibling's left child
					}
					// case 4, sibling's right is red
					sibling->color = parent->color;
					parent->color = Color::BLACK;
					sibling->right->color = Color::BLACK;
					rotate_left(parent);
					successor = root;
				}
			}
			// successor is right child, mirror above
			else {
				NP sibling {parent->left};

				if (sibling->color == Color::RED) {
					sibling->color = Color::BLACK;
					parent->color = Color::RED;
					rotate_right(parent);
					sibling = parent->left;
				}

				if (sibling->right->color == Color::BLACK && sibling->left->color == Color::BLACK) {
					sibling->color = Color::RED;
					successor = parent;	// parent now double black
				}
				else {
					if (sibling->left->color == Color::BLACK) {
						sibling->right->color = Color::BLACK;
						sibling->color = Color::RED;
						rotate_left(sibling);
						sibling = parent->left;	
					}
					sibling->color = parent->color;
					parent->color = Color::BLACK;
					sibling->left->color = Color::BLACK;
					rotate_right(parent);
					successor = root;
				}
			}
		}
		successor->color = Color::BLACK;
	}
	// moves one subtree to replace another one
	void transplant(NP old, NP moved) {
		if (old->parent == nil) root = moved;
		else if (old == old->parent->left) old->parent->left = moved;
		else old->parent->right = moved;
		// can assign to parent unconditionally due to sentinel
		moved->parent = old->parent;
		// updating moved's children is up to the caller
	}


	// rb operations modified for intervals
	void tree_insert(NP start, NP node) {
		NP parent {nil};
		while (start != nil) {
			start->max = max(start->max, node->max);	// update max
			parent = start;
			if (node->low < start->low) start = start->left;
			else start = start->right;
		}
		node->parent = parent;
		if (parent == nil) root = node;
		else if (node->low < parent->low) parent->left = node;
		else parent->right = node;
	}

	void rb_delete(NP node) {
		NP old {node};
		NP moved {node};
		NP successor;
		Color moved_original_color {moved->color};
		if (node->left == nil) {
			successor = node->right;
			transplant(node, node->right);
		}
		else if (node->right == nil) {
			successor = node->left;
			transplant(node, node->left);
		}
		else {
			moved = tree_min(node->right);
			moved_original_color = moved->color;
			successor = moved->right;
			if (moved->parent == node) successor->parent = moved;
			else {
				transplant(moved, moved->right);
				moved->right = node->right;
				moved->right->parent = moved;
			}

			transplant(node, moved);
			moved->left = node->left;
			moved->left->parent = moved;
			moved->color = node->color;
		}
		// update all ancestors' max
		while (moved != nil) {
			update_max(moved);
			moved = moved->parent;
		}
		if (moved_original_color == Color::BLACK) rb_delete_fixup(successor);
		delete old;
	}

	// rotations, augmented by changing child and node's sizes
	void rotate_left(NP node) {
		NP child {node->right};

		node->right = child->left;
		if (child->left != nil) child->left->parent = node;

		child->parent = node->parent;
		if (node->parent == nil) root = child;
		else if (node == node->parent->left) node->parent->left = child;
		else node->parent->right = child;

		child->left = node;
		node->parent = child;

		update_max(node);
		update_max(child);
	}
	void rotate_right(NP node) {
		NP child {node->left};

		node->left = child->right;
		if (child->right != nil) child->right->parent = node;

		child->parent = node->parent;
		if (node->parent == nil) root = child;
		else if (node == node->parent->left) node->parent->left = child;
		else node->parent->right = child;

		child->right = node;
		node->parent = child;	

		update_max(node);
		update_max(child);
	}

public:
	Interval_set() = default;
	Interval_set(std::initializer_list<Interval<T>> l) {
		for (const auto& v : l) insert(v);
	}
	~Interval_set() {
		postorder_walk(root, [](NP interval){delete interval;});
	}

	void insert(T low, T high) {
		NP node {new Internode(low, high)};
		rb_insert(node);
	};
	void insert(Interval<T> interval) {
		NP node {new Internode(interval.low, interval.high)};
		rb_insert(node);
	}

	void erase(T low, T high) {
		NP interval {interval_exact_search(root, low, high)};
		if (interval != nil) rb_delete(interval);
	}
	void erase(Interval<T> interval) {
		NP candidate {interval_exact_search(root, interval.low, interval.high)};
		if (candidate != nil) rb_delete(candidate);
	}

	// interval tree operation - find overlapping interval
	NP find(T low, T high) {
		return interval_search(root, low, high);
	}
	NP find(Interval<T> interval) {
		return interval_search(root, interval.low, interval.high);
	}
	NP find_first(T low, T high) {
		return interval_min_search(root, low, high);
	}
	NP find_first(Interval<T> interval) {
		return interval_min_search(root, interval.low, interval.high);
	}
	std::vector<NP> find_all(T low, T high) {
		return interval_all_search(root, low, high);
	}
	std::vector<NP> find_all(Interval<T> interval) {
		return interval_all_search(root, interval.low, interval.high);
	}
	NP find_exact(T low, T high) {
		return interval_exact_search(root, low, high);
	}
	NP find_exact(Interval<T> interval) {
		return interval_exact_search(root, interval.low, interval.high);
	}

	// traversals, Op is a function that performs a function on a NP
	template <typename Op>
	friend void preorder_walk(NP start, Op op) {
		if (start != nil) {
			op(start);
			inorder_walk(start->left, op);
			inorder_walk(start->right, op);
		}
	}
	template <typename Op>
	friend void inorder_walk(NP start, Op op) {
		if (start != nil) {
			inorder_walk(start->left, op);
			op(start);
			inorder_walk(start->right, op);
		}
	}
	template <typename Op>
	friend void postorder_walk(NP start, Op op) {
		if (start != nil) {
			inorder_walk(start->left, op);
			inorder_walk(start->right, op);
			op(start);
		}
	}


	void print() const {
		inorder_walk(root, [](NP node){std::cout << '[' << node->low << ',' << node->high << ']' << '(' << node->max << ')' << ' ';});
		std::cout << "root: " << '[' << root->low << ',' << root->high << ']' << '(' << root->max << ')' << std::endl; 
	}
	const static NP get_nil() {
		return nil;
	}
};

template <typename T>
typename Interval_set<T>::NP Interval_set<T>::nil {new Internode{}};

}