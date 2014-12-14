#pragma once
#include <iostream>
#include <initializer_list>
#include <unordered_set>

using namespace std;

template <typename T>
struct Node {
	T data;
	Node* next {nullptr};
	Node(T d) : data{d} {}
};

template <typename T>
class Mylist {
	Node<T>* head {nullptr};
public:
	Mylist(T d) : head{new Node<T>(d)} {}
	Mylist(initializer_list<T>);

	Node<T>& kth_last(size_t k);

	void append(T d);
	void append_node(Node<T>* node);
	Node<T>& remove(T d);
	void remove_dup();

	template <typename TT>
	friend istream& operator>>(istream&, Mylist<TT>&);
	template <typename TT>
	friend ostream& operator<<(ostream&, const Mylist<TT>&);
	void print() const {cout << *this;}
};

template <typename T>
Mylist<T>::Mylist(initializer_list<T> l) : head{new Node<T>(*l.begin())} {
	Node<T>* n {head};
	for (auto i = l.begin()+1; i != l.end(); ++i) {
		n->next = new Node<T>{*i};
		n = n->next;
	}
}

template <typename T>
Node<T>& Mylist<T>::kth_last(size_t k) {
	if (head == nullptr) return T();
	// get 2 pointers separated by k
	Node<T>* n {head};
	Node<T>* jump {head};
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

template <typename T>
void Mylist<T>::append(T d) {
	Node<T>* end = new Node<T>{d};
	Node<T>* n {head};
	while (n->next) 
		n = n->next;	// go to end
	n->next = end;
}
template <typename T>
void Mylist<T>::append_node(Node<T>* node) {
	Node<T>* n {head};
	while (n->next)
		n = n->next;
	n->next = node;
}
template <typename T>
Node<T>& Mylist<T>::remove(T d) {	// returns node immediately after delete, nullptr means not found
	Node<T>* n {head};
	if (head->data == d) {
		head = head->next;
		delete n;
		return head;
	}

	while (n->next) {
		if (n->next->data == d) {
			Node<T>* to_del {n->next};
			n->next = to_del->next;
			delete to_del;
			return n->next;
		}
	}
	return nullptr;
}

template <typename T>
void Mylist<T>::remove_dup() {
	unordered_set<T> elems(head->data);
	// cannot have duplicate in first element
	for (Node<T>* n = head; n != nullptr && n->next != nullptr; n = n->next) {
		if (elems.find(n->next->data) != elems.end()) {
			Node<T>* to_del {n->next};
			n->next = n->next->next;
			delete to_del;
		}
		else elems.insert(n->next->data);
	}
}

template <typename T>
istream& operator>>(istream& is, Mylist<T>& l) {
	T tmp;
	is >> tmp;	// first elem
	Node<T>* start {new Node<T>(tmp)};
	Node<T>* n {start};
	while (is >> tmp) {
		n->next = new Node<T>(tmp);
		n = n->next;
	}
	l.append_node(start);	// only need to append once
	return is;
}

template <typename T>
ostream& operator<<(ostream& os, const Mylist<T>& l) {
	for (Node<T>* n = l.head; n != nullptr; n = n->next)
		os << n->data << ' ';
	os << endl;
	return os;
}