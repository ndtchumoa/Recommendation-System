#include "CSVLoader.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>
#include <algorithm>

// ─────────────────────────────────────────────────────────────────────────────
// Private Helpers
// ─────────────────────────────────────────────────────────────────────────────

std::string CSVLoader::trim(const std::string& s) const {
    // Tìm vị trí ký tự đầu tiên không phải whitespace
    size_t start = s.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";

    // Tìm vị trí ký tự cuối cùng không phải whitespace
    size_t end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

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

// ─────────────────────────────────────────────────────────────────────────────
// Public Methods
// ─────────────────────────────────────────────────────────────────────────────

std::vector<User> CSVLoader::loadUsers(const std::string& filepath) const {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw std::runtime_error("[CSVLoader] Khong mo duoc file: " + filepath);
    }

    std::vector<User> users;
    std::string line;

    // Bỏ qua dòng header (dòng đầu tiên)
    std::getline(file, line);

    int lineNum = 1;
    while (std::getline(file, line)) {
        ++lineNum;
        line = trim(line);
        if (line.empty()) continue; // bỏ qua dòng trống

        std::vector<std::string> cols = parseLine(line);

        // Kiểm tra số lượng cột tối thiểu: user_id, name, created_at
        if (cols.size() < 3) {
            std::cerr << "[CSVLoader][WARN] users.csv dong " << lineNum
                      << ": thieu cot, bo qua.\n";
            continue;
        }

        users.emplace_back(
            cols[0], // user_id
            cols[1], // name
            cols[2]  // created_at
        );
    }

    std::cout << "[CSVLoader] Da nap " << users.size()
              << " nguoi dung tu '" << filepath << "'\n";
    return users;
}

// ─────────────────────────────────────────────────────────────────────────────

std::vector<Item> CSVLoader::loadItems(const std::string& filepath) const {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw std::runtime_error("[CSVLoader] Khong mo duoc file: " + filepath);
    }

    std::vector<Item> items;
    std::string line;

    // Bỏ qua dòng header
    std::getline(file, line);

    int lineNum = 1;
    while (std::getline(file, line)) {
        ++lineNum;
        line = trim(line);
        if (line.empty()) continue;

        std::vector<std::string> cols = parseLine(line);

        // Kiểm tra số lượng cột tối thiểu: item_id, name, category, price
        if (cols.size() < 4) {
            std::cerr << "[CSVLoader][WARN] items.csv dong " << lineNum
                      << ": thieu cot, bo qua.\n";
            continue;
        }

        double price = 0.0;
        try {
            price = std::stod(cols[3]);
        } catch (const std::exception&) {
            std::cerr << "[CSVLoader][WARN] items.csv dong " << lineNum
                      << ": gia khong hop le ('" << cols[3] << "'), dat = 0.\n";
        }

        items.emplace_back(
            cols[0], // item_id
            cols[1], // name
            cols[2], // category
            price    // price
        );
    }

    std::cout << "[CSVLoader] Da nap " << items.size()
              << " san pham tu '" << filepath << "'\n";
    return items;
}

// ─────────────────────────────────────────────────────────────────────────────

std::vector<Interaction> CSVLoader::loadInteractions(const std::string& filepath) const {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw std::runtime_error("[CSVLoader] Khong mo duoc file: " + filepath);
    }

    std::vector<Interaction> interactions;
    std::string line;

    // Bỏ qua dòng header
    std::getline(file, line);

    int lineNum = 1;
    while (std::getline(file, line)) {
        ++lineNum;
        line = trim(line);
        if (line.empty()) continue;

        std::vector<std::string> cols = parseLine(line);

        // Kiểm tra số lượng cột tối thiểu:
        // user_id, item_id, click, add_cart, purchase, rating
        if (cols.size() < 6) {
            std::cerr << "[CSVLoader][WARN] interactions.csv dong " << lineNum
                      << ": thieu cot, bo qua.\n";
            continue;
        }

        // ── Parse từng cột số, xử lý lỗi nhẹ nhàng ──────────────────
        int click = 0, add_cart = 0, purchase = 0;
        double rating = 0.0;

        try { click    = std::stoi(cols[2]); } catch (...) {
            std::cerr << "[CSVLoader][WARN] interactions.csv dong " << lineNum
                      << ": click khong hop le, dat = 0.\n";
        }
        try { add_cart = std::stoi(cols[3]); } catch (...) {
            std::cerr << "[CSVLoader][WARN] interactions.csv dong " << lineNum
                      << ": add_cart khong hop le, dat = 0.\n";
        }
        try { purchase = std::stoi(cols[4]); } catch (...) {
            std::cerr << "[CSVLoader][WARN] interactions.csv dong " << lineNum
                      << ": purchase khong hop le, dat = 0.\n";
        }
        try { rating   = std::stod(cols[5]); } catch (...) {
            std::cerr << "[CSVLoader][WARN] interactions.csv dong " << lineNum
                      << ": rating khong hop le, dat = 0.\n";
        }

        // ── Validate khoảng giá trị hợp lệ ───────────────────────────
        if (click    < 0) click    = 0;
        if (add_cart < 0) add_cart = 0;
        if (purchase < 0) purchase = 0;
        if (rating   < 0.0 || rating > 5.0) {
            std::cerr << "[CSVLoader][WARN] interactions.csv dong " << lineNum
                      << ": rating=" << rating << " ngoai khoang [0,5], dat = 0.\n";
            rating = 0.0;
        }

        interactions.emplace_back(
            cols[0],   // user_id
            cols[1],   // item_id
            click,
            add_cart,
            purchase,
            rating
        );
    }

    std::cout << "[CSVLoader] Da nap " << interactions.size()
              << " tuong tac tu '" << filepath << "'\n";
    return interactions;
}
