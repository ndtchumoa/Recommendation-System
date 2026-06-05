#hàm gợi ý top K sản phẩm tương đòng
#hàm gợi ý top K sản phẩm có điểm cao cho user mới 
3_recommendation/

item-item.h
 include

#include "../1_data_structure/3_matrix.h"

#include "../2_similarity/cosine.h"
void normalizeMatrix(
    Matrix& ratingMatrix
);

--------------------------------

Matrix buildItemSimilarityMatrix(

    Matrix& ratingMatrix
);

--------------------------------

double predictScore(

    int userIndex,

    int itemIndex,

    Matrix& ratingMatrix,

    Matrix& itemSimilarityMatrix
);

--------------------------------

void recommendProducts(

    int userIndex,

    Matrix& ratingMatrix,

    Matrix& itemSimilarityMatrix,

    Product* products,

    int numProducts,

    int topK
);

--------------------------------

void recommendTrendingProducts(

    Matrix& ratingMatrix,

    Product* products,

    int numProducts,

    int topK
);
bool isNewUser(

    int userIndex,

    Matrix& ratingMatrix
);
recommendTrendingProducts()

Input:

- ratingMatrix
- products
- numProducts
- topK
Logic recommendTrendingProducts()

1. Tạo mảng điểm cho từng sản phẩm

score[item] = tổng rating cột item

2. Sắp xếp giảm dần

3. Lấy Top K sản phẩm