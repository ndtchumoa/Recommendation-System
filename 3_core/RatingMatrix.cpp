#include "RatingMatrix.h"
#include "../1_models/Interaction.h"
#include <iostream>
#include <iomanip>
#include <cmath>

RatingMatrix::RatingMatrix() {}

// --- Xây dựng ma trận thưa từ danh sách Interactions ---
void RatingMatrix::buildMatrix(const MyVector<Interaction>& interactions) {
    for (int i = 0; i < interactions.size(); ++i) {
        std::string u_id = interactions[i].getUserId();
        std::string i_id = interactions[i].getItemId();

        // Gọi computeScore() từ lớp Interaction để lấy điểm tổng hợp
        double score = interactions[i].computeScore();

        // Gán vào ma trận (Tự động tạo mới nếu u_id hoặc i_id chưa tồn tại)
        matrix[u_id][i_id] = score;
    }
}

// --- Logic Chuẩn hóa (Mean-Centering) ---
void RatingMatrix::normalizeMatrix() {
    // Lấy danh sách user_id
    MyVector<std::string> userIDs = matrix.keys();

    for (int u = 0; u < userIDs.size(); ++u) {
        std::string user_id = userIDs[u];

        // Lấy danh sách item_id và score của user này
        MyMap<std::string, double>& item_scores = matrix[user_id];

        if (item_scores.size() == 0) continue;

        // Bước 1: Tính tổng điểm các item mà user NÀY đã tương tác thực tế
        double sum = 0.0;
        MyVector<std::string> itemIDs = item_scores.keys();
        for (int k = 0; k < itemIDs.size(); ++k) {
            sum += item_scores[itemIDs[k]];
        }

        // Bước 2: Tính điểm trung bình (Mean)
        double mean = sum / item_scores.size();
        user_means[user_id] = mean; // Lưu lại

        // Bước 3: Lấy giá trị thực tế trừ đi trung bình
        for (int k = 0; k < itemIDs.size(); ++k) {
            item_scores[itemIDs[k]] -= mean;
        }
    }
}

// --- Getters ---
const MyMap<std::string, MyMap<std::string, double>>& RatingMatrix::getMatrix() const {
    return matrix;
}

// --- Các phương thức hỗ trợ cho Recommender ---

// Kiểm tra user có phải là người dùng mới (chưa có lịch sử tương tác) không
bool RatingMatrix::isNewUser(const std::string& user_id) const {
    // Kiểm tra xem user_id có tồn tại trong ma trận không
    // MyMap::find trả về true và lưu value vào out nếu tìm thấy
    MyMap<std::string, double> dummy;
    return !matrix.find(user_id, dummy);
}

// Lấy điểm số của một user-item cụ thể
double RatingMatrix::get(const std::string& user_id, const std::string& item_id) const {
    MyMap<std::string, double> userRow;
    if (!matrix.find(user_id, userRow)) {
        return 0.0; // User không tồn tại
    }
    double score;
    if (userRow.find(item_id, score)) {
        return score;
    }
    return 0.0; // Item không tồn tại trong hàng của user
}

// Lấy danh sách tất cả item_id trong ma trận
MyVector<std::string> RatingMatrix::items() const {
    // Dùng một MyMap để lưu item_id duy nhất (tránh trùng lặp)
    MyMap<std::string, bool> seen;

    MyVector<std::string> userIDs = matrix.keys();
    for (int u = 0; u < userIDs.size(); ++u) {
        MyMap<std::string, double> userRow;
        if (matrix.find(userIDs[u], userRow)) {
            MyVector<std::string> itemIDs = userRow.keys();
            for (int k = 0; k < itemIDs.size(); ++k) {
                seen[itemIDs[k]] = true; // Có thể dùng toán tử []
            }
        }
    }

    return seen.keys();
}

// Tính tổng điểm của một item trên toàn bộ user
double RatingMatrix::itemTotalScore(const std::string& item_id) const {
    double total = 0.0;

    MyVector<std::string> userIDs = matrix.keys();
    for (int u = 0; u < userIDs.size(); ++u) {
        MyMap<std::string, double> userRow;
        if (matrix.find(userIDs[u], userRow)) {
            double score;
            if (userRow.find(item_id, score)) {
                total += score;
            }
        }
    }
    return total;
}

// --- Xuất ma trận để kiểm tra ---
void RatingMatrix::printMatrix() const {
    std::cout << "\n--- RATING MATRIX (Da chuan hoa) ---\n";

    MyVector<std::string> userIDs = matrix.keys();
    for (int u = 0; u < userIDs.size(); ++u) {
        std::string uid = userIDs[u];
        std::cout << "User [" << uid << "]: ";

        MyMap<std::string, double> userRow;
        if (matrix.find(uid, userRow)) {
            MyVector<std::string> itemIDs = userRow.keys();
            for (int k = 0; k < itemIDs.size(); ++k) {
                double score;
                userRow.find(itemIDs[k], score);
                std::cout << "{" << itemIDs[k] << ": "
                          << std::fixed << std::setprecision(2) << score << "} ";
            }
        }
        std::cout << "\n";
    }
    std::cout << "------------------------------------\n";
}