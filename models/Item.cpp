#include "Item.h"
#include <iostream>
#include <iomanip>

Item::Item(const std::string& id,
           const std::string& name,
           const std::string& category,
           double price)
    : item_id(id), name(name), category(category), price(price) {}

void Item::print() const {
    std::cout << "[Item] ID=" << item_id
              << " | Name=" << name
              << " | Category=" << category
              << " | Price=" << std::fixed << std::setprecision(0) << price
              << " VND\n";
}
