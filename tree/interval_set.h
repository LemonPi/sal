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
template <typename Node>
class Interval_augment : public Tree<Node> {

	using NP = Node*;
	using T = typename Node::key_type;
	using Tree<Node>::root;
	using Tree<Node>::rb_insert;
	using Tree<Node>::transplant;
	using Tree<Node>::rb_insert_fixup;
	using Tree<Node>::rb_delete_fixup;

	// interval tree operation
	bool no_overlap(NP interval, T low, T high) const {
		return (low > interval->high || interval->key > high);
	}
	bool overlap(NP interval, T low, T high) const {
		return !no_overlap(interval, low, high);
	}
	void update_max(NP interval) {
		interval->max = max(interval->high, max(interval->left->max, interval->right->max));
	}
	// either finds an overlapping interval or nil
	NP interval_search(NP interval, T low, T high) const {
		while (interval != Node::nil && no_overlap(interval, low, high)) {
			if (interval->left != Node::nil && interval->left->max >= low)
				interval = interval->left;
			else interval = interval->right; 
		}
		return interval;
	}
	NP interval_min_search(NP interval, T low, T high) const {
		// check if left subtree overlaps
		if (interval->left != Node::nil && interval->left->max >= low) {
			NP min {interval_min_search(interval->left, low, high)};
			// there exists an overlapping interval with lower low
			if (min != Node::nil) return min;
			else if (overlap(interval, low, high)) return interval;
			else return Node::nil;
		}
		// if left subtree doesn't overlap, then check if current one does
		else if (overlap(interval, low, high)) return interval;
		else return interval_min_search(interval->right, low, high);
	}
	std::vector<NP> interval_all_search(NP interval, T low, T high) const {
		std::vector<NP> res;
		if (overlap(interval, low, high)) res.push_back(interval);
		if (interval->left != Node::nil && interval->left->max >= low) {
			std::vector<NP> left {interval_all_search(interval->left, low, high)};
			res.insert(res.end(), left.begin(), left.end());
		} 
		if (interval->right != Node::nil && interval->right->key <= low) {
			std::vector<NP> right {interval_all_search(interval->right, low, high)};
			res.insert(res.end(), right.begin(), right.end());
		}
		return res;
	}
	NP interval_exact_search(NP start, T low, T high) {
		NP interval {tree_find(start, low)};
		// if found interval shares same low but different high, can continually search on same subtree
		while (interval != Node::nil && high != interval->high) interval = tree_find(interval, low);
		return interval;
	}

	// rb operations modified for intervals
	virtual void tree_insert(NP start, NP node) override {
		NP parent {Node::nil};
		while (start != Node::nil) {
			start->max = max(start->max, node->max);	// update max
			parent = start;
			if (node->key < start->key) start = start->left;
			else start = start->right;
		}
		node->parent = parent;
		if (parent == Node::nil) root = node;
		else if (node->key < parent->key) parent->left = node;
		else parent->right = node;
	}

	virtual void rb_delete(NP node) override {
		NP moved {node};
		NP successor;
		Color moved_original_color {moved->color};
		if (node->left == Node::nil) {
			successor = node->right;
			transplant(node, node->right);
		}
		else if (node->right == Node::nil) {
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
		while (moved != Node::nil) {
			update_max(moved);
			moved = moved->parent;
		}
		if (moved_original_color == Color::BLACK) rb_delete_fixup(successor);
		delete node;
	}

	// rotations, augmented by changing child and node's sizes
	virtual void rotate_left(NP node) override {
		NP child {node->right};

		node->right = child->left;
		if (child->left != Node::nil) child->left->parent = node;

		child->parent = node->parent;
		if (node->parent == Node::nil) root = child;
		else if (node == node->parent->left) node->parent->left = child;
		else node->parent->right = child;

		child->left = node;
		node->parent = child;

		update_max(node);
		update_max(child);
	}
	virtual void rotate_right(NP node) override {
		NP child {node->left};

		node->left = child->right;
		if (child->right != Node::nil) child->right->parent = node;

		child->parent = node->parent;
		if (node->parent == Node::nil) root = child;
		else if (node == node->parent->left) node->parent->left = child;
		else node->parent->right = child;

		child->right = node;
		node->parent = child;	

		update_max(node);
		update_max(child);
	}

public:
	Interval_augment() = default;
	Interval_augment(std::initializer_list<Interval<T>> l) {
		for (const auto& v : l) insert(v);
	}

	void insert(T low, T high) {
		NP node {new Node(low, high)};
		rb_insert(node);
	};
	void insert(Interval<T> interval) {
		NP node {new Node(interval.low, interval.high)};
		rb_insert(node);
	}

	void erase(T low, T high) {
		NP interval {interval_exact_search(root, low, high)};
		if (interval != Node::nil) rb_delete(interval);
	}
	void erase(Interval<T> interval) {
		NP candidate {interval_exact_search(root, interval.low, interval.high)};
		if (candidate != Node::nil) rb_delete(candidate);
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

	void print() const {
		inorder_walk(root, [](NP node){std::cout << '[' << node->key << ',' << node->high << ']' << '(' << node->max << ')' << ' ';});
		std::cout << "root: " << '[' << root->key << ',' << root->high << ']' << '(' << root->max << ')' << std::endl; 
	}
};

template <typename T>
struct Internode {
	static Internode* nil;
	using key_type = T;

	T key, high, max;	// low is the key
	Internode *parent, *left, *right;
	Color color;
	Internode() : max{std::numeric_limits<T>::lowest()}, color{Color::BLACK} {}	// sentinel construction
	Internode(T l, T h) : key{l}, high{h}, max{h}, parent{nil}, left{nil}, right{nil}, color{Color::RED} {}
	// convert to interval
	operator Interval<T>() {return {key, high};}
};

template <typename T>
Internode<T>* Internode<T>::nil {new Internode{}};

template <typename T>
using Interval_set = Interval_augment<Internode<T>>;

}