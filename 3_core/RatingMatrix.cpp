#include "../1_models/Interaction.h"
#include <iostream>
#include <iomanip>

void RatingMatrix::build(const std::vector<Interaction>& interactions) {
    // Bước 1: thu thập user_id và item_id duy nhất (giữ thứ tự xuất hiện)
    for (const auto& it : interactions) {
        if (userIdx_.find(it.user_id) == userIdx_.end()) {
            userIdx_[it.user_id] = (int)users_.size();
            users_.push_back(it.user_id);
        }
        if (itemIdx_.find(it.item_id) == itemIdx_.end()) {
            itemIdx_[it.item_id] = (int)items_.size();
            items_.push_back(it.item_id);
        }
    }

    // Bước 2: khởi tạo ma trận toàn 0
    data_.assign(users_.size(),
                 std::vector<double>(items_.size(), 0.0));

    // Bước 3: cộng dồn score vào ô tương ứng
    for (const auto& it : interactions) {
        int r = userIdx_.at(it.user_id);
        int c = itemIdx_.at(it.item_id);
        data_[r][c] += it.computeScore();
    }
}

double RatingMatrix::get(const std::string& uid,
                         const std::string& iid) const {
    auto ur = userIdx_.find(uid);
    auto ir = itemIdx_.find(iid);
    if (ur == userIdx_.end() || ir == itemIdx_.end()) return 0.0;
    return data_[ur->second][ir->second];
}

double RatingMatrix::getByIndex(int r, int c) const {
    return data_[r][c];
}

int RatingMatrix::numUsers() const { return (int)users_.size(); }
int RatingMatrix::numItems() const { return (int)items_.size(); }

const std::vector<std::string>& RatingMatrix::users() const { return users_; }
const std::vector<std::string>& RatingMatrix::items() const { return items_; }

std::vector<double> RatingMatrix::itemColumn(int c) const {
    std::vector<double> col(data_.size());
    for (int r = 0; r < (int)data_.size(); ++r)
        col[r] = data_[r][c];
    return col;
}

bool RatingMatrix::isNewUser(const std::string& uid) const {
    auto it = userIdx_.find(uid);
    if (it == userIdx_.end()) return true;
    for (double v : data_[it->second])
        if (v > 0.0) return false;
    return true;
}

double RatingMatrix::itemTotalScore(const std::string& iid) const {
    auto it = itemIdx_.find(iid);
    if (it == itemIdx_.end()) return 0.0;
    int c = it->second;
    double sum = 0.0;
    for (int r = 0; r < (int)data_.size(); ++r)
        sum += data_[r][c];
    return sum;
}

void RatingMatrix::print() const {
    const int W = 7;
    std::cout << "\n=== Rating Matrix ("
              << numUsers() << " users x " << numItems() << " items) ===\n";
    std::cout << std::setw(6) << "";
    for (const auto& iid : items_)
        std::cout << std::setw(W) << iid;
    std::cout << "\n";
    for (int r = 0; r < numUsers(); ++r) {
        std::cout << std::setw(6) << users_[r];
        for (int c = 0; c < numItems(); ++c)
            std::cout << std::setw(W) << std::fixed
                      << std::setprecision(1) << data_[r][c];
        std::cout << "\n";
    }
    std::cout << "\n";
}