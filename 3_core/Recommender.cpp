#include "Recommender.h"
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <iostream>
#include <iomanip>

Recommender::Recommender(const RatingMatrix&     rm,
                         const SimilarityMatrix& sm,
                         const std::vector<Item>& items)
    : rm_(rm), sm_(sm), items_(items) {}

// ── forUser ───────────────────────────────────────────────────────────
RecommendResult Recommender::forUser(const std::string& uid,
                                     int topN) const {
    // User mới hoặc chưa có interaction → Cold Start
    if (rm_.isNewUser(uid)) {
        std::cout << "[Recommender] '" << uid
                  << "' la nguoi dung moi -> Cold Start\n";
        return coldStart(topN);
    }

    const auto& allItems = rm_.items();

    // Tập item user đã tương tác
    std::unordered_set<std::string> seen;
    for (const auto& iid : allItems)
        if (rm_.get(uid, iid) > 0.0) seen.insert(iid);

    // Tích lũy điểm dự đoán cho các item chưa tương tác
    // predicted[j] = Σ sim(i,j)×score(u,i)  /  Σ|sim(i,j)|
    std::unordered_map<std::string, double> num, den;
    for (const auto& iid : seen) {
        double userScore = rm_.get(uid, iid);
        auto neighbors   = sm_.topK(iid, (int)allItems.size());
        for (const auto& [nbr, sim] : neighbors) {
            if (seen.count(nbr)) continue;   // đã xem → bỏ
            num[nbr] += sim * userScore;
            den[nbr] += std::abs(sim);
        }
    }

    // Tính điểm cuối và gom vào result
    RecommendResult result;
    for (const auto& [iid, n] : num) {
        if (den.at(iid) == 0.0) continue;
        result.emplace_back(iid, n / den.at(iid));
    }

    std::sort(result.begin(), result.end(),
              [](const auto& a, const auto& b){ return a.second > b.second; });
    if ((int)result.size() > topN) result.resize(topN);
    return result;
}

// ── coldStart ─────────────────────────────────────────────────────────
RecommendResult Recommender::coldStart(int topN) const {
    RecommendResult result;
    for (const auto& iid : rm_.items())
        result.emplace_back(iid, rm_.itemTotalScore(iid));

    std::sort(result.begin(), result.end(),
              [](const auto& a, const auto& b){ return a.second > b.second; });
    if ((int)result.size() > topN) result.resize(topN);
    return result;
}

// ── printResult ───────────────────────────────────────────────────────
void Recommender::printResult(const std::string& label,
                               const RecommendResult& result) const {
    // Map item_id → Item* để tra tên nhanh
    std::unordered_map<std::string, const Item*> itemMap;
    for (const auto& it : items_) itemMap[it.item_id] = &it;

    const int LINE = 52;
    std::string border(LINE, '=');

    std::cout << "\n+" << border << "+\n";
    std::cout << "| Goi y cho: " << std::left
              << std::setw(LINE - 12) << label << "|\n";
    std::cout << "+" << border << "+\n";
    std::cout << "| #   Item     Score     Ten san pham"
              << std::setw(LINE - 36) << "" << "|\n";
    std::cout << "+" << border << "+\n";

    int rank = 1;
    for (const auto& [iid, score] : result) {
        std::string name = "?";
        if (itemMap.count(iid)) name = itemMap.at(iid)->name;
        if ((int)name.size() > 24) name = name.substr(0, 21) + "...";

        std::cout << "| " << std::right << std::setw(2)  << rank++
                  << "   " << std::left << std::setw(6)  << iid
                  << "  "  << std::right << std::fixed
                  << std::setprecision(2) << std::setw(8) << score
                  << "  "  << std::left << std::setw(24) << name
                  << " |\n";
    }
    std::cout << "+" << border << "+\n";
}