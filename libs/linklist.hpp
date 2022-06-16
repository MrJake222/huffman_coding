#pragma once

#include <sstream>
#include <iostream>

namespace lnklist {

template<class T>
class LinkList;

/*
 * Node class
 */

template<class T>
class Node {

    Node<T>* next_ = nullptr;
    T value_;

public:
    Node() { }
    Node(T value) : value_(value) { }

    T get_value() const { return value_; }
    void set_value(T value) { value_ = value; }
    Node* get_next() const { return next_; }

    friend class LinkList<T>;

    template<class X>
    friend void swap(Node<X>& n1, Node<X>& n2);
};

/*
 * Implementations
 */

template<class T>
void swap(Node<T>& n1, Node<T>& n2) {
    std::swap(n1.value_, n2.value_);
}

template<class T>
std::ostream& operator<<(std::ostream& os, const Node<T>& node) {
    os << node.get_value();
    return os;
}


/*
 * LinkList class
 */

template<class T>
class LinkList {

    Node<T>* head_ = nullptr;

public:
    ~LinkList();

    Node<T>* get_head() const { return head_; }

    void insert_left(T value);
    Node<T>* create_left();

    void print_deref_values();

    /*
     * Iterators
     */
    class Iterator {
        Node<T>* node_;

    public:
        using difference_type = std::ptrdiff_t;
        using value_type = T;
        using pointer = value_type*;
        using reference = value_type&;
        using iterator_category = std::forward_iterator_tag;

        Iterator(Node<T>* node) : node_(node) { }

        Iterator& operator++() { node_ = node_->next_; return *this; }          // prefix
        Iterator operator++(int) { Iterator i = *this; ++*this; return i; } // postfix
        bool operator==(const Iterator& o) const { return node_ == o.node_; }
        bool operator!=(const Iterator& o) const { return !(*this == o); }
        T operator*() { return node_->get_value(); }
    };

    Iterator begin() { return Iterator(head_); }
    Iterator end() { return Iterator(nullptr); }
};



/*
 * Implementations
 */

template<class T>
LinkList<T>::~LinkList() {
    Node<T>* node = get_head();
    while (node) {
        Node<T>* next = node->get_next();
        delete node;
        node = next;
    }
}

template<class T>
void LinkList<T>::insert_left(T value) {
    Node<T>* new_node = new Node<T>(value);
    new_node->next_ = get_head();
    head_ = new_node;
}

template<class T>
Node<T>* LinkList<T>::create_left() {
    Node<T>* new_node = new Node<T>;
    new_node->next_ = get_head();
    head_ = new_node;

    return new_node;
}

template<class T>
std::ostream& operator<<(std::ostream& os, const LinkList<T>& list) {
    Node<T>* node = list.get_head();
    while (node) {
        os << node->get_value() << " -> ";
        node = node->get_next();
    }

    return os;
}

template<class T>
void LinkList<T>::print_deref_values() {
    using std::cout;
    using std::endl;

    Node<T>* node = get_head();
    while (node) {
        T value = node->get_value();
        if (value) {
            cout << *value << " -> ";
        } else {
            cout << "<0>" << " -> ";
        }

        node = node->get_next();
    }

    cout << endl;
}

} // namespace end
