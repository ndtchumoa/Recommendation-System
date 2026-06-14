// ═══════════════════════════════════════════════════════════════════════
// main.cpp  —  Hệ thống gợi ý sản phẩm Item-Based CF  (C++)
//
// Quy trình:
//   1. Load CSV  →  2. Build Rating Matrix  →  3. Calculate Similarity
//   →  4. Recommend  →  5. Print Top-N
//
// Ngoài ra, main.cpp còn cung cấp menu để đồng bộ 2 chiều với MySQL:
//   - Import: đọc users/items/interactions.csv  ->  ghi vào MySQL
//   - Export: đọc users/items/interactions từ MySQL  ->  ghi ra CSV
//
// Compile (g++, xem chi tiết trong SETUP_MYSQL_CONNECTOR.md):
//   g++ -std=c++17 -O2 -Wall -Ithird_party/mysql/include
//       main.cpp 1_models/*.cpp 2_data/*.cpp 3_core/*.cpp
//       -Lthird_party/mysql/lib -llibmysql -o recommendation.exe
//
// Chạy:
//   ./recommendation.exe
// ═══════════════════════════════════════════════════════════════════════

#include <iostream>
#include <string>
#include <stdexcept>
#include <iomanip>
#include <limits>

#include "2_data/CSVLoader.h"
#include "2_data/DatabaseManager.h"
#include "3_core/RatingMatrix.h"
#include "3_core/SimilarityMatrix.h"
#include "3_core/Recommender.h"

// ─────────────────────────────────────────────────────────────────────
// Cấu hình kết nối MySQL — đọc từ "2_data/db_config.h"
//
// File "db_config.h" KHÔNG được commit lên Git (xem .gitignore).
// Nếu chưa có file này, hãy copy "2_data/db_config.example.h" thành
// "2_data/db_config.h" rồi sửa DB_USER/DB_PASS/DB_NAME cho khớp máy bạn.
// (Xem hướng dẫn chi tiết trong README.md, phần "Hướng dẫn cho thành
//  viên nhóm").
// ─────────────────────────────────────────────────────────────────────
#include "2_data/db_config.h"

// Đường dẫn các tệp CSV gốc (dùng cho chạy gợi ý + import vào DB)
static const std::string USERS_CSV        = "4_dataset/users.csv";
static const std::string ITEMS_CSV        = "4_dataset/items.csv";
static const std::string INTERACTIONS_CSV = "4_dataset/interactions.csv";

// Đường dẫn các tệp CSV xuất ra (export từ DB, không ghi đè dữ liệu gốc)
static const std::string EXPORT_USERS_CSV        = "4_dataset/export_users.csv";
static const std::string EXPORT_ITEMS_CSV        = "4_dataset/export_items.csv";
static const std::string EXPORT_INTERACTIONS_CSV = "4_dataset/export_interactions.csv";

// ─────────────────────────────────────────────────────────────────────
// Chức năng 1: Chạy hệ thống gợi ý từ dữ liệu CSV (luồng gốc)
// ─────────────────────────────────────────────────────────────────────
void runRecommendationFromCSV() {
    CSVLoader loader;

    std::vector<User>        users;
    std::vector<Item>        items;
    std::vector<Interaction> interactions;

    try {
        users        = loader.loadUsers(USERS_CSV);
        items        = loader.loadItems(ITEMS_CSV);
        interactions = loader.loadInteractions(INTERACTIONS_CSV);
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] " << e.what() << "\n";
        return;
    }

    std::cout << "\n>>> Build Rating Matrix\n";
    RatingMatrix ratingMatrix;
    ratingMatrix.build(interactions);
    ratingMatrix.print();

    std::cout << ">>> Tinh Item Similarity (Cosine)\n";
    SimilarityMatrix simMatrix;
    simMatrix.build(ratingMatrix);
    simMatrix.print();

    std::cout << ">>> Recommendation\n";
    Recommender engine(ratingMatrix, simMatrix, items);
    const int TOP_N = 5;

    std::cout << "\n--- Nguoi dung cu ---\n";
    for (const auto& u : users) {
        auto result = engine.forUser(u.user_id, TOP_N);
        engine.printResult(u.user_id, result);
    }

    std::cout << "\n--- Nguoi dung moi (chua co lich su) ---\n";
    auto trending = engine.coldStart(TOP_N);
    engine.printResult("NEW_USER", trending);

    std::cout << "\n--- San pham tuong tu voi I001 ---\n";
    auto similar = simMatrix.topK("I001", 5);
    std::cout << "Top 5 san pham tuong tu I001:\n";
    int rank = 1;
    for (const auto& [iid, sim] : similar) {
        std::cout << "  " << rank++ << ". " << iid
                  << "  sim=" << std::fixed << std::setprecision(4) << sim << "\n";
    }

    std::cout << "\n[DONE] Chay goi y tu CSV hoan tat.\n";
}

// ─────────────────────────────────────────────────────────────────────
// Chức năng 2: Import CSV -> MySQL Database
// ─────────────────────────────────────────────────────────────────────
void importCsvToDatabase() {
    CSVLoader loader;
    DatabaseManager db;

    std::cout << "\n>>> Ket noi MySQL (" << DB_HOST << ":" << DB_PORT
              << ", db=" << DB_NAME << ")...\n";
    if (!db.connect(DB_HOST, DB_USER, DB_PASS, DB_NAME, DB_PORT)) {
        std::cerr << "[ERROR] Khong the ket noi MySQL. "
                     "Kiem tra cau hinh DB_HOST/DB_USER/DB_PASS/DB_NAME.\n";
        return;
    }

    std::cout << ">>> Tao bang (neu chua co)...\n";
    if (!db.createTables()) {
        std::cerr << "[ERROR] Tao bang that bai.\n";
        db.disconnect();
        return;
    }

    try {
        // Thứ tự QUAN TRỌNG: users, items phải được insert TRƯỚC
        // interactions vì interactions có FOREIGN KEY tham chiếu tới
        // users(user_id) và items(item_id).
        std::cout << ">>> Doc users.csv va import...\n";
        auto users = loader.loadUsers(USERS_CSV);
        int okUsers = 0;
        for (const auto& u : users)
            if (db.insertUser(u)) ++okUsers;
        std::cout << "    -> Import thanh cong " << okUsers << "/"
                  << users.size() << " users.\n";

        std::cout << ">>> Doc items.csv va import...\n";
        auto items = loader.loadItems(ITEMS_CSV);
        int okItems = 0;
        for (const auto& it : items)
            if (db.insertItem(it)) ++okItems;
        std::cout << "    -> Import thanh cong " << okItems << "/"
                  << items.size() << " items.\n";

        std::cout << ">>> Doc interactions.csv va import...\n";
        auto interactions = loader.loadInteractions(INTERACTIONS_CSV);
        int okInter = 0;
        for (const auto& i : interactions)
            if (db.insertInteraction(i)) ++okInter;
        std::cout << "    -> Import thanh cong " << okInter << "/"
                  << interactions.size() << " interactions.\n";

    } catch (const std::exception& e) {
        std::cerr << "[ERROR] " << e.what() << "\n";
    }

    db.disconnect();
    std::cout << "[DONE] Import CSV -> MySQL hoan tat.\n";
}

// ─────────────────────────────────────────────────────────────────────
// Chức năng 3: Export MySQL Database -> CSV
// ─────────────────────────────────────────────────────────────────────
void exportDatabaseToCsv() {
    CSVLoader loader;
    DatabaseManager db;

    std::cout << "\n>>> Ket noi MySQL (" << DB_HOST << ":" << DB_PORT
              << ", db=" << DB_NAME << ")...\n";
    if (!db.connect(DB_HOST, DB_USER, DB_PASS, DB_NAME, DB_PORT)) {
        std::cerr << "[ERROR] Khong the ket noi MySQL. "
                     "Kiem tra cau hinh DB_HOST/DB_USER/DB_PASS/DB_NAME.\n";
        return;
    }

    std::cout << ">>> Doc du lieu tu MySQL...\n";
    auto users        = db.loadUsers();
    auto items        = db.loadItems();
    auto interactions = db.loadInteractions();

    std::cout << ">>> Ghi ra cac tep CSV...\n";
    loader.saveUsers(EXPORT_USERS_CSV, users);
    loader.saveItems(EXPORT_ITEMS_CSV, items);
    loader.saveInteractions(EXPORT_INTERACTIONS_CSV, interactions);

    db.disconnect();
    std::cout << "[DONE] Export MySQL -> CSV hoan tat. Xem cac tep:\n"
              << "    " << EXPORT_USERS_CSV << "\n"
              << "    " << EXPORT_ITEMS_CSV << "\n"
              << "    " << EXPORT_INTERACTIONS_CSV << "\n";
}

// ─────────────────────────────────────────────────────────────────────
// Menu chính
// ─────────────────────────────────────────────────────────────────────
void printMenu() {
    std::cout << "\n========================================================\n";
    std::cout << "  HE THONG GOI Y SAN PHAM - MENU\n";
    std::cout << "========================================================\n";
    std::cout << "  1. Chay he thong goi y (doc tu CSV)\n";
    std::cout << "  2. Import CSV -> MySQL Database\n";
    std::cout << "  3. Export MySQL Database -> CSV\n";
    std::cout << "  0. Thoat\n";
    std::cout << "========================================================\n";
    std::cout << "Chon: ";
}

int main() {
    std::cout << "+--------------------------------------------------+\n";
    std::cout << "|     He Thong Goi Y San Pham - Item-Based CF       |\n";
    std::cout << "+--------------------------------------------------+\n";

    while (true) {
        printMenu();

        int choice;
        if (!(std::cin >> choice)) {
            // Input không hợp lệ (vd: nhập chữ) -> xóa buffer và thử lại
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "[WARN] Vui long nhap mot so.\n";
            continue;
        }

        switch (choice) {
            case 1:
                runRecommendationFromCSV();
                break;
            case 2:
                importCsvToDatabase();
                break;
            case 3:
                exportDatabaseToCsv();
                break;
            case 0:
                std::cout << "Tam biet!\n";
                return 0;
            default:
                std::cout << "[WARN] Lua chon khong hop le.\n";
                break;
        }
    }

    return 0;
}
