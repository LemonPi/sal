#pragma once
#include <cstdlib>	// rand
#include <limits>	// max
// Treaps are a combination of BST and min-heap with
// treed on key and heaped on priority, which is randomly generated
// this simulates a tree built with random data, prevents imbalance
namespace sal {

template <typename T>
class Treap {
	struct Treap_node {
		T key;
		int priority;
		Treap_node *parent, *left, *right;
		Treap_node() : priority{std::numeric_limits<int>::max()} {}	// sentinel construction
		Treap_node(T val) : key{val}, priority{rand()}, parent{nil}, left{nil}, right{nil} {}
	};

	using NP = typename Treap::Treap_node*;

	NP root {nil};
	// nil sentinel
	static NP nil;

	// treap specific methods
	void treap_insert(NP node) {
		tree_insert(root, node);
		while (node != root && node->priority < node->parent->priority) {
			if (node == node->parent->left) rotate_right(node->parent);
			else rotate_left(node->parent);
		}
	}
	void treap_delete(NP node) {
		// 4 cases based on the children of node
		NP old {node};
		// either no children or just one child
		if 		(!node->left) transplant(node, node->right);
		else if (!node->right) transplant(node, node->left);
		// else both children, find successor in right subtree
		else {
			NP successor {tree_min(node->right)};
			if (successor->parent != node) {	// not immediate right child
				// replace successor by its right child, then give successor node's right subtree
				transplant(successor, successor->right);
				successor->right = node->right;
				successor->right->parent = successor;
			}
			transplant(node, successor);
			successor->left = node->left;
			successor->left->parent = successor; 
			// restore priority
			while (successor->priority > successor->left->priority || successor->priority > successor->right->priority) {
				if (successor->left->priority < successor->right->priority) rotate_right(successor);
				else rotate_left(successor);
			}
		}
		delete old;
	}

	// core tree utilities
	NP tree_find(NP start, T key) const {
		while (start != nil && start->key != key) {
			if (key < start->key) start = start->left;
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
	// successor is node with smallest key greater than start
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
	// rotations to preserve RB properties
	/* rotate left shifts everything left s.t. 
	   it becomes the left child of its original right child
	   its right child is replaced by its right child's left child
	*/
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
	}
	// rotate right shifts everything right, inverse of rotate left
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
	}
	// cannot do a simple tree_find for insert since parent has to be updated
	void tree_insert(NP start, NP node) {
		NP parent {nil};
		while (start != nil) {
			parent = start;
			if (node->key < start->key) start = start->left;
			else start = start->right;
		}
		node->parent = parent;
		if (parent == nil) root = node;	// tree was empty
		else if (node->key < parent->key) parent->left = node;
		else parent->right = node;
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


public:
	Treap() = default;
	Treap(std::initializer_list<T> l) {
		for (const auto& v : l) insert(v);
	}
	~Treap() {
		postorder_walk(root, [](NP node){delete node;});
	}

	void insert(T data) {
		NP node {new Treap_node(data)};
		treap_insert(node);
	};

	void erase(T data) {
		NP node {tree_find(root, data)};
		if (node != nil) treap_delete(node);
	}

	NP find(T key) {
		return tree_find(root, key);
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
		inorder_walk(root, [](NP node){std::cout << node->key << '(' << node->priority << ')' << ' ';});
		std::cout << "root: " << root->key << '(' << root->priority << ')' << std::endl; 
	}

	const static NP get_nil() {
		return nil;
	}
};

template <typename T>
typename Treap<T>::NP Treap<T>::nil {new Treap_node{}};

}	// end namespace sal