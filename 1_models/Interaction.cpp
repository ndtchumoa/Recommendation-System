#include "Interaction.h"

// --- Định nghĩa các trọng số hành vi (Weights) ---
// Bạn có thể tùy chỉnh các giá trị này để thay đổi mức độ ảnh hưởng của hành vi lên điểm số.
const double Interaction::WEIGHT_CLICK    = 1.0;  // Click có trọng số thấp nhất
const double Interaction::WEIGHT_ADD_CART = 2.0;  // Quan tâm hơn mới thêm vào giỏ
const double Interaction::WEIGHT_PURCHASE = 5.0;  // Mua hàng thể hiện sự tin tưởng cao
const double Interaction::WEIGHT_RATING   = 3.0;  // Điểm đánh giá (Rating * Trọng số)

// --- Hàm khởi tạo (Constructors) ---

Interaction::Interaction() 
    : user_id(""), item_id(""), click_count(0), add_cart_count(0), purchase_count(0), rating(0.0) {}

Interaction::Interaction(std::string u_id, std::string i_id, int clicks, int carts, int purchases, double rate)
    : user_id(u_id), item_id(i_id), click_count(clicks), add_cart_count(carts), purchase_count(purchases), rating(rate) {}

// --- Getters ---

std::string Interaction::getUserId() const { return user_id; }
std::string Interaction::getItemId() const { return item_id; }
int Interaction::getClickCount() const { return click_count; }
int Interaction::getAddCartCount() const { return add_cart_count; }
int Interaction::getPurchaseCount() const { return purchase_count; }
double Interaction::getRating() const { return rating; }

// --- Setters ---

void Interaction::setClickCount(int clicks) { click_count = clicks; }
void Interaction::setAddCartCount(int carts) { add_cart_count = carts; }
void Interaction::setPurchaseCount(int purchases) { purchase_count = purchases; }
void Interaction::setRating(double rate) { rating = rate; }

// --- Hàm logic chính: Tính toán điểm số tổng hợp ---
double Interaction::computeScore() const {
    // Công thức tính điểm có trọng số:
    // Score = (Click * W_click) + (Cart * W_cart) + (Purchase * W_purchase) + (Rating * W_rating)
    double score = (click_count * WEIGHT_CLICK) + 
                   (add_cart_count * WEIGHT_ADD_CART) + 
                   (purchase_count * WEIGHT_PURCHASE) + 
                   (rating * WEIGHT_RATING);
    return score;
}

// --- Xuất thông tin tương tác ---
void Interaction::displayInfo() const {
    std::cout << "User: " << user_id 
              << " | Item: " << item_id 
              << " | Clicks: " << click_count 
              << " | Carts: " << add_cart_count 
              << " | Purchases: " << purchase_count 
              << " | Rating: " << rating 
              << " => Total Score: " << computeScore() << std::endl;
}