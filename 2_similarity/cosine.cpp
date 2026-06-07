// 2_similarity/cosine.cpp
 
#include "cosine.h"
 
double cosineSimilarity(double* vectorA, double* vectorB, int size) {
    double dot    = 0.0;  // tích vô hướng A · B
    double normA  = 0.0;  // ||A||
    double normB  = 0.0;  // ||B||
 
    for (int i = 0; i < size; i++) {
        dot   += vectorA[i] * vectorB[i];
        normA += vectorA[i] * vectorA[i];
        normB += vectorB[i] * vectorB[i];
    }
 
    // Tránh chia cho 0 (vector toàn 0 = user/item chưa có tương tác)
    if (normA == 0.0 || normB == 0.0) return 0.0;
 
    return dot / (std::sqrt(normA) * std::sqrt(normB));
}
 