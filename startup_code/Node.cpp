#include "Node.h"

Price::Price()
    : dollars(0), cents(0)
{
}

Price::Price(unsigned dollars, unsigned cents)
    : dollars(dollars), cents(cents)
{
}

Stock::Stock()
    : on_hand(0)
{
}

Stock::Stock(std::string id, std::string name, std::string description, Price price, unsigned on_hand)
    : id(id), name(name), description(description), price(price), on_hand(on_hand)
{
}

Node::Node()
    : data(nullptr), next(nullptr), prev(nullptr){};

Node::Node(Stock *data)
    : data(data), next(nullptr), prev(nullptr){};

Node::~Node(){};
