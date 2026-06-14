#include "CSVLoader.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>

std::string CSVLoader::trim(const std::string& str) const {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (std::string::npos == first) return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}

std::string CSVLoader::escapeField(const std::string& field) const {
    bool needsQuoting =
        field.find(',')  != std::string::npos ||
        field.find('"')  != std::string::npos ||
        field.find('\n') != std::string::npos ||
        field.find('\r') != std::string::npos;

    if (!needsQuoting) return field;

    std::string out = "\"";
    for (char c : field) {
        if (c == '"') out += "\"\""; 
        else           out += c;
    }
    out += "\"";
    return out;
}

bool CSVLoader::loadUsers(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "[Error] Cannot open file: " << filepath << std::endl;
        return false;
    }
    std::string line;
    std::getline(file, line); 
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        std::stringstream ss(line);
        std::string id, name, date;
        std::getline(ss, id, ',');
        std::getline(ss, name, ',');
        std::getline(ss, date, ',');
        std::string email    = trim(id) + "@email.com";
        std::string password = "password123";
        users.push_back(User(trim(id), trim(name), email, password, trim(date)));
    }
    file.close();
    return true;
}

bool CSVLoader::loadItems(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "[Error] Cannot open file: " << filepath << std::endl;
        return false;
    }
    std::string line;
    std::getline(file, line); 
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        std::stringstream ss(line);
        std::string id, name, category, price_str;
        std::getline(ss, id, ',');
        std::getline(ss, name, ',');
        std::getline(ss, category, ',');
        std::getline(ss, price_str, ',');
        double price = price_str.empty() ? 0.0 : std::stod(trim(price_str));
        std::string badge = "";
        if (price >= 10000000) badge = "Hot";
        else if (trim(category) == "Books") badge = "Sale";
        double star = 4.0;
        int sold = 100;
        std::string img = trim(category) + "/" + trim(id) + ".jpg";
        items.push_back(Item(trim(id), trim(name), trim(category), badge, price, star, sold, img));
    }
    file.close();
    return true;
}

bool CSVLoader::loadInteractions(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "[Error] Cannot open file: " << filepath << std::endl;
        return false;
    }
    std::string line;
    std::getline(file, line); 
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        std::stringstream ss(line);
        std::string u_id, i_id, click_str, cart_str, purchase_str, rating_str;
        std::getline(ss, u_id, ',');
        std::getline(ss, i_id, ',');
        std::getline(ss, click_str, ',');
        std::getline(ss, cart_str, ',');
        std::getline(ss, purchase_str, ',');
        std::getline(ss, rating_str, ',');
        int clicks     = click_str.empty()    ? 0 : std::stoi(trim(click_str));
        int carts      = cart_str.empty()     ? 0 : std::stoi(trim(cart_str));
        int purchases  = purchase_str.empty() ? 0 : std::stoi(trim(purchase_str));
        double rating  = rating_str.empty()   ? 0.0 : std::stod(trim(rating_str));
        interactions.push_back(Interaction(trim(u_id), trim(i_id), clicks, carts, purchases, rating));
    }
    file.close();
    return true;
}

bool CSVLoader::saveUsers(const std::string& filepath, const MyVector<User>& users_to_save) const {
    std::ofstream file(filepath);
    if (!file.is_open()) return false;
    file << "user_id,name,created_at\n";
    for (int i = 0; i < users_to_save.size(); ++i) {
        file << escapeField(users_to_save[i].user_id) << ","
             << escapeField(users_to_save[i].name) << ","
             << escapeField(users_to_save[i].created_at) << "\n";
    }
    return true;
}

bool CSVLoader::saveItems(const std::string& filepath, const MyVector<Item>& items_to_save) const {
    std::ofstream file(filepath);
    if (!file.is_open()) return false;
    file << "item_id,name,category,price\n";
    for (int i = 0; i < items_to_save.size(); ++i) {
        file << escapeField(items_to_save[i].item_id) << ","
             << escapeField(items_to_save[i].name) << ","
             << escapeField(items_to_save[i].category) << ","
             << std::fixed << std::setprecision(0) << items_to_save[i].price << "\n";
    }
    return true;
}

bool CSVLoader::saveInteractions(const std::string& filepath, const MyVector<Interaction>& interactions_to_save) const {
    std::ofstream file(filepath);
    if (!file.is_open()) return false;
    file << "user_id,item_id,click,add_cart,purchase,rating\n";
    for (int i = 0; i < interactions_to_save.size(); ++i) {
        file << escapeField(interactions_to_save[i].user_id) << ","
             << escapeField(interactions_to_save[i].item_id) << ","
             << interactions_to_save[i].click_count << ","
             << interactions_to_save[i].add_cart_count << ","
             << interactions_to_save[i].purchase_count << ","
             << std::fixed << std::setprecision(1) << interactions_to_save[i].rating << "\n";
    }
    return true;
}