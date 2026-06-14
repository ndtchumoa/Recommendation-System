#ifndef RATING_MATRIX_H
#define RATING_MATRIX_H

#include <string>
#include "../2_data/MyDataStructures.h"
#include "../1_models/Interaction.h"

class RatingMatrix {
private:
    MyMap<std::string, MyMap<std::string, double>> matrix;
    MyMap<std::string, double> user_means;

public:
    RatingMatrix();

    void buildMatrix(const MyVector<Interaction>& interactions);
    void normalizeMatrix();

    const MyMap<std::string, MyMap<std::string, double>>& getMatrix() const;

    bool isNewUser(const std::string& user_id) const;
    double get(const std::string& user_id, const std::string& item_id) const;
    MyVector<std::string> items() const;
    double itemTotalScore(const std::string& item_id) const;

    void printMatrix() const;
};

#endif // RATING_MATRIX_H