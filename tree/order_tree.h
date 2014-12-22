#pragma once
#include "tree.h"

namespace sal {

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
	NP os_select(NP start, size_t rank) const {
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

	size_t os_rank(NP node) {
		size_t rank {node->left->size + 1};
		while (node != root) {
			if (node == node->parent->right) 
				rank += node->parent->left->size + 1;
			node = node->parent;
		}
		return rank;
	}

	// rb operations modified for order statistics
	virtual void tree_insert(NP start, NP node) override {
		NP parent {Node::nil};
		while (start != Node::nil) {
			++start->size;	// simply increment size of each ancestor going down
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
		NP old {node};
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
		// decrement size of ancestors of moved
		moved = moved->parent;
		while (moved != Node::nil) {
			--moved->size;
			moved = moved->parent;
		}
		if (moved_original_color == Color::BLACK) rb_delete_fixup(successor);
		delete old;
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
	Order_augment() = default;
	Order_augment(std::initializer_list<T> l) {
		for (const auto& v : l) insert(v);
	}

	void insert(T data) {
		NP node {new Node(data)};
		rb_insert(node);
	};

	void erase(T data) {
		NP node {tree_find(root, data)};
		if (node != Node::nil) rb_delete(node);
	}

	NP find(T key) {
		return tree_find(root, key);
	}

	// order statistics methods interface
	NP select(size_t rank) const {
		return os_select(root, rank);
	}
	size_t rank(NP node) {
		return os_rank(node);
	}

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