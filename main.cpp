    // ═══════════════════════════════════════════════════════════════════════
    // main.cpp  —  Hệ thống gợi ý sản phẩm Item-Based CF  (C++)
    //
    // Quy trình:
    //   1. Load CSV  →  2. Build Rating Matrix  →  3. Calculate Similarity
    //   →  4. Recommend  →  5. Print Top-N
    //
    // Compile (g++):
    //   g++ -std=c++17 -O2 -Wall main.cpp -o recommendation.exe
    //
    // Chạy:
    //   ./recommendation.exe
    // ═══════════════════════════════════════════════════════════════════════

    #include <iostream>
    #include <string>
    #include <stdexcept>
    #include <iomanip>

    #include "2_data/CSVLoader.h"
    #include "3_core/RatingMatrix.h"
    #include "3_core/SimilarityMatrix.h"
    #include "3_core/Recommender.h"

    int main() {
        std::cout << "╔══════════════════════════════════════════════════╗\n";
        std::cout << "║     He Thong Goi Y San Pham — Item-Based CF     ║\n";
        std::cout << "╚══════════════════════════════════════════════════╝\n\n";

        // ─── BƯỚC 1: Load CSV ─────────────────────────────────────────────
        std::cout << ">>> BUOC 1: Load CSV\n";
        CSVLoader loader;

        std::vector<User>        users;
        std::vector<Item>        items;
        std::vector<Interaction> interactions;

        try {
            users        = loader.loadUsers       ("4_dataset/users.csv");
            items        = loader.loadItems       ("4_dataset/items.csv");
            interactions = loader.loadInteractions("4_dataset/interactions.csv");
        } catch (const std::exception& e) {
            std::cerr << "[ERROR] " << e.what() << "\n";
            return 1;
        }

        // ─── BƯỚC 2: Build Rating Matrix ──────────────────────────────────
        std::cout << "\n>>> BUOC 2: Build Rating Matrix\n";
        RatingMatrix ratingMatrix;
        ratingMatrix.build(interactions);
        ratingMatrix.print();   // in ra màn hình để kiểm tra

        // ─── BƯỚC 3: Calculate Item Similarity ────────────────────────────
        std::cout << ">>> BUOC 3: Tinh Item Similarity (Cosine)\n";
        SimilarityMatrix simMatrix;
        simMatrix.build(ratingMatrix);
        simMatrix.print();      // in ma trận similarity

        // ─── BƯỚC 4 & 5: Recommend & Print ────────────────────────────────
        std::cout << ">>> BUOC 4: Recommendation\n";
        Recommender engine(ratingMatrix, simMatrix, items);

        const int TOP_N = 5;

        // Gợi ý cho người dùng CŨ (đã có lịch sử)
        std::cout << "\n--- Nguoi dung cu ---\n";
        for (const auto& u : users) {
            auto result = engine.forUser(u.user_id, TOP_N);
            engine.printResult(u.user_id, result);
        }

        // Gợi ý cho người dùng MỚI (cold start)
        std::cout << "\n--- Nguoi dung moi (chua co lich su) ---\n";
        auto trending = engine.coldStart(TOP_N);
        engine.printResult("NEW_USER", trending);

        // Demo: tìm sản phẩm tương tự với I001
        std::cout << "\n--- San pham tuong tu voi I001 ---\n";
        auto similar = simMatrix.topK("I001", 5);
        std::cout << "Top 5 san pham tuong tu I001:\n";
        int rank = 1;
        for (const auto& [iid, sim] : similar) {
            std::cout << "  " << rank++ << ". " << iid
                    << "  sim=" << std::fixed << std::setprecision(4) << sim << "\n";
        }

        std::cout << "\n[DONE] Chuong trinh ket thuc thanh cong.\n";
        return 0;
    }