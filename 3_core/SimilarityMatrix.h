#pragma once
#include <vector>
#include <string>
#include <unordered_map>

#include "RatingMatrix.h"

// ═══════════════════════════════════════════════════════════════════════
// SimilarityMatrix  —  Ma trận độ tương đồng item × item
//
//         I001   I002   I003
// I001  [  1.0    0.9    0.5 ]
// I002  [  0.9    1.0    0.6 ]
// I003  [  0.5    0.6    1.0 ]
//
// Dùng Cosine Similarity tính trên các cột của RatingMatrix.
// Ma trận đối xứng: sim[i][j] == sim[j][i].
// ═══════════════════════════════════════════════════════════════════════
class SimilarityMatrix {
public:
    // Xây dựng từ RatingMatrix
    void build(const RatingMatrix& rm);

    // Lấy độ tương đồng giữa 2 item (0.0 nếu không tìm thấy)
    double get(const std::string& item_a, const std::string& item_b) const;

    // Top-K item tương tự nhất với item_id (loại chính nó)
    // Trả về vector<pair<item_id, similarity>> sắp xếp giảm dần
    std::vector<std::pair<std::string,double>>
    topK(const std::string& item_id, int k) const;

    // In ma trận ra console (debug)
    void print() const;

private:
    std::vector<std::string>            items_;
    std::unordered_map<std::string,int> itemIdx_;
    std::vector<std::vector<double>>    sim_;   // sim_[i][j]

    // Tính cosine similarity giữa 2 vector
    static double cosine(const std::vector<double>& a,
                         const std::vector<double>& b);
};