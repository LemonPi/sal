#pragma once
#include <iostream>
#include <initializer_list>
#include <unordered_set>

namespace sal {

template <typename Node>
class List {
protected:
	using NP = Node*;
	using NR = Node&;
	using T = typename Node::key_type;
	NP head {nullptr};

public:
	List(T d) : head{new Node(d)} {}
	List(initializer_list<T> l) : head{new Node{*l.begin()}} {
		NP node {head};
		for (auto i = l.begin()+1; i != l.end(); ++i) {
			node->next = new Node{*i};
			node = node->next;
		}
	}

	NR kth_last(size_t k) {
		if (head == nullptr) return T{};
		// get 2 pointers separated by k
		NP n {head};
		NP jump {head};
		while (k) {
			--k; 
			if (!jump) return nullptr; 
			else jump = jump->next;
		}
		// now go down len-k steps
		while (jump->next) {
			n = n->next;
			jump = jump->next;
		}
		return n;	
	}

	// insert at head
	void insert(T d) {
		NP n = new Node{d};
		n->next = head;
		head = n;
	}	
	void insert(NP node) {
		node->next = head;
		head = node;
	}
	void insert_after(T d, NP prev) {
		NP n = new Node{d};
		n->next = prev->next;
		prev->next = n;
	}
	void insert_after(NP node, NP prev) {
		node->next = prev->next;
		prev->next = node;
	}
	// append to tail
	void append(T d) {
		NP end = new Node{d};
		NP n {head};
		while (n->next) 
			n = n->next;	// go to end
		n->next = end;
	}
	void append(NP node) {
		NP n {head};
		while (n->next)
			n = n->next;
		n->next = node;
	}
	NR erase(T d);
	void remove_dup() {
		unordered_set<T> elems(head->data);
		// cannot have duplicate in first element
		for (Node* node = head; node != nullptr && node->next != nullptr; node = node->next) {
			if (elems.find(node->next->data) != elems.end()) {
				NP to_del {node->next};
				node->next = node->next->next;
				delete to_del;
			}
			else elems.insert(node->next->data);
		}
	}

	template <typename TT>
	friend istream& operator>>(istream&, List<TT>&);
	template <typename TT>
	friend ostream& operator<<(ostream&, const List<TT>&);
	void print() const {std::cout << *this;}
};

template <typename Node>
istream& operator>>(istream& is, List<Node>& l) {
	typename Node::key_type tmp;
	is >> tmp;	// first elem
	Node* start {new Node{tmp}};
	Node* node {start};
	while (is >> tmp) {
		node->next = new Node{tmp};
		node = node->next;
	}
	l.append_node(start);	// only need to append once
	return is;
}

template <typename Node>
ostream& operator<<(ostream& os, const List<Node>& l) {
	for (Node* node = l.head; node != nullptr; node = node->next)
		os << node->data << ' ';
	os << endl;
	return os;
}


template <typename T>
struct Basic_list_node {
	T data;
	using key_type = T;
	Basic_list_node* next {nullptr};
	Basic_list_node(T d) : data{d} {}
};


template <typename T>
using Basic_list = List<Basic_list_node<T>>;

}	// end namespace sal