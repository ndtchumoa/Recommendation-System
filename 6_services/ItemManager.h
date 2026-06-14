#ifndef ITEM_MANAGER_H
#define ITEM_MANAGER_H

#include <string>
#include "../2_data/MyDataStructures.h"
#include "../1_models/Item.h"

class ItemManager {
private:
    MyVector<Item> items;

public:
    ItemManager(const MyVector<Item>& loaded_items);

    MyVector<Item> GetHomeProducts(const std::string& userId, int limit) const;
    MyVector<Item> SearchProducts(const std::string& keyword) const;
    MyVector<Item> FilterAndSortProducts(const std::string& category,
                                          const std::string& badge,
                                          double minStar,
                                          const std::string& sortBy) const;
    bool GetProductDetail(const std::string& productId, Item& out) const;
    const MyVector<Item>& getAllItems() const;
};

#endif // ITEM_MANAGER_H