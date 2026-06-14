#pragma once
#include <string>
#include "../2_data/MyDataStructures.h"
#include "RatingMatrix.h"
#include "SimilarityMatrix.h"
#include "../1_models/Item.h"

using RecommendResult = MyVector<MyPair<std::string, double>>;

class Recommender {
public:
    Recommender(const RatingMatrix&     rm,
                const SimilarityMatrix& sm,
                const MyVector<Item>& items);

    RecommendResult forUser(const std::string& user_id, int topN = 10) const;
    RecommendResult coldStart(int topN = 10) const;

    void printResult(const std::string& label,
                     const RecommendResult& result) const;

private:
    const RatingMatrix&     rm_;
    const SimilarityMatrix& sm_;
    const MyVector<Item>& items_;
};