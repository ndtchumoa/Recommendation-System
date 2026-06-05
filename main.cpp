#phải có sự phân loại user để dùng timestamp rồi gợi ý 
main.cpp include

#include "1_data_structure/2_struct.h"

#include "1_data_structure/3_matrix.h"

#include "2_similarity/cosine.h"

#include "3_recommendation/item-item.h"
main.cpp

if (isNewUser(userIndex, ratingMatrix))
{
    recommendTrendingProducts(
        ratingMatrix,
        products,
        numProducts,
        topK
    );
}
else
{
    recommendProducts(
        userIndex,
        ratingMatrix,
        itemSimilarityMatrix,
        products,
        numProducts,
        topK
    );
}