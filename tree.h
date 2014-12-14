#pragma once
#include <iostream>
#include <initializer_list>

namespace sal {

// RB tree, 4 properties:
// 1. Every node either red or black
// 2. Root and leaves (nil) are black
// 3. A red node's children are both black / Parent of red node is black
// 4. All simple paths from a node to descendent have same # of black
template <typename T>
class Tree {
	enum class Color : char {BLACK = 0, RED = 1};

	struct Node {
		T key;
		Node *parent, *left, *right;
		Color color;
		Node() : color{Color::BLACK} {}	// sentinel construction
		Node(T val, Color col = Color::RED) : key{val}, parent{nil}, left{nil}, right{nil}, color{col} {}
	};
	using NP = typename Tree::Node*;

	NP root {nil};
	// nil sentinel
	static NP nil;

	// core utilities
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
	// assume node is colored RED
	void rb_insert(NP node) {
		tree_insert(root, node);
		rb_insert_fixup(node);
	}
	void rb_insert_fixup(NP node) {
		// node is red, either its parents are red or it is the root
		while (node->parent->color == Color::RED) {
			// grandparent is black since parent is red
			if (node->parent == node->parent->parent->left) {
				NP uncle {node->parent->parent->right};
				// case 1, simply recolor so parents and uncle are black while grandparent red
				if (uncle->color == Color::RED) {
					node->parent->color = Color::BLACK;
					uncle->color = Color::BLACK;
					node->parent->parent->color = Color::RED;
					// now violation (if any) is in grandparent layer
					node = node->parent->parent;	
				}
				// case 2 and 3, uncle is black, cannot recolor layers
				else { 
					if (node == node->parent->right) {
						node = node->parent;
						rotate_left(node);
					}
					// case 2 reduces to case 3
					node->parent->color = Color::BLACK;
					node->parent->parent->color = Color::RED;
					rotate_right(node->parent->parent);
					// properties after rotation must be restored
				}
			}
			// else parent is a right child, mirror the cases above
			else {
				// uncle is now the other child of grandparent
				NP uncle {node->parent->parent->left};
				if (uncle->color == Color::RED) {
					node->parent->color = Color::BLACK;
					uncle->color = Color::BLACK;
					node->parent->parent->color = Color::RED;
					node = node->parent->parent;	
				}
				else { 
					if (node == node->parent->left) {
						node = node->parent;
						rotate_right(node);
					}
					node->parent->color = Color::BLACK;
					node->parent->parent->color = Color::RED;
					rotate_left(node->parent->parent);
				}				
			}
		}
		// fix the terminating case of a red root, which can always be turned black
		root->color = Color::BLACK;
	}
	// moves one subtree to replace another one
	void transplant(NP old, NP moved) {
		if (old->parent == nil) root = moved;
		else if (old == old->parent->left) old->parent->left = moved;
		else old->parent->right = moved;
		if (moved != nil) moved->parent = old->parent;
		// updating moved's children is up to the caller
	}
	void tree_delete(NP node) {
		// 4 cases based on the children of node
		NP old {node};
		// either no children or just one child
		if 		(node->left == nil) transplant(node, node->right);
		else if (node->right == nil) transplant(node, node->left);
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
		if (start != nil) {
			op(start);
			inorder_walk(start->left, op);
			inorder_walk(start->right, op);
		}
	}
	template <typename Op>
	void inorder_walk(NP start, Op op) {
		if (start != nil) {
			inorder_walk(start->left, op);
			op(start);
			inorder_walk(start->right, op);
		}
	}
	template <typename Op>
	void postorder_walk(NP start, Op op) {
		if (start != nil) {
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
		rb_insert(node);
	};

	void erase(T data) {
		NP node {tree_find(root, data)};
		if (node != nil) tree_delete(node);
	}

	NP find(T key) {
		return tree_find(root, key);
	}

	void print() {
		inorder_walk(root, [](NP node){std::cout << node->key << ' ';});
		std::cout << "root: " << root->key << std::endl; 
	}

	const static NP get_nil() {
		return nil;
	}
};

template <typename T>
typename Tree<T>::NP Tree<T>::nil {new Node{}};

}	// end namespace sal