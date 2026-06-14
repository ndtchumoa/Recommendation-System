#include "SimilarityMatrix.h"
#include <cmath>
#include <iostream>
#include <iomanip>

SimilarityMatrix::SimilarityMatrix() {}

// --- Thuật toán cốt lõi: Tính Cosine Similarity ---
double SimilarityMatrix::calculateCosine(const MyMap<std::string, double>& itemA_users,
                                          const MyMap<std::string, double>& itemB_users) const {
    double dotProduct = 0.0;
    double normA = 0.0;
    double normB = 0.0;

    // 1. Tính độ dài vector A (Mẫu số)
    MyVector<std::string> keysA = itemA_users.keys();
    for (int i = 0; i < keysA.size(); ++i) {
        double val;
        itemA_users.find(keysA[i], val);
        normA += val * val;
    }

    // 2. Tính độ dài vector B (Mẫu số)
    MyVector<std::string> keysB = itemB_users.keys();
    for (int i = 0; i < keysB.size(); ++i) {
        double val;
        itemB_users.find(keysB[i], val);
        normB += val * val;
    }

    // Nếu một trong 2 vector bằng 0 thì độ tương đồng = 0 (tránh chia cho 0)
    if (normA == 0.0 || normB == 0.0) return 0.0;

    // 3. Tính tích vô hướng (Tử số)
    // Mẹo tối ưu: Chỉ duyệt qua map có kích thước nhỏ hơn để tăng tốc
    const MyMap<std::string, double>* smallerMap = &itemA_users;
    const MyMap<std::string, double>* largerMap = &itemB_users;

    if (itemA_users.size() > itemB_users.size()) {
        smallerMap = &itemB_users;
        largerMap = &itemA_users;
    }

    // Chỉ nhân điểm của những User đã tương tác với CẢ HAI item
    MyVector<std::string> smallerKeys = smallerMap->keys();
    for (int i = 0; i < smallerKeys.size(); ++i) {
        std::string user_id = smallerKeys[i];
        double scoreA;
        smallerMap->find(user_id, scoreA);

        // Tìm xem User này có tương tác với item bên kia không?
        double scoreB;
        if (largerMap->find(user_id, scoreB)) {
            dotProduct += (scoreA * scoreB); // R_{u,i} * R_{u,j}
        }
    }

    // 4. Trả về kết quả Cosine
    return dotProduct / (std::sqrt(normA) * std::sqrt(normB));
}

// --- Xây dựng Ma trận Tương đồng ---
void SimilarityMatrix::build(const RatingMatrix& ratingMatrix) {
    const MyMap<std::string, MyMap<std::string, double>>& originalMatrix = ratingMatrix.getMatrix();

    // Bước 1: Đảo ma trận (Transpose) từ User-Item sang Item-User
    // itemUserMap[item_id][user_id] = score
    MyMap<std::string, MyMap<std::string, double>> itemUserMap;

    MyVector<std::string> userIDs = originalMatrix.keys();
    for (int u = 0; u < userIDs.size(); ++u) {
        std::string u_id = userIDs[u];
        MyMap<std::string, double> userRow;
        if (!originalMatrix.find(u_id, userRow)) continue;

        MyVector<std::string> itemIDs = userRow.keys();
        for (int k = 0; k < itemIDs.size(); ++k) {
            std::string i_id = itemIDs[k];
            double score;
            userRow.find(i_id, score);
            itemUserMap[i_id][u_id] = score; // Đẩy dữ liệu vào ma trận đảo
        }
    }

    // Bước 2: Gom tất cả các Item ID lại thành một mảng để dễ duyệt cặp
    MyVector<std::string> item_ids = itemUserMap.keys();

    // Bước 3: Duyệt qua mọi cặp Item (A và B) để tính Cosine
    int n = item_ids.size();
    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) { // Bắt đầu từ i+1 để không tính trùng lặp
            std::string itemA = item_ids[i];
            std::string itemB = item_ids[j];

            MyMap<std::string, double> mapA, mapB;
            itemUserMap.find(itemA, mapA);
            itemUserMap.find(itemB, mapB);

            double sim = calculateCosine(mapA, mapB);

            // Chỉ lưu vào bộ nhớ nếu có độ tương đồng (> 0 hoặc < 0)
            if (sim != 0.0) {
                simMatrix[itemA][itemB] = sim;
                simMatrix[itemB][itemA] = sim;
            }
        }
    }
}
// --- Lấy Top K Item tương đồng ---
MyVector<MyPair<std::string, double>> SimilarityMatrix::getTopKSimilarItems(const std::string& item_id, int k) const {
    MyVector<MyPair<std::string, double>> result;

    MyMap<std::string, double> neighbors;
    if (!simMatrix.find(item_id, neighbors)) {
        return result; // Không tìm thấy hoặc sản phẩm này không giống ai
    }

    // Đổ dữ liệu từ map sang vector để chuẩn bị sort
    MyVector<std::string> keys = neighbors.keys();
    for (int i = 0; i < keys.size(); ++i) {
        double score;
        neighbors.find(keys[i], score);
        result.push_back(MyPair<std::string, double>(keys[i], score));
    }

    // --- SẮP XẾP BUBBLE SORT TRỰC TIẾP ---
    for (int i = 0; i < result.size() - 1; ++i) {
        for (int j = 0; j < result.size() - i - 1; ++j) {
            if (result[j].second < result[j + 1].second) { // Giảm dần
                MyPair<std::string, double> temp = result[j];
                result[j] = result[j + 1];
                result[j + 1] = temp;
            }
        }
    }

    // Cắt mảng lấy đúng số lượng K
    if (result.size() > k) {
        MyVector<MyPair<std::string, double>> truncated;
        for (int i = 0; i < k; ++i) {
            truncated.push_back(result[i]);
        }
        result = truncated;
    }

    return result;
}

// --- topK (alias cho Recommender) ---
MyVector<MyPair<std::string, double>> SimilarityMatrix::topK(const std::string& item_id, int k) const {
    return getTopKSimilarItems(item_id, k);
}

// --- In ma trận ra màn hình ---
void SimilarityMatrix::printMatrix() const {
    std::cout << "\n--- ITEM SIMILARITY MATRIX ---\n";

    MyVector<std::string> rows = simMatrix.keys();
    for (int r = 0; r < rows.size(); ++r) {
        std::string itemA = rows[r];
        std::cout << "Item [" << itemA << "] giong voi: ";

        MyMap<std::string, double> rowData;
        if (simMatrix.find(itemA, rowData)) {
            MyVector<std::string> cols = rowData.keys();
            for (int c = 0; c < cols.size(); ++c) {
                double score;
                rowData.find(cols[c], score);
                std::cout << "{" << cols[c] << ": "
                          << std::fixed << std::setprecision(3) << score << "} ";
            }
        }
        std::cout << "\n";
    }
    std::cout << "------------------------------\n";
}