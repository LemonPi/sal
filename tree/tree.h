#pragma once
#include <iostream>
#include <iomanip>
#include <initializer_list>

namespace sal {
// no virtual methods since not meant to be polymorphic (e.g. don't point to an Order_tree with a Tree*)
// RB tree, 4 properties:
// 1. Every node either red or black
// 2. Root and leaves (nil) are black
// 3. A red node's children are both black / Parent of red node is black
// 4. All simple paths from a node to descendent have same # of black
enum class Color : char {BLACK = 0, RED = 1};

// core utilities
// either finds key or the location where the key should go
template <typename Node>
Node* tree_find(Node* start, const typename Node::key_type& key) {
	while (start != Node::nil && start->key != key) {
		if (key < start->key) start = start->left;
		else start = start->right;
	}
	return start;
}
template <typename Node>
Node* tree_min(Node* start) {
	while (start->left != Node::nil) start = start->left;
	return start;
}
template <typename Node>
Node* tree_max(Node* start) {
	while (start->right != Node::nil) start = start->right;
	return start;
}
// const versions
template <typename Node>
const Node* tree_find(const Node* start, const typename Node::key_type& key) {
	while (start != Node::nil && start->key != key) {
		if (key < start->key) start = start->left;
		else start = start->right;
	}
	return start;
}
template <typename Node>
const Node* tree_min(const Node* start) {
	while (start->left != Node::nil) start = start->left;
	return start;
}
template <typename Node>
const Node* tree_max(const Node* start) {
	while (start->right != Node::nil) start = start->right;
	return start;
}


// successor is node with smallest key greater than start
template <typename Node>
Node* tree_successor(const Node* start) {
	if (start->right != Node::nil) return tree_min(start->right);
	// else go up until a node that's the left child of parent
	Node* parent {start->parent};
	while (parent != Node::nil && start == parent->right) {
		start = parent;
		parent = parent->parent;
	}
	return parent;
}
template <typename Node>
Node* tree_predecessor(const Node* start) {
	if (start->left != Node::nil) return tree_max(start->left);
	// else go up until a node that's the right child of parent
	Node* parent {start->parent};
	while (parent != Node::nil && start == parent->left) {
		start = parent;
		parent = parent->parent;
	}
	return parent;
}
// convenient and semantically clear functions
template <typename Node>
bool is_left_child(const Node* node) {
	return node == node->parent->left;
}
template <typename Node>
bool is_right_child(const Node* node) {
	return node == node->parent->right;
}

// BST traversals
template <typename Node, typename Op>
void preorder_walk(Node* start, Op&& op) {
	if (start != Node::nil) {
		op(start);
		preorder_walk(start->left, op);
		preorder_walk(start->right, op);
	}
}
template <typename Node, typename Op>
void inorder_walk(Node* start, Op&& op) {
	if (start != Node::nil) {
		inorder_walk(start->left, op);
		op(start);
		inorder_walk(start->right, op);
	}
}
template <typename Node, typename Op>
void postorder_walk(Node* start, Op&& op) {
	if (start != Node::nil) {
		postorder_walk(start->left, op);
		postorder_walk(start->right, op);
		op(start);
	}
}
// const overloads
template <typename Node, typename Op>
void preorder_walk(const Node* start, Op&& op) {
	if (start != Node::nil) {
		op(start);
		preorder_walk(start->left, op);
		preorder_walk(start->right, op);
	}
}
template <typename Node, typename Op>
void inorder_walk(const Node* start, Op&& op) {
	if (start != Node::nil) {
		inorder_walk(start->left, op);
		op(start);
		inorder_walk(start->right, op);
	}
}
template <typename Node, typename Op>
void postorder_walk(const Node* start, Op&& op) {
	if (start != Node::nil) {
		postorder_walk(start->left, op);
		postorder_walk(start->right, op);
		op(start);
	}
}

template <typename Node, typename Print>
void walk_and_print_indented(const Node* start, Print&& printer, int indent_step_size, int indent_level = 0, bool new_branch = false) {
	if (start == Node::nil) return;
	if (new_branch) std::cout << '\n' << std::setw(indent_step_size * indent_level) << "\\ ";
	printer(start);
	// top / same level is right branch, further down is left branch
	walk_and_print_indented(start->right, printer, indent_step_size, indent_level+1, false);
	walk_and_print_indented(start->left, printer, indent_step_size, indent_level+1, true);
}


// no-op fixup operators
template<typename Node>
void skip_insert_fixup(const Node*, const Node*) {}
template<typename Node>
void skip_delete_fixup(const Node*) {}


// iterators
// adjacent iterator (expected of all graphs)
// iterators from left to right if they exist
template <typename Node>
struct Tree_adj_iterator {
	using pointer = Node*;
	using key_type = typename Node::key_type;
	using CR = const Tree_adj_iterator<Node>&;

	pointer cur;

	void operator++() {if (is_left_child(cur)) cur = cur->parent->right; else cur = Node::nil;}
	void operator--() {if (is_right_child(cur)) cur = cur->parent->left; else cur = Node::nil;}
	key_type& operator*() {return cur->key;}
	pointer operator->() {return cur;}
	pointer get() 		{return cur;}
	bool operator==(CR other) const {return other.cur == cur;}
	bool operator!=(CR other) const {return !(*this == other);}	
	friend std::ostream& operator<<(std::ostream& os, CR itr) {os << *itr.cur; return os;}
};
// const version
template <typename Node>
struct Tree_adj_const_iterator {
	using const_pointer = const Node*;
	using key_type = typename Node::key_type;
	using CR = const Tree_adj_const_iterator<Node>&;

	const_pointer cur;

	void operator++() {if (is_left_child(cur)) cur = cur->parent->right; else cur = Node::nil;}
	void operator--() {if (is_right_child(cur)) cur = cur->parent->left; else cur = Node::nil;}
	const key_type& operator*() const {return cur->key;}
	const_pointer operator->() const {return cur;}
	const_pointer get() const {return cur;}
	bool operator==(CR other) const {return other.cur == cur;}
	bool operator!=(CR other) const {return !(*this == other);}	
};

// node iterators
// full traversal takes O(n) time and O(1) space
// traverse at most 2(n - 1) edges, where n is # nodes
// non-const bidirectional iterator
template <typename Node>
struct Tree_iterator {
	using key_type = typename Node::key_type;
	using CR = const Tree_iterator<Node>&;
	using adjacent_iterator = Tree_adj_iterator<Node>;

	Node* cur;

	void operator++() {cur = tree_successor(cur);}
	void operator--() {cur = tree_predecessor(cur);}
	key_type& operator*() {return cur->key;}
	Node* operator->() {return cur;}
	Node* get()		{return cur;}
	bool operator==(CR other) const {return other.cur == cur;}
	bool operator!=(CR other) const {return !(*this == other);}
	adjacent_iterator begin() {
		if (cur->left != Node::nil) return {cur->left};
		else return {cur->right};
	}
	adjacent_iterator end()   {return {Node::nil};}
	friend std::ostream& operator<<(std::ostream& os, CR itr) {os << *itr.cur; return os;}
};
// const bidirectional iterator
template <typename Node>
struct Tree_const_iterator {
	using key_type = typename Node::key_type;
	using CR = const Tree_const_iterator<Node>&;
	using adjacent_const_iterator = Tree_adj_const_iterator<Node>;

	const Node* cur;

	void operator++() {cur = tree_successor(cur);}
	void operator--() {cur = tree_predecessor(cur);}
	const key_type& operator*() const {return cur->key;}
	const Node* operator->() const {return cur;}
	const Node* get() 		  const {return cur;}
	bool operator==(CR other) const {return other.cur == cur;}
	bool operator!=(CR other) const {return !(*this == other);}
	adjacent_const_iterator begin() const {
		if (cur->left != Node::nil) return {cur->left};
		else return {cur->right};
	}
	adjacent_const_iterator end()   const {return {Node::nil};}
};

template <typename Node>
class Tree {
protected:
	friend struct Tree_iterator<Node>;
	using NP = Node*;
	using T = typename Node::key_type;

	NP root {Node::nil};
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
			// keep track of augmented data
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
	// assume node is colored RED
	template <typename Op>
	void rb_insert(NP node, Op&& fixup) {
		tree_insert(root, node, std::forward<Op>(fixup));
		rb_insert_fixup(node);
	}
	void rb_insert_fixup(NP node) {
		// node is red, either its parents are red or it is the root
		while (node->parent->color == Color::RED) {
			NP parent {node->parent};
			// grandparent is black since parent is red
			if (is_left_child(parent)) {
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

	template <typename Op>
	void rb_delete(NP node, Op&& fixup) {
		// successor replaces the moved node
		// moved either has 1 or no children
		NP moved {node};
		// successor is either the single child of moved or Node::nil
		NP successor;
		Color moved_original_color {moved->color};
		// < 2 children, successor is just the other child
		if (node->left == Node::nil) {
			successor = node->right;
			transplant(node, node->right);
		}
		else if (node->right == Node::nil) {
			successor = node->left;
			transplant(node, node->left);
		}
		// both children, successor replaces node
		else {
			moved = tree_min(node->right);
			moved_original_color = moved->color;
			successor = moved->right;
			// immediate right child of node
			if (moved->parent == node) successor->parent = moved;
			else {
				// transplant assigns successor's parents
				transplant(moved, moved->right);
				moved->right = node->right;
				moved->right->parent = moved;
			}

			transplant(node, moved);
			moved->left = node->left;
			moved->left->parent = moved;
			moved->color = node->color;
		}
		// additional fixup to restore augmented data
		fixup(moved);
		// possible violation if a black node was moved
		if (moved_original_color == Color::BLACK) rb_delete_fixup(successor);
		delete node;
	}
	void rb_delete_fixup(NP successor) {
		// successor starts black-black, always has 1 extra black
		// move extra black up tree until 
		// 1. successor is red-black
		// 2. successor is root, where extra black is removed
		while (successor != root && successor->color == Color::BLACK) {
			NP parent {successor->parent};
			if (successor == parent->left) {
				// sibling cannot be Node::nil since successor is black (so bh is at least 1)
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
		if (old->parent == Node::nil) root = moved;
		else if (is_left_child(old)) old->parent->left = moved;
		else old->parent->right = moved;
		// can assign to parent unconditionally due to sentinel
		moved->parent = old->parent;
		// updating moved's children is up to the caller
	}

	// test if valid rb tree, returns black height rooted at root
	int blackheight(NP root) const {
		if (root == Node::nil) return 1;
		NP left {root->left};
		NP right {root->right};
		if (root->color == Color::RED) {
			if (left->color == Color::RED || right->color == Color::RED) {
				std::cout << "Consequtive red nodes\n";
				return 0;
			}
		}
	
		if ((left != Node::nil && left->key > root->key) &&
		   (right != Node::nil && right->key < root->key)) {
			std::cout << "Basic binary tree violation\n";
			return 0;
		}

		int left_bh {blackheight(left)};
		int right_bh {blackheight(right)};
		if (left_bh != 0 && right_bh != 0 && left_bh != right_bh) {
			// check for 0 first to pardon carry-on error
			std::cout << "Different black height\n";
			return 0;
		}

		if (left_bh != 0 && right_bh != 0) return root->color == Color::RED? left_bh : left_bh + 1;
		else return 0; 
	}

public:
	using value_type = T;
	using iterator = Tree_iterator<Node>;
	using const_iterator = Tree_const_iterator<Node>;
	using adjacent_iterator = Tree_adj_iterator<Node>;
	using adjacent_const_iterator = Tree_adj_iterator<Node>;

	Tree() = default;
	Tree(std::initializer_list<T> l) {
		for (const auto& v : l) insert(v);
	}
	virtual ~Tree() {
		clear();
	}

	// modifiers (non-virtual)
	void insert(T data) {
		NP node {new Node(data)};
		rb_insert(node, skip_insert_fixup<Node>);	// no additional work (useful only for augments)
	}
	template <typename...Args>
	void emplace(Args&&... args) {
		NP node {new Node(std::forward<Args>(args)...)};
		rb_insert(node);
	}
	void erase(T data) {
		NP node {tree_find(root, data)};
		if (node != Node::nil) rb_delete(node, skip_delete_fixup<Node>);	// no additional work (useful only for augments)
	}
	void clear() {
		postorder_walk(root, [](NP node){delete node;});
		root = Node::nil;
	}			


	// query and retrieval
	iterator find(T key) 			 {return iterator{tree_find(root, key)};}
	const_iterator find(T key) const {return const_iterator{tree_find(root, key)};}
	// O(n) size lookup; not terribly efficient...
	size_t size() const {
		size_t num_elems {0};
		inorder_walk(root, [&](const Node* node){++num_elems;});
		return num_elems;
	}
	bool empty() const {return root == Node::nil;}
	// get root for traversal purposes
	iterator get_root()				{return iterator{root};}
	const_iterator get_root() const {return const_iterator{root};}


	// iteration
	iterator begin() 				{return iterator{tree_min(root)};}
	iterator end() 					{return iterator{Node::nil};}
	const_iterator begin() const 	{return const_iterator{tree_min(root)};}
	const_iterator end() const 		{return const_iterator{Node::nil};}

	// adjacent iterators for compatibility with graph algorithms
	// assumes node is in tree
	std::pair<adjacent_iterator, adjacent_iterator> adjacent(NP node) {
		// begins on the first valid node, or nil
		if (node->left != Node::nil) return {{node->left}, {Node::nil}};
		else return {{node->right}, {Node::nil}};
	}
	std::pair<adjacent_const_iterator, adjacent_const_iterator> adjacent(NP node) const {
		if (node->left != Node::nil) return {{node->left}, {Node::nil}};
		else return {{node->right}, {Node::nil}};
	}
	// overloaded version for data
	std::pair<adjacent_iterator, adjacent_iterator> adjacent(T data) {
		NP node {tree_find(root, data)};
		if (node->left != Node::nil) return {{node->left}, {Node::nil}};
		else return {{node->right}, {Node::nil}};
	}
	std::pair<adjacent_const_iterator, adjacent_const_iterator> adjacent(T data) const {
		NP node {tree_find(root, data)};
		if (node->left != Node::nil) return {{node->left}, {Node::nil}};
		else return {{node->right}, {Node::nil}};
	}	


	void print() const {
		walk_and_print_indented(root, [](const Node* node){std::cout << node->key << ' ';}, 5);
	}
	// 0 for invalid, else blackheight
	bool valid() const {
		return blackheight(root) != 0;
	}

};


template <typename T>
struct Basic_node {
	static Basic_node* nil;
	using key_type = T;

	Basic_node *parent, *left, *right;
	T key;
	Color color;
	Basic_node() : color{Color::BLACK} {}	// sentinel construction
	Basic_node(T val) : parent{nil}, left{nil}, right{nil}, key{val}, color{Color::RED} {}
};

template <typename T>
using Basic_tree = Tree<Basic_node<T>>;

template <typename T>
Basic_node<T>* Basic_node<T>::nil {new Basic_node{}};

}	// end namespace sal