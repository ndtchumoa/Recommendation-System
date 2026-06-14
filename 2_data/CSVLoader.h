#ifndef CSVLOADER_H
#define CSVLOADER_H

#include <string>
#include "MyDataStructures.h"
#include "../1_models/User.h"
#include "../1_models/Item.h"
#include "../1_models/Interaction.h"

class CSVLoader {
private:
    MyVector<User> users;
    MyVector<Item> items;
    MyVector<Interaction> interactions;

    std::string trim(const std::string& str) const;
    std::string escapeField(const std::string& field) const;

public:
    CSVLoader() = default;

    bool loadUsers(const std::string& filepath);
    bool loadItems(const std::string& filepath);
    bool loadInteractions(const std::string& filepath);

    const MyVector<User>& getUsers() const { return users; }
    const MyVector<Item>& getItems() const { return items; }
    const MyVector<Interaction>& getInteractions() const { return interactions; }

    bool saveUsers(const std::string& filepath, const MyVector<User>& users_to_save) const;
    bool saveItems(const std::string& filepath, const MyVector<Item>& items_to_save) const;
    bool saveInteractions(const std::string& filepath, const MyVector<Interaction>& interactions_to_save) const;
};

#endif