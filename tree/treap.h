#pragma once
#include <cstdlib>	// rand
#include <limits>	// max
#include "tree.h"	// iterators
// Treaps are a combination of BST and min-heap with
// treed on key and heaped on priority, which is randomly generated
// this simulates a tree built with random data, prevents imbalance
// parent pointer not really necessary
namespace sal {

template <typename Node>
class Treap {
protected:
	using NP = Node*;
	using T = typename Node::key_type;

	NP root {Node::nil};

	// treap specific methods
	void heap_fix_up(NP node) {
		while (node != root && node->priority < node->parent->priority) {
			if (node == node->parent->left) rotate_right(node->parent);
			else rotate_left(node->parent);
		}
	}
	void treap_insert(NP node) {
		tree_insert(root, node);
		heap_fix_up(node);
	}
	virtual void treap_delete(NP node) {
		// 4 cases based on the children of node
		// either no children or just one child
		if 		(node->left == Node::nil) transplant(node, node->right);
		else if (node->right == Node::nil) transplant(node, node->left);
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
		delete node;
	}

	// core treap utilities
	// rotations to preserve RB properties
	/* rotate left shifts everything left s.t. 
	   it becomes the left child of its original right child
	   its right child is replaced by its right child's left child
	*/
	virtual void rotate_left(NP node) {
		NP child {node->right};

		node->right = child->left;
		if (child->left != Node::nil) child->left->parent = node;

		child->parent = node->parent;
		if (node->parent == Node::nil) root = child;
		else if (node == node->parent->left) node->parent->left = child;
		else node->parent->right = child;

		child->left = node;
		node->parent = child;	
	}
	// rotate right shifts everything right, inverse of rotate left
	virtual void rotate_right(NP node) {
		NP child {node->left};

		node->left = child->right;
		if (child->right != Node::nil) child->right->parent = node;

		child->parent = node->parent;
		if (node->parent == Node::nil) root = child;
		else if (node == node->parent->left) node->parent->left = child;
		else node->parent->right = child;

		child->right = node;
		node->parent = child;	
	}
	// cannot do a simple tree_find for insert since parent has to be updated
	virtual void tree_insert(NP start, NP node) {
		NP parent {Node::nil};
		while (start != Node::nil) {
			parent = start;
			if (node->key < start->key) start = start->left;
			else start = start->right;
		}
		node->parent = parent;
		if (parent == Node::nil) root = node;	// tree was empty
		else if (node->key < parent->key) parent->left = node;
		else parent->right = node;
	}

	// moves one subtree to replace another one
	void transplant(NP old, NP moved) {
		if (old->parent == Node::nil) root = moved;
		else if (old == old->parent->left) old->parent->left = moved;
		else old->parent->right = moved;
		// can assign to parent unconditionally due to sentinel
		moved->parent = old->parent;
		// updating moved's children is up to the caller
	}

public:
	using value_type = T;
	using pointer = Node*;
	using iterator = Tree_iterator<Node>;
	using const_iterator = Tree_const_iterator<Node>;
	Treap() = default;
	Treap(std::initializer_list<T> l) {
		for (const auto& v : l) insert(v);
	}
	virtual ~Treap() {clear();}

	// modifiers
	void insert(const T& data) {
		NP node {new Node(data)};
		treap_insert(node);
	};

	void erase(const T& data) {
		NP node {tree_find(root, data)};
		if (node != Node::nil) treap_delete(node);
	}
	void clear() {
		postorder_walk(root, [](NP node){delete node;});
		root = Node::nil;
	}

	// query
	iterator find_and_elevante(const T& key) {
		NP found {tree_find(root, key)};
		// elevate the found node's priority for better temporal locality
		if (found != Node::nil) {
			found->priority >>= 1;
			// fix any min-heap violations
			heap_fix_up(found);
		}
		return iterator{found};
	}
	iterator find(const T& key) {
		NP found {tree_find(root,key)};
		return iterator{found};
	}	
	const_iterator find(const T& key) const {
		NP found {tree_find(root,key)};
		return const_iterator{found};
	}	
	size_t size() const {
		size_t num_elems {0};
		inorder_walk(root, [&](const Node* node){++num_elems;});
		return num_elems;
	}
	bool empty() const {return root == Node::nil;}

	// iterators
	iterator begin() 			 {return iterator{tree_min(root)};}
	iterator end() 				 {return iterator{Node::nil};}
	const_iterator begin() const {return const_iterator{tree_min(root)};}
	const_iterator end() const 	 {return const_iterator{Node::nil};}

	void print() const {
		inorder_walk(root, [](NP node){std::cout << node->key << '(' << node->priority << ')' << ' ';});
		std::cout << "root: " << root->key << '(' << root->priority << ')' << std::endl; 
	}

};

template <typename T>
struct Treap_node {
	static Treap_node* nil;
	using key_type = T;

	Treap_node *parent, *left, *right;
	T key;
	int priority;
	Treap_node() : priority{std::numeric_limits<int>::max()} {}	// sentinel construction
	Treap_node(T val) : parent{nil}, left{nil}, right{nil}, key{val}, priority{rand()} {}
};


template <typename T>
Treap_node<T>* Treap_node<T>::nil {new Treap_node{}};

template <typename T>
using Basic_treap = Treap<Treap_node<T>>;

}	// end namespace sal