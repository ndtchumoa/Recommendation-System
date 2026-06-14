#ifndef CSV_LOADER_H
#define CSV_LOADER_H

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

// Nhúng các cấu trúc dữ liệu tự cài đặt (thay thế STL)
#include "MyDataStructures.h"

// Nhúng các thực thể đã tạo từ tầng Models
#include "../1_models/User.h"
#include "../1_models/Item.h"
#include "../1_models/Interaction.h"

class CSVLoader {
private:
    MyVector<User> users;
    MyVector<Item> items;
    MyVector<Interaction> interactions;

    // Hàm hỗ trợ để cắt khoảng trắng thừa (trim) nếu dữ liệu CSV bị lỗi format
    std::string trim(const std::string& str);

public:
    CSVLoader();

    // 1. Các hàm đọc dữ liệu từ file
    bool loadUsers(const std::string& filepath);
    bool loadItems(const std::string& filepath);
    bool loadInteractions(const std::string& filepath);

    // 2. Getters — trả về tham chiếu hằng để tránh sao chép
    const MyVector<User>& getUsers() const;
    const MyVector<Item>& getItems() const;
    const MyVector<Interaction>& getInteractions() const;
};

#endif // CSV_LOADER_H