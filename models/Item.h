#pragma once
#include <string>

/**
 * @struct Item
 * @brief Đại diện cho một sản phẩm trong hệ thống.
 *
 * Lưu trữ thông tin định danh và phân loại sản phẩm.
 * Dữ liệu được nạp từ tệp datasets/items.csv.
 */
struct Item {
    std::string item_id;  // Mã định danh duy nhất (vd: "I001")
    std::string name;     // Tên sản phẩm
    std::string category; // Danh mục (Electronics, Books, Apparel, ...)
    double      price;    // Giá niêm yết (VND)

    // Constructor mặc định
    Item() = default;

    // Constructor khởi tạo đầy đủ
    Item(const std::string& id,
         const std::string& name,
         const std::string& category,
         double price);

    // In thông tin sản phẩm ra console (dùng để debug)
    void print() const;
};
