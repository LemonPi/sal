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
		// climb up via rotations until priority's max-heap property is fulfilled
		while (node != root && node->priority < node->parent->priority) {
			if (is_left_child(node)) rotate_right(node->parent);
			else rotate_left(node->parent);
		}
	}
	template <typename Op>
	void treap_insert(NP node, Op&& fixup) {
		tree_insert(root, node, fixup);
		heap_fix_up(node);
	}
	template <typename Op>
	void treap_delete(NP node, Op&& fixup) {
		// 4 cases based on the children of node
		// either no children or just one child
		if 		(node->left == Node::nil) transplant(node, node->right);	// no left child, become right child
		else if (node->right == Node::nil) transplant(node, node->left);	// no right child, become left child
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
		fixup(node);
		delete node;
	}

	// core treap utilities
	// rotations to balance tree
	/* rotate left shifts everything left s.t. 
	   it becomes the left child of its original right child
	   its right child is replaced by its original right child's left child
	             A <--
	            / \   |
	           /   \  |
	          B     C
	         / \   / \
	        /   \ /   \
	       D    E F    G
				
		rotate_left(A) transforms into

				 C
				/ \
			   /   \
			  A     G
			 / \
			/   \
		   B     F
		  / \
		 /   \
		D     E
	*/
	virtual void rotate_left(NP node) {
		NP child {node->right};

		node->right = child->left;
		if (child->left != Node::nil) child->left->parent = node;

		child->parent = node->parent;
		if (node->parent == Node::nil) root = child;
		else if (is_left_child(node)) node->parent->left = child;
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
		else if (is_left_child(node)) node->parent->left = child;
		else node->parent->right = child;

		child->right = node;
		node->parent = child;	
	}
	// cannot do a simple tree_find for insert since parent has to be updated
	template <typename Op>
	void tree_insert(NP start, NP node, Op&& fixup) {
		NP parent {Node::nil};
		while (start != Node::nil) {
			fixup(start, node);
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
		// move moved into old's position in the tree without deleting any trees
		// no parent must mean that old is root or nil itself
		if (old->parent == Node::nil) root = moved;
		// old is a left child
		else if (is_left_child(old)) old->parent->left = moved;
		else old->parent->right = moved;
		// can assign to parent unconditionally due to sentinel (we know it's not nullptr)
		moved->parent = old->parent;
		// updating moved's children and erasing old is up to the caller
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
		treap_insert(node, [](const Node*, const Node*){});
	};

	void erase(const T& data) {
		NP node {tree_find(root, data)};
		if (node != Node::nil) treap_delete(node, [](const Node*){});
	}
	void clear() {
		postorder_walk(root, [](NP node){delete node;});
		root = Node::nil;
	}

	// query
	iterator find_and_elevate(const T& key) {
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
	// with hints / starting points
	iterator find(const T& key, const_iterator hint) {
		NP found {tree_find(hint.get(),key)};
		return iterator{found};
	}	
	const_iterator find(const T& key, const_iterator hint) const {
		NP found {tree_find(hint.get(),key)};
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