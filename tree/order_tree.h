#pragma once
#include "tree.h"

namespace sal {

// O(h) = O(lgn) time for selecting rank-th smallest element, start at 1
template <typename Node>
const Node* os_select(const Node* start, size_t rank) {
	while (rank > 0) {
		size_t cur_rank {start->left->size + 1};
		if (cur_rank == rank) return start;
		else if (rank < cur_rank) start = start->left;
		else {
			start = start->right;
			rank -= cur_rank;
		}
	}
	return Node::nil;
}
template <typename Node>
Node* os_select(Node* start, size_t rank) {
	while (rank > 0) {
		size_t cur_rank {start->left->size + 1};
		if (cur_rank == rank) return start;
		else if (rank < cur_rank) start = start->left;
		else {
			start = start->right;
			rank -= cur_rank;
		}
	}
	return Node::nil;
}

template <typename Node>
class Order_augment : public Tree<Node> {

	using NP = Node*;
	using T = typename Node::key_type;
	using Tree<Node>::root;
	using Tree<Node>::rb_insert;
	using Tree<Node>::transplant;
	using Tree<Node>::rb_insert_fixup;
	using Tree<Node>::rb_delete_fixup;

	// order statistics operations
	size_t os_rank(NP node) const {
		size_t rank {node->left->size + 1};
		while (node != root) {
			if (node == node->parent->right) 
				rank += node->parent->left->size + 1;
			node = node->parent;
		}
		return rank;
	}

	// rb operations modified for order statistics
	static void grow_subtree_size(Node* start, const Node*) {
		++start->size; // simply increment size of each ancestor going down
	}
	static void update_ancestor_size(Node* moved) {
		moved = moved->parent;
		while (moved != Node::nil) {
			--moved->size;
			moved = moved->parent;
		}		
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

		child->size = node->size;
		node->size = node->left->size + node->right->size + 1;	
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

		child->size = node->size;
		node->size = node->left->size + node->right->size + 1;
	}

public:
	using iterator  = typename Tree<Node>::iterator;
	using const_iterator = typename Tree<Node>::const_iterator;
	Order_augment() = default;
	Order_augment(std::initializer_list<T> l) {
		for (const auto& v : l) insert(v);
	}

	void insert(const T& data) {
		NP node {new Node(data)};
		rb_insert(node, grow_subtree_size);
	};

	void erase(const T& data) {
		NP node {tree_find(root, data)};
		if (node != Node::nil) rb_delete(node, update_ancestor_size);
	}

	// order statistics methods interface
	iterator operator[](size_t rank)		 {return iterator{os_select(root, rank)};}
	iterator select(size_t rank) 			 {return iterator{os_select(root, rank)};}
	const_iterator select(size_t rank) const {return const_iterator{os_select(root, rank)};}
	size_t rank(NP node) const {return os_rank(node);}

	void print() const {
		inorder_walk(root, [](NP node){std::cout << node->key << '(' << node->size << ')' << ' ';});
		std::cout << "root: " << root->key << '(' << root->size << ')' << std::endl; 
	}
};

template <typename T>
struct Order_node {
	static Order_node* nil;

	using key_type = T;
	T key;
	Order_node *parent, *left, *right;
	size_t size;	// # of descendent nodes including itself = left->size + right->size + 1
	Color color;
	Order_node() : size{0}, color{Color::BLACK} {}	// sentinel construction
	Order_node(T val) : key{val}, parent{nil}, left{nil}, right{nil}, size{1}, color{Color::RED} {}
};

template <typename T>
Order_node<T>* Order_node<T>::nil {new Order_node{}};

template <typename T>
using Order_tree = Order_augment<Order_node<T>>;

}	// end namespace sal