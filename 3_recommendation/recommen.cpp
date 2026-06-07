#include "Recommender.h"
#include <unordered_map>
#include <unordered_set>
#include <algorithm>

// ── forUser ───────────────────────────────────────────────────────────────────
RecommendResult Recommender::forUser(int user_id,
                                     int topN,
                                     double scoreThreshold) const
{
    // Lấy row của user (map item_id → score)
    const auto& mat = rm_.matrix();
    auto uit = mat.find(user_id);

    // Nếu user không có lịch sử → chuyển sang cold-start
    if (uit == mat.end() || uit->second.empty())
        return coldStart(topN);

    const auto& userRow = uit->second;

    // Tập item user đã tương tác (để loại khỏi gợi ý)
    std::unordered_set<int> seen;
    for (const auto& [iid, sc] : userRow) seen.insert(iid);

    // Tích lũy điểm dự đoán cho các item chưa tương tác
    // predicted[item_j] = Σ (sim(i,j) * score_i) / Σ |sim(i,j)|
    std::unordered_map<int, double> numerator, denominator;

    for (const auto& [iid, userScore] : userRow) {
        if (userScore < scoreThreshold) continue;
        for (const auto& [nbr, sim] : sm_.neighbors(iid)) {
            if (seen.count(nbr)) continue;   // đã tương tác → bỏ qua
            numerator[nbr]   += sim * userScore;
            denominator[nbr] += std::abs(sim);
        }
    }

    // Tính điểm cuối cùng
    RecommendResult result;
    result.reserve(numerator.size());
    for (const auto& [iid, num] : numerator) {
        double denom = denominator[iid];
        if (denom == 0.0) continue;
        result.emplace_back(iid, num / denom);
    }

    // Sắp xếp giảm dần
    std::sort(result.begin(), result.end(),
              [](const auto& a, const auto& b){ return a.second > b.second; });

    if ((int)result.size() > topN) result.resize(topN);
    return result;
}

// ── coldStart ─────────────────────────────────────────────────────────────────
RecommendResult Recommender::coldStart(int topN) const {
    const auto& items = rm_.allItems();

    RecommendResult result;
    result.reserve(items.size());
    for (int iid : items) {
        double total = rm_.itemTotalScore(iid);
        result.emplace_back(iid, total);
    }

    std::sort(result.begin(), result.end(),
              [](const auto& a, const auto& b){ return a.second > b.second; });

    if ((int)result.size() > topN) result.resize(topN);
    return result;
}