#ifndef ITEM_H
#define ITEM_H

#include <string>
#include <iostream>

/*
 * Cấu trúc sản phẩm (Product) mở rộng:
 * - item_id:   Mã sản phẩm (VD: I001)
 * - name:      Tên sản phẩm
 * - category:  Danh mục (VD: Laptop, Book, Accessory, Monitor)
 * - badge:     Nhãn (VD: "Hot", "Sale", "New", "")
 * - price:     Giá sản phẩm (VND)
 * - star:      Đánh giá trung bình (0.0 - 5.0)
 * - sold:      Số lượng đã bán
 * - image_url: Đường dẫn ảnh (tùy chọn)
 */
class Item {
public:
    std::string item_id;
    std::string name;
    std::string category;
    std::string badge;
    double price;
    double star;
    int sold;
    std::string image_url;

public:
    // 1. Khởi tạo
    Item();
    Item(std::string id, std::string n, std::string cat, std::string bdg,
         double p, double st, int s, std::string img = "");

    // 2. Getters
    std::string getItemId() const;
    std::string getName() const;
    std::string getCategory() const;
    std::string getBadge() const;
    double getPrice() const;
    double getStar() const;
    int getSold() const;
    std::string getImageUrl() const;

    // 3. Setters
    void setName(const std::string& n);
    void setCategory(const std::string& cat);
    void setBadge(const std::string& bdg);
    void setPrice(double p);
    void setStar(double st);
    void setSold(int s);
    void setImageUrl(const std::string& img);

    // 4. Xuất thông tin
    void displayInfo() const;
};

#endif // ITEM_H