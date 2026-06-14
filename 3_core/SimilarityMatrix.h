#ifndef SIMILARITY_MATRIX_H
#define SIMILARITY_MATRIX_H

#include <string>
#include "../2_data/MyDataStructures.h"
#include "RatingMatrix.h"

class SimilarityMatrix {
private:
    MyMap<std::string, MyMap<std::string, double>> simMatrix;

    double calculateCosine(const MyMap<std::string, double>& itemA_users,
                           const MyMap<std::string, double>& itemB_users) const;

public:
    SimilarityMatrix();

    void build(const RatingMatrix& ratingMatrix);

    MyVector<MyPair<std::string, double>> getTopKSimilarItems(const std::string& item_id, int k) const;
    MyVector<MyPair<std::string, double>> topK(const std::string& item_id, int k) const;

    void printMatrix() const;
    void sortPairDescByScore(MyVector<MyPair<std::string, double>>& arr);
};

#endif // SIMILARITY_MATRIX_H