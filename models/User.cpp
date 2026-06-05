#include "User.h"
#include <iostream>

User::User(const std::string& id,
           const std::string& name,
           const std::string& created_at)
    : user_id(id), name(name), created_at(created_at) {}

void User::print() const {
    std::cout << "[User] ID=" << user_id
              << " | Name=" << name
              << " | Joined=" << created_at
              << "\n";
}
