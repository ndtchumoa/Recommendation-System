#include "CSVLoader.h"

CSVLoader::CSVLoader() {}

// Hàm hỗ trợ xóa khoảng trắng ở đầu và cuối chuỗi
std::string CSVLoader::trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (std::string::npos == first) return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}

// --- Đọc dữ liệu Users ---
// Format thực tế: user_id,name,created_at
// (bổ sung email, password mặc định cho các tính năng Auth mới)
bool CSVLoader::loadUsers(const std::string& filepath) {
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

// --- Getters ---
const MyVector<User>& CSVLoader::getUsers() const { return users; }
const MyVector<Item>& CSVLoader::getItems() const { return items; }
const MyVector<Interaction>& CSVLoader::getInteractions() const { return interactions; }