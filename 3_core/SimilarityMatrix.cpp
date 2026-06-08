#include "SimilarityMatrix.h"
#include <cmath>
#include <algorithm>
#include <iostream>
#include <iomanip>

void SimilarityMatrix::build(const RatingMatrix& rm) {
    items_ = rm.items();
    int N  = (int)items_.size();

    // Lấy vector cột cho mỗi item
    std::vector<std::vector<double>> cols(N);
    for (int c = 0; c < N; ++c)
        cols[c] = rm.itemColumn(c);

    // Khởi tạo ma trận NxN
    sim_.assign(N, std::vector<double>(N, 0.0));

    // Tính cosine similarity cho mọi cặp (i, j)
    for (int i = 0; i < N; ++i) {
        sim_[i][i] = 1.0;                      // chính nó = 1
        for (int j = i + 1; j < N; ++j) {
            double s    = cosine(cols[i], cols[j]);
            sim_[i][j]  = s;
            sim_[j][i]  = s;                   // đối xứng
        }
    }

    // Xây index
    for (int i = 0; i < N; ++i)
        itemIdx_[items_[i]] = i;
}

double SimilarityMatrix::get(const std::string& a,
                             const std::string& b) const {
    auto ia = itemIdx_.find(a);
    auto ib = itemIdx_.find(b);
    if (ia == itemIdx_.end() || ib == itemIdx_.end()) return 0.0;
    return sim_[ia->second][ib->second];
}

std::vector<std::pair<std::string,double>>
SimilarityMatrix::topK(const std::string& iid, int k) const {
    auto it = itemIdx_.find(iid);
    if (it == itemIdx_.end()) return {};

    int idx = it->second;
    std::vector<std::pair<std::string,double>> cands;
    cands.reserve(items_.size() - 1);

    for (int j = 0; j < (int)items_.size(); ++j) {
        if (j == idx) continue;
        cands.emplace_back(items_[j], sim_[idx][j]);
    }
    std::sort(cands.begin(), cands.end(),
              [](const auto& a, const auto& b){ return a.second > b.second; });
    if ((int)cands.size() > k) cands.resize(k);
    return cands;
}

void SimilarityMatrix::print() const {
    int N = (int)items_.size();
    const int W = 7;
    std::cout << "\n=== Item Similarity Matrix (" << N << "x" << N << ") ===\n";
    std::cout << std::setw(6) << "";
    for (const auto& id : items_) std::cout << std::setw(W) << id;
    std::cout << "\n";
    for (int i = 0; i < N; ++i) {
        std::cout << std::setw(6) << items_[i];
        for (int j = 0; j < N; ++j)
            std::cout << std::setw(W) << std::fixed
                      << std::setprecision(2) << sim_[i][j];
        std::cout << "\n";
    }
    std::cout << "\n";
}

// ── Cosine similarity ─────────────────────────────────────────────────
double SimilarityMatrix::cosine(const std::vector<double>& a,
                                const std::vector<double>& b) {
    double dot = 0.0, na = 0.0, nb = 0.0;
    for (size_t i = 0; i < a.size(); ++i) {
        dot += a[i] * b[i];
        na  += a[i] * a[i];
        nb  += b[i] * b[i];
    }
    if (na == 0.0 || nb == 0.0) return 0.0;   // tránh chia 0
    return dot / (std::sqrt(na) * std::sqrt(nb));
}