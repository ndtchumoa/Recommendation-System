#include "CSVLoader.h"
<<<<<<< HEAD

CSVLoader::CSVLoader() {}
=======
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>
#include <algorithm>
#include <iomanip>
>>>>>>> b8c1ae64c94847b04dee00ae1cc482c9f316487e

// Hàm hỗ trợ xóa khoảng trắng ở đầu và cuối chuỗi
std::string CSVLoader::trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (std::string::npos == first) return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}

<<<<<<< HEAD
// --- Đọc dữ liệu Users ---
// Format thực tế: user_id,name,created_at
// (bổ sung email, password mặc định cho các tính năng Auth mới)
bool CSVLoader::loadUsers(const std::string& filepath) {
=======
std::vector<std::string> CSVLoader::parseLine(const std::string& line) const {
    std::vector<std::string> tokens;
    std::string token;
    bool insideQuotes = false;

    for (size_t i = 0; i < line.size(); ++i) {
        char c = line[i];

        if (c == '"') {
            // Kiểm tra escaped quote: hai dấu "" liền nhau bên trong quotes
            if (insideQuotes && i + 1 < line.size() && line[i + 1] == '"') {
                token += '"';
                ++i; // bỏ qua dấu nháy thứ hai
            } else {
                insideQuotes = !insideQuotes;
            }
        } else if (c == ',' && !insideQuotes) {
            tokens.push_back(trim(token));
            token.clear();
        } else {
            token += c;
        }
    }

    // Token cuối cùng (sau dấu phẩy cuối hoặc cuối dòng)
    tokens.push_back(trim(token));
    return tokens;
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
        if (c == '"') out += "\"\""; // escape dấu nháy kép -> ""
        else           out += c;
    }
    out += "\"";
    return out;
}

// ─────────────────────────────────────────────────────────────────────────────
// Public Methods - Load (CSV -> memory)
// ─────────────────────────────────────────────────────────────────────────────

std::vector<User> CSVLoader::loadUsers(const std::string& filepath) const {
>>>>>>> b8c1ae64c94847b04dee00ae1cc482c9f316487e
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Loi: Khong the mo file " << filepath << std::endl;
        return false;
    }

    std::string line;
    std::getline(file, line); // Bỏ qua dòng tiêu đề

    while (std::getline(file, line)) {
        if (line.empty()) continue;

        std::stringstream ss(line);
        std::string id, name, date;

        std::getline(ss, id, ',');
        std::getline(ss, name, ',');
        std::getline(ss, date, ',');

        // Tạo email & password mặc định dựa trên id
        std::string email    = id + "@email.com";
        std::string password = "password123";

        users.push_back(User(trim(id), trim(name), trim(email), password, trim(date)));
    }
    file.close();
    return true;
}

// --- Đọc dữ liệu Items ---
// Format thực tế: item_id,name,category,price
// (bổ sung badge, star, sold, image_url mặc định)
bool CSVLoader::loadItems(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Loi: Khong the mo file " << filepath << std::endl;
        return false;
    }

    std::string line;
    std::getline(file, line); // Bỏ qua dòng tiêu đề

    while (std::getline(file, line)) {
        if (line.empty()) continue;

        std::stringstream ss(line);
        std::string id, name, category, price_str;

        std::getline(ss, id, ',');
        std::getline(ss, name, ',');
        std::getline(ss, category, ',');
        std::getline(ss, price_str, ',');

        double price = price_str.empty() ? 0.0 : std::stod(trim(price_str));

        // Gán badge dựa trên category và price (mô phỏng)
        std::string badge = "";
        if (price >= 10000000) badge = "Hot";
        else if (category == "Books") badge = "Sale";

        // star, sold, image_url mặc định
        double star = 4.0;
        int sold = 100;
        std::string img = category + "/" + id + ".jpg";

        items.push_back(Item(trim(id), trim(name), trim(category), badge,
                             price, star, sold, img));
    }
    file.close();
    return true;
}

// --- Đọc dữ liệu Interactions ---
bool CSVLoader::loadInteractions(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Loi: Khong the mo file " << filepath << std::endl;
        return false;
    }

    std::string line;
    std::getline(file, line); // Bỏ qua dòng tiêu đề

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

<<<<<<< HEAD
// --- Getters ---
const MyVector<User>& CSVLoader::getUsers() const { return users; }
const MyVector<Item>& CSVLoader::getItems() const { return items; }
const MyVector<Interaction>& CSVLoader::getInteractions() const { return interactions; }
=======
// ─────────────────────────────────────────────────────────────────────────────
// Public Methods - Save (memory -> CSV)
// ─────────────────────────────────────────────────────────────────────────────

bool CSVLoader::saveUsers(const std::string& filepath,
                          const std::vector<User>& users) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "[CSVLoader][ERROR] Khong the ghi file: " << filepath << "\n";
        return false;
    }

    file << "user_id,name,created_at\n";
    for (const auto& u : users) {
        file << escapeField(u.user_id) << ","
             << escapeField(u.name) << ","
             << escapeField(u.created_at) << "\n";
    }

    std::cout << "[CSVLoader] Da ghi " << users.size()
              << " nguoi dung ra '" << filepath << "'\n";
    return true;
}

bool CSVLoader::saveItems(const std::string& filepath,
                         const std::vector<Item>& items) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "[CSVLoader][ERROR] Khong the ghi file: " << filepath << "\n";
        return false;
    }

    file << "item_id,name,category,price\n";
    for (const auto& it : items) {
        file << escapeField(it.item_id) << ","
             << escapeField(it.name) << ","
             << escapeField(it.category) << ","
             << std::fixed << std::setprecision(0) << it.price << "\n";
    }

    std::cout << "[CSVLoader] Da ghi " << items.size()
              << " san pham ra '" << filepath << "'\n";
    return true;
}

bool CSVLoader::saveInteractions(const std::string& filepath,
                                 const std::vector<Interaction>& interactions) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "[CSVLoader][ERROR] Khong the ghi file: " << filepath << "\n";
        return false;
    }

    file << "user_id,item_id,click,add_cart,purchase,rating\n";
    for (const auto& i : interactions) {
        file << escapeField(i.user_id) << ","
             << escapeField(i.item_id) << ","
             << i.click << ","
             << i.add_cart << ","
             << i.purchase << ","
             << std::fixed << std::setprecision(1) << i.rating << "\n";
    }

    std::cout << "[CSVLoader] Da ghi " << interactions.size()
              << " tuong tac ra '" << filepath << "'\n";
    return true;
}
>>>>>>> b8c1ae64c94847b04dee00ae1cc482c9f316487e
