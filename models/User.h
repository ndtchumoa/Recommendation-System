#pragma once
#include <string>

/**
 * @struct User
 * @brief Đại diện cho một tài khoản người dùng trong hệ thống.
 *
 * Lưu trữ thông tin định danh cơ bản của người dùng.
 * Dữ liệu được nạp từ tệp datasets/users.csv.
 */
struct User {
    std::string user_id;    // Mã định danh duy nhất (vd: "U001")
    std::string name;       // Tên hiển thị của người dùng
    std::string created_at; // Ngày tạo tài khoản (định dạng YYYY-MM-DD)

    // Constructor mặc định
    User() = default;

    // Constructor khởi tạo đầy đủ
    User(const std::string& id,
         const std::string& name,
         const std::string& created_at);

    // In thông tin người dùng ra console (dùng để debug)
    void print() const;
};
