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
