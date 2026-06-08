#pragma once
#include <vector>
#include <string>
#include <utility>

#include "RatingMatrix.h"
#include "SimilarityMatrix.h"
#include "../1_models/Item.h"

// Kết quả gợi ý: (item_id, predicted_score)
using RecommendResult = std::vector<std::pair<std::string, double>>;

// ═══════════════════════════════════════════════════════════════════════
// Recommender  —  Engine gợi ý sản phẩm
//
// Chiến lược tự động:
//   • Người dùng CŨ  → Item-Based CF
//       predicted[j] = Σ( sim(i,j) × score(u,i) ) / Σ|sim(i,j)|
//   • Người dùng MỚI → Cold Start
//       Gợi ý top-K sản phẩm có tổng score cao nhất toàn hệ thống
// ═══════════════════════════════════════════════════════════════════════
class Recommender {
public:
    Recommender(const RatingMatrix&     rm,
                const SimilarityMatrix& sm,
                const std::vector<Item>& items);

    // Gợi ý cho người dùng đã có lịch sử
    // Tự động chuyển sang coldStart nếu user chưa có interaction
    RecommendResult forUser(const std::string& user_id, int topN = 10) const;

    // Gợi ý Cold-Start: top-K sản phẩm phổ biến nhất
    RecommendResult coldStart(int topN = 10) const;

    // In kết quả ra console (có tên sản phẩm, rank, score)
    void printResult(const std::string& label,
                     const RecommendResult& result) const;

private:
    const RatingMatrix&     rm_;
    const SimilarityMatrix& sm_;
    const std::vector<Item>& items_;
};