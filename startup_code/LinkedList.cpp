#include "LinkedList.h"
#include <cassert>

LinkedList::Iterator::Iterator(LinkedList *list, Node *node) : list(list), node(node) {
}

LinkedList::Iterator LinkedList::Iterator::operator++() {
    node = node->next;
    return *this;
}

LinkedList::Iterator LinkedList::Iterator::operator++(int) {
    Iterator it = *this;
    node = node->next;
    return it;
}

bool LinkedList::Iterator::operator==(const Iterator &other) {
    assert(list == other.list);
    return node == other.node;
}

bool LinkedList::Iterator::operator!=(const Iterator &other) {
    assert(list == other.list);
    return node != other.node;
}

Stock &LinkedList::Iterator::operator*() {
    return *node->data;
}

Stock *LinkedList::Iterator::operator->() {
    return node->data;
}

LinkedList::LinkedList() : head(nullptr), tail(nullptr), count(0) {
}

LinkedList::~LinkedList() {
    clear();
}

bool LinkedList::insert(Stock *data) {
    assert(data != nullptr);

    // Insert data, sorted by name

    Node *node = new Node;
    node->data = data;

    if (head == nullptr) {
        node->prev = nullptr;
        node->next = nullptr;
        head = node;
        tail = node;
    } else {
        Node *curr = head;
        while (curr != nullptr && curr->data->name < data->name) {
            curr = curr->next;
        }

        if (curr == nullptr) {
            node->prev = tail;
            node->next = nullptr;
            tail->next = node;
            tail = node;
        } else {
            node->prev = curr->prev;
            node->next = curr;
            if (curr->prev != nullptr) {
                curr->prev->next = node;
            } else {
                head = node;
            }
            curr->prev = node;
        }
    }

    count++;

    return true;
}






void LinkedList::remove(Iterator &it) {
    assert(it.list == this);
    assert(it.node != nullptr);

    Node *node = it.node;
    if (node->prev != nullptr) {
        node->prev->next = node->next;
    } else {
        head = node->next;
    }

    if (node->next != nullptr) {
        node->next->prev = node->prev;
    } else {
        tail = node->prev;
    }

    it.node = node->next;
    delete node;

    count--;
}

void LinkedList::clear() {
    Node *node = head;
    while (node != nullptr) {
        Node *next = node->next;
        delete node->data;
        delete node;
        node = next;
    }
    head = nullptr;
    tail = nullptr;
    count = 0;
}

unsigned LinkedList::size() const {
    return count;
}

bool LinkedList::empty() const {
    return count == 0;
}

LinkedList::Iterator LinkedList::begin() {
    return Iterator(this, head);
}
LinkedList::Iterator LinkedList::end() {
    return Iterator(this, nullptr);
}
