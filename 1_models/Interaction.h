#ifndef INTERACTION_H
#define INTERACTION_H

#include <string>
#include <iostream>

class Interaction {
public:
    std::string user_id;
    std::string item_id;
    int click_count;      // Số lần click xem sản phẩm
    int add_cart_count;   // Số lần thêm vào giỏ hàng
    int purchase_count;   // Số lần mua hàng
    double rating;        // Số sao đánh giá (ví dụ: từ 1.0 đến 5.0, mặc định là 0.0 nếu chưa đánh giá)

    // Các hệ số trọng số cố định để tính Score
    static const double WEIGHT_CLICK;
    static const double WEIGHT_ADD_CART;
    static const double WEIGHT_PURCHASE;
    static const double WEIGHT_RATING;

public:
    // 1. Khởi tạo
    Interaction();
    Interaction(std::string u_id, std::string i_id, int clicks = 0, int carts = 0, int purchases = 0, double rate = 0.0);

    // 2. Getters
    std::string getUserId() const;
    std::string getItemId() const;
    int getClickCount() const;
    int getAddCartCount() const;
    int getPurchaseCount() const;
    double getRating() const;

    // 3. Setters (Hỗ trợ cập nhật hành vi khi người dùng tương tác thêm)
    void setClickCount(int clicks);
    void setAddCartCount(int carts);
    void setPurchaseCount(int purchases);
    void setRating(double rate);

    // 4. Hàm cốt lõi: Tính toán điểm tương tác tổng hợp (Score)
    double computeScore() const;

    // 5. Xuất thông tin
    void displayInfo() const;
};
//bỏ giỏ hàng, thanh toán

#endif // INTERACTION_H