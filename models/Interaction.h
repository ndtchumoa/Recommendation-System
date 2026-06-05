#pragma once
#include <string>

/**
 * @struct Interaction
 * @brief Ghi lại toàn bộ hành vi của một người dùng với một sản phẩm.
 *
 * Thay vì chỉ lưu điểm đánh giá (rating), struct này theo dõi nhiều loại
 * hành vi khác nhau. Hàm computeScore() tổng hợp các hành vi thành một
 * điểm tương tác duy nhất (score) theo công thức có trọng số.
 *
 * Dữ liệu được nạp từ tệp datasets/interactions.csv.
 *
 * ─── Bảng Trọng Số (Interaction Weights) ────────────────────────────
 *   Hành vi       | Trọng số | Lý do
 *   ------------- | -------- | ----------------------------------------
 *   click         |   0.5    | Thể hiện sự quan tâm nhẹ, chi phí thấp
 *   add_cart      |   1.5    | Ý định mua cao hơn, nhưng chưa cam kết
 *   purchase      |   4.0    | Hành vi cam kết mạnh nhất
 *   rating (1-5)  |   1.0×r  | Phản hồi chủ động, scaled theo mức sao
 * ─────────────────────────────────────────────────────────────────────
 *
 * Công thức:
 *   score = click×0.5 + add_cart×1.5 + purchase×4.0 + rating×1.0
 */
struct Interaction {
    std::string user_id;  // Mã người dùng
    std::string item_id;  // Mã sản phẩm

    int    click;     // Số lần click/xem sản phẩm
    int    add_cart;  // Số lần thêm vào giỏ hàng
    int    purchase;  // Số lần mua (thường là 0 hoặc 1)
    double rating;    // Số sao đánh giá (0.0 nếu chưa đánh giá, 1.0–5.0)

    // ── Trọng số hành vi (có thể điều chỉnh tập trung ở đây) ──────────
    static constexpr double W_CLICK    = 0.5;
    static constexpr double W_ADD_CART = 1.5;
    static constexpr double W_PURCHASE = 4.0;
    static constexpr double W_RATING   = 1.0; // nhân thêm với số sao thực tế

    // Constructor mặc định
    Interaction() = default;

    // Constructor khởi tạo đầy đủ
    Interaction(const std::string& user_id,
                const std::string& item_id,
                int click,
                int add_cart,
                int purchase,
                double rating);

    /**
     * @brief Tính điểm tương tác tổng hợp từ các hành vi đa dạng.
     * @return Điểm số dương phản ánh mức độ quan tâm/yêu thích.
     *
     * Ví dụ: click=3, add_cart=1, purchase=1, rating=5
     *   → score = 3×0.5 + 1×1.5 + 1×4.0 + 5×1.0
     *           = 1.5   + 1.5   + 4.0   + 5.0  = 12.0
     */
    double computeScore() const;

    // In thông tin tương tác ra console (dùng để debug)
    void print() const;
};
