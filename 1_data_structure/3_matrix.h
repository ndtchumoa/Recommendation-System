#tạo 1 matrix rating gồm user id, product id và nd là score- product là hàng ngang
#chuẩn hóa ma trận,... theo lý thuyết để tính cosine
#từ mai trận rating tính sum của từng hàng để xem product nào có score cao nhất 

#include "2_struct.h"
class Matrix
{
private:

    int rows;
    int cols;

    double** data;

public:

    Matrix(int r, int c);

    ~Matrix();

    void set(
        int r,
        int c,
        double value
    );

    double get(
        int r,
        int c
    );

    int getRows();

    int getCols();
};

--------------------------------

Matrix buildRatingMatrix(

    Interaction* interactions,

    int numInteractions,

    int numUsers,

    int numProducts
); 