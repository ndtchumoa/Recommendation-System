// Dùng hàm cosine tính độ tương đồng giữa các sp
// 2_similarity/
// cosine.h
 
#ifndef COSINE_H
#define COSINE_H
 
#include <cmath>
 
// Tính cosine similarity giữa 2 vector dạng mảng double[]
// Trả về giá trị trong [0.0, 1.0] (0 = hoàn toàn khác, 1 = giống hệt)
double cosineSimilarity(
    double* vectorA,
    double* vectorB,
    int size
);
 
#endif // COSINE_H
 