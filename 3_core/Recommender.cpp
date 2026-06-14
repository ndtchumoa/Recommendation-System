#include "Recommender.h"
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <iomanip>

Recommender::Recommender(const RatingMatrix&     rm,
                         const SimilarityMatrix& sm,
                         const MyVector<Item>& items)
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

    const MyVector<std::string>& allItems = rm_.items();

    // Tập item user đã tương tác (dùng MyMap như một set)
    MyMap<std::string, bool> seen;
    for (int i = 0; i < allItems.size(); ++i) {
        if (rm_.get(uid, allItems[i]) > 0.0) {
            seen[allItems[i]] = true;
        }
    }

    // Tích lũy điểm dự đoán cho các item chưa tương tác
    // predicted[j] = Σ sim(i,j)×score(u,i)  /  Σ|sim(i,j)|
    MyMap<std::string, double> num, den;

    MyVector<std::string> seenKeys = seen.keys();
    for (int s = 0; s < seenKeys.size(); ++s) {
        std::string iid = seenKeys[s];
        double userScore = rm_.get(uid, iid);
        MyVector<MyPair<std::string, double>> neighbors = sm_.topK(iid, allItems.size());

        for (int n = 0; n < neighbors.size(); ++n) {
            std::string nbr = neighbors[n].first;
            double sim = neighbors[n].second;
            if (seen.contains(nbr)) continue;   // đã xem → bỏ
            num[nbr] += sim * userScore;
            den[nbr] += std::abs(sim);
        }
    }

    // Tính điểm cuối và gom vào result
    RecommendResult result;
    MyVector<std::string> numKeys = num.keys();
    for (int i = 0; i < numKeys.size(); ++i) {
        std::string iid = numKeys[i];
        double n = num[iid];
        double d;
        if (!den.find(iid, d) || d == 0.0) continue;
        result.push_back(MyPair<std::string, double>(iid, n / d));
    }

    // Sắp xếp giảm dần theo score (Bubble Sort)
    for (int i = 0; i < result.size() - 1; ++i) {
        for (int j = 0; j < result.size() - i - 1; ++j) {
            if (result[j].second < result[j + 1].second) {
                MyPair<std::string, double> temp = result[j];
                result[j] = result[j + 1];
                result[j + 1] = temp;
            }
        }
    }

    if (result.size() > topN) {
        MyVector<MyPair<std::string, double>> truncated;
        for (int i = 0; i < topN; ++i) {
            truncated.push_back(result[i]);
        }
        result = truncated;
    }
    return result;
}

// ── coldStart ─────────────────────────────────────────────────────────
RecommendResult Recommender::coldStart(int topN) const {
    MyVector<MyPair<std::string, double>> result;
    MyVector<std::string> allItems = rm_.items();
    for (int i = 0; i < allItems.size(); ++i) {
        result.push_back(MyPair<std::string, double>(allItems[i], rm_.itemTotalScore(allItems[i])));
    }

    // Sắp xếp giảm dần theo score
    for (int i = 0; i < result.size() - 1; ++i) {
        for (int j = 0; j < result.size() - i - 1; ++j) {
            if (result[j].second < result[j + 1].second) {
                MyPair<std::string, double> temp = result[j];
                result[j] = result[j + 1];
                result[j + 1] = temp;
            }
        }
    }

    if (result.size() > topN) {
        MyVector<MyPair<std::string, double>> truncated;
        for (int i = 0; i < topN; ++i) {
            truncated.push_back(result[i]);
        }
        result = truncated;
    }
    return result;
}

// ── printResult ───────────────────────────────────────────────────────
void Recommender::printResult(const std::string& label,
                               const RecommendResult& result) const {
    // Map item_id → Item* để tra tên nhanh
    MyMap<std::string, const Item*> itemMap;
    for (int i = 0; i < items_.size(); ++i) {
        itemMap[items_[i].getItemId()] = &items_[i];
    }

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
    for (int i = 0; i < result.size(); ++i) {
        std::string iid = result[i].first;
        double score = result[i].second;

        std::string name = "?";
        const Item* itemPtr;
        if (itemMap.find(iid, itemPtr)) {
            name = itemPtr->getName();
        }
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