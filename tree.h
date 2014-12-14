#pragma once
#include <iostream>
#include <initializer_list>

namespace sal {

// RB tree
template <typename T>
class Tree {
	struct Node {
		T key;
		Node *parent {nullptr}, *left {nullptr}, *right {nullptr};
		Node(T val) : key{val} {}
	};
	using NP = typename Tree::Node*;

	NP root {nullptr};

	// core utilities
	NP tree_find(NP start, T key) {
		while (start && start->key != key) {
			if (key < start->key) start = start->left;
			else start = start->right;
		}
		return start;
	}
	NP tree_min(NP start) {
		while (start->left) start = start->left;
		return start;
	}
	NP tree_max(NP start) {
		while (start->right) start = start->right;
		return start;
	}
	// successor is node with smallest key greater than start
	NP tree_successor(NP start) {
		if (start->right) return tree_min(start->right);
		// else go up until a node that's the left child of parent
		NP parent {start->parent};
		while (parent && start == parent->right) {
			start = parent;
			parent = parent->parent;
		}
		return parent;
	}
	NP tree_predecessor(NP start) {
		if (start->left) return tree_max(start->left);
		// else go up until a node that's the right child of parent
		NP parent {start->parent};
		while (parent && start == parent->left) {
			start = parent;
			parent = parent->parent;
		}
		return parent;
	}
	// cannot do a simple tree_find for insert since parent has to be updated
	void tree_insert(NP start, NP node) {
		// NP start {root};
		NP parent {nullptr};
		while (start) {
			parent = start;
			if (node->key < start->key) start = start->left;
			else start = start->right;
		}
		node->parent = parent;
		if (!parent) root = node;	// tree was empty
		else if (node->key < parent->key) parent->left = node;
		else parent->right = node;
	}

	// moves one subtree to replace another one
	void transplant(NP old, NP moved) {
		if (!old->parent) root = moved;
		else if (old == old->parent->left) old->parent->left = moved;
		else old->parent->right = moved;
		if (moved) moved->parent = old->parent;
		// updating moved's children is up to the caller
	}
	void tree_delete(NP node) {
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
		}
		delete old;
	}

	// traversals, Op is a function that performs a function on a NP
	template <typename Op>
	void preorder_walk(NP start, Op op) {
		if (start) {
			op(start);
			inorder_walk(start->left, op);
			inorder_walk(start->right, op);
		}
	}
	template <typename Op>
	void inorder_walk(NP start, Op op) {
		if (start) {
			inorder_walk(start->left, op);
			op(start);
			inorder_walk(start->right, op);
		}
	}
	template <typename Op>
	void postorder_walk(NP start, Op op) {
		if (start) {
			inorder_walk(start->left, op);
			inorder_walk(start->right, op);
			op(start);
		}
	}

public:
	Tree() = default;
	Tree(std::initializer_list<T> l) {
		for (const auto& v : l) insert(v);
	}
	~Tree() {
		postorder_walk(root, [](NP node){delete node;});
	}

	void insert(T data) {
		NP node {new Node(data)};
		tree_insert(root, node);
	};

	void erase(T data) {
		NP node {tree_find(root, data)};
		if (node) tree_delete(node);
	}

	NP find(T key) {
		return tree_find(root, key);
	}

	void print() {
		inorder_walk(root, [](NP node){std::cout << node->key << ' ';});
		std::cout << "root: " << root->key << std::endl; 
	}

};

}	// end namespace sal