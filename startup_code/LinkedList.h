#ifndef LINKEDLIST_H
#define LINKEDLIST_H
#include "Node.h"

/**
 * A linked list of stocks.
 *
 * Example usage:
 *   LinkedList list;
 *   list.insert(new Stock("AAPL", 100, ...));
 *   list.insert(new Stock("GOOG", 200, ...));
 *   list.insert(new Stock("MSFT", 300, ...));
 *   for (LinkedList::Iterator it = list.begin(); it != list.end(); ++it) {
 *       Stock &stock = *it;
 *       // do something with stock
 *   }
 */
class LinkedList {
  public:
    LinkedList();
    ~LinkedList();

    // Iterator class for iterating over the list.
    class Iterator {
      public:
        Iterator(LinkedList *list, Node *node);
        Iterator operator++();
        Iterator operator++(int);
        bool operator==(const Iterator &other);
        bool operator!=(const Iterator &other);
        Stock &operator*();
        Stock *operator->();

        friend class LinkedList;

      private:
        LinkedList *list;
        Node *node;
    };

    // Insert a stock into the list, sorted by name.
    bool insert(Stock *data);

    // Remove the given stock from the list.
    // Note that after calling this function, the given iterator is moved to the
    // next element in the list.
    void remove(Iterator &it);

    // Clear the list.
    void clear();

    // Return the number of stocks in the list.
    unsigned size() const;

    // Return true if the list is empty.
    bool empty() const;

    // Return an iterator pointing to the beginning of the list.
    Iterator begin();

    // Return an iterator pointing to the end of the list.
    Iterator end();

  private:
    // the beginning of the list
    Node *head;
    Node *tail;

    // how many nodes are there in the list?
    unsigned count;
};

#endif // LINKEDLIST_H
