#pragma once
#include "RatingMatrix.h"
#include "SimilarityMatrix.h"
#include <vector>
#include <utility>

// Kết quả gợi ý: (item_id, predicted_score)
using RecommendResult = std::vector<std::pair<int, double>>;

class Recommender {
public:
    Recommender(const RatingMatrix& rm, const SimilarityMatrix& sm)
        : rm_(rm), sm_(sm) {}

    // Gợi ý cho người dùng đã có lịch sử
    // scoreThreshold: chỉ dùng các item user đã tương tác có score >= ngưỡng
    // topN: số sản phẩm trả về
    RecommendResult forUser(int user_id,
                            int topN = 10,
                            double scoreThreshold = 0.0) const;

    // Gợi ý Cold-Start: sản phẩm phổ biến nhất toàn hệ thống
    RecommendResult coldStart(int topN = 10) const;

private:
    const RatingMatrix&    rm_;
    const SimilarityMatrix& sm_;
};