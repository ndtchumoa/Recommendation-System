#pragma once
#include <vector>
#include <string>
#include <unordered_map>

#include "../1_models/Interaction.h"

// ═══════════════════════════════════════════════════════════════════════
// RatingMatrix  —  Ma trận user × item
//
//         I001   I002   I003  ...
// U001  [ 12.5    8.0    2.5  ...]
// U002  [  7.0    0.0   11.0  ...]
//
// Mỗi ô [r][c] = tổng computeScore() của tất cả Interaction
// có cùng user và item.  Ô = 0.0 nếu chưa tương tác.
// ═══════════════════════════════════════════════════════════════════════
class RatingMatrix {
public:
    // Xây dựng ma trận từ danh sách interactions
    void build(const std::vector<Interaction>& interactions);

    // Truy xuất điểm (trả 0.0 nếu không tìm thấy)
    double get(const std::string& user_id, const std::string& item_id) const;
    double getByIndex(int r, int c) const;

    // Kích thước
    int numUsers() const;
    int numItems() const;

    // Danh sách user_id / item_id theo thứ tự chỉ số
    const std::vector<std::string>& users() const;
    const std::vector<std::string>& items() const;

    // Lấy cột item c (vector score theo trục user) — dùng tính cosine
    std::vector<double> itemColumn(int c) const;

    // Kiểm tra user mới: tất cả ô trong hàng = 0
    bool isNewUser(const std::string& user_id) const;

    // Tổng score của một item qua tất cả user — dùng cho cold-start
    double itemTotalScore(const std::string& item_id) const;

    // In ma trận ra console (debug)
    void print() const;

private:
    std::vector<std::string>            users_, items_;
    std::unordered_map<std::string,int> userIdx_, itemIdx_;
    std::vector<std::vector<double>>    data_;  // data_[r][c]
};