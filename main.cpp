<<<<<<< HEAD
=======
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

>>>>>>> b8c1ae64c94847b04dee00ae1cc482c9f316487e
#include <iostream>
#include <iomanip>
<<<<<<< HEAD
#include <string>
=======
#include <limits>
>>>>>>> b8c1ae64c94847b04dee00ae1cc482c9f316487e

#include "1_models/Item.h"
#include "2_data/MyDataStructures.h"
#include "2_data/CSVLoader.h"
<<<<<<< HEAD
#include "6_services/ItemManager.h"
#include "6_services/InteractionManager.h"
#include "6_services/UserManager.h"

int main() {
    std::cout << "==============================================\n";
    std::cout << "=  RECOMMENDATION SYSTEM (FULL NO-STL VER)   =\n";
    std::cout << "==============================================\n\n";

    // ─────────────────────────────────────────────────────────────
    // 1. NẠP DỮ LIỆU
    // ─────────────────────────────────────────────────────────────
    std::cout << "[1] Dang doc du lieu tu thu muc 4_dataset...\n";
=======
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

std::vector<std::string> getRecommendationData(std::string userId) {
    CSVLoader loader;
    int id = std::stoi(userId.substr(1));
    auto interactions = loader.loadInteractions("4_dataset/interactions.csv");
    auto items = loader.loadItems("4_dataset/items.csv");
    
    RatingMatrix rm; rm.build(interactions);
    SimilarityMatrix sm; sm.build(rm);
    Recommender engine(rm, sm, items);
    
    auto results = engine.forUser(userId, 5); // Lưu ý kiểu dữ liệu ID của bạn
    std::vector<std::string> itemIds;
    for(auto& pair : results) itemIds.push_back(pair.first);
    return itemIds;
}

// ─────────────────────────────────────────────────────────────────────
// Chức năng 1: Chạy hệ thống gợi ý từ dữ liệu CSV (luồng gốc)
// ─────────────────────────────────────────────────────────────────────
void runRecommendationFromCSV() {
>>>>>>> b8c1ae64c94847b04dee00ae1cc482c9f316487e
    CSVLoader loader;
    loader.loadUsers("4_dataset/users.csv");
    loader.loadItems("4_dataset/items.csv");
    loader.loadInteractions("4_dataset/interactions.csv");

    const MyVector<User>& users = loader.getUsers();
    const MyVector<Item>& items = loader.getItems();

<<<<<<< HEAD
    std::cout << "   Users: " << users.size()
              << " | Items: " << items.size() << "\n\n";

    // ─────────────────────────────────────────────────────────────
    // 2. KHỞI TẠO CÁC MANAGER
    // ─────────────────────────────────────────────────────────────
    UserManager userMgr(users);
    ItemManager itemMgr(items);
    InteractionManager interactMgr(items);

    // ─────────────────────────────────────────────────────────────
    // 3. DEMO: AUTH (UserManager)
    // ─────────────────────────────────────────────────────────────
    std::cout << "══════════════════════════════════════════════\n";
    std::cout << "   AUTH: Dang ky & Dang nhap & Dang xuat\n";
    std::cout << "══════════════════════════════════════════════\n";

    std::cout << "\n--- DANG KY ---\n";
    MyPair<bool, std::string> regResult = userMgr.Register("Nguyen Van Teo", "teo@email.com", "123456");
    if (regResult.first) {
        std::cout << "   => Thanh cong! User ID: " << regResult.second << "\n";
    } else {
        std::cout << "   => That bai: " << regResult.second << "\n";
    }

    std::cout << "\n--- DANG NHAP ---\n";
    MyPair<bool, std::string> loginResult = userMgr.Login("teo@email.com", "123456");
    if (loginResult.first) {
        std::cout << "   => Token: " << loginResult.second << "\n";
    } else {
        std::cout << "   => That bai: " << loginResult.second << "\n";
    }

    std::cout << "\n--- DANG XUAT ---\n";
    bool logoutOk = userMgr.Logout(userMgr.getLoggedInUserId());
    std::cout << "   => " << (logoutOk ? "OK" : "That bai") << "\n\n";

    // ─────────────────────────────────────────────────────────────
    // 4. DEMO: ITEM (ItemManager)
    // ─────────────────────────────────────────────────────────────
    std::cout << "══════════════════════════════════════════════\n";
    std::cout << "   ITEM: Tim kiem & Loc & Xem chi tiet\n";
    std::cout << "══════════════════════════════════════════════\n";

    std::cout << "\n--- TIM KIEM: 'sach' ---\n";
    MyVector<Item> searchResult = itemMgr.SearchProducts("sach");
    for (int i = 0; i < searchResult.size(); ++i) {
        std::cout << "   " << (i+1) << ". [" << searchResult[i].getItemId() << "] "
                  << searchResult[i].getName() << "\n";
    }

    std::cout << "\n--- LOC: category=Accessory, sortBy=price_asc ---\n";
    MyVector<Item> filtered = itemMgr.FilterAndSortProducts("Accessory", "", 0.0, "price_asc");
    for (int i = 0; i < filtered.size(); ++i) {
        std::cout << "   " << (i+1) << ". [" << filtered[i].getItemId() << "] "
                  << filtered[i].getName()
                  << " | Price: " << filtered[i].getPrice() << "\n";
    }

    std::cout << "\n--- CHI TIET SAN PHAM I001 ---\n";
    Item detail;
    if (itemMgr.GetProductDetail("I001", detail)) {
        detail.displayInfo();
    }

    // ─────────────────────────────────────────────────────────────
    // 5. DEMO: CART & ORDER (InteractionManager)
    // ─────────────────────────────────────────────────────────────
    std::cout << "\n══════════════════════════════════════════════\n";
    std::cout << "   CART: Them, cap nhat, xem, xoa\n";
    std::cout << "══════════════════════════════════════════════\n";

    std::string demoUser = "U001";

    std::cout << "\n--- THEM VAO GIO HANG ---\n";
    interactMgr.AddToCart(demoUser, "I001", 1);
    interactMgr.AddToCart(demoUser, "I013", 2);
    interactMgr.AddToCart(demoUser, "I005", 1);

    std::cout << "\n--- XEM GIO HANG ---\n";
    MyPair<MyVector<CartItem>, double> cartInfo = interactMgr.GetCartDetails(demoUser);
    MyVector<CartItem>& cartItems = cartInfo.first;
    double cartTotal = cartInfo.second;
    for (int i = 0; i < cartItems.size(); ++i) {
        std::cout << "   " << (i+1) << ". [" << cartItems[i].item_id << "] "
                  << cartItems[i].item_name
                  << " | SL: " << cartItems[i].quantity
                  << " | Don gia: " << cartItems[i].price
                  << " | Thanh tien: " << cartItems[i].subtotal << "\n";
    }
    std::cout << "   => TONG: " << cartTotal << "\n";

    std::cout << "\n--- CAP NHAT SO LUONG I001: 3 ---\n";
    MyPair<double, int> updateResult = interactMgr.UpdateCartItem(demoUser, "I001", 3);
    std::cout << "   => Tong moi: " << updateResult.first
              << " | So luong: " << updateResult.second << "\n";

    std::cout << "\n--- TAO DON HANG ---\n";
    MyPair<bool, std::string> orderResult = interactMgr.CreateOrder(demoUser, "COD");
    if (orderResult.first) {
        std::cout << "   => Order ID: " << orderResult.second << "\n";

        std::cout << "\n--- GOI Y (CROSS-SELL) SAU KHI MUA ---\n";
        MyVector<Item> suggestions = interactMgr.GetSuggestedProducts(orderResult.second);
        for (int i = 0; i < suggestions.size(); ++i) {
            std::cout << "   " << (i+1) << ". [" << suggestions[i].getItemId() << "] "
                      << suggestions[i].getName()
                      << " | Category: " << suggestions[i].getCategory() << "\n";
        }
    }

    std::cout << "\n--- CHI TIET DON HANG ---\n";
    Order placedOrder;
    if (interactMgr.GetOrderDetails(orderResult.second, placedOrder)) {
        std::cout << "   Order ID: " << placedOrder.order_id << "\n";
        std::cout << "   User: " << placedOrder.user_id << "\n";
        std::cout << "   Status: " << placedOrder.status << "\n";
        std::cout << "   Payment: " << placedOrder.payment_method << "\n";
        std::cout << "   Created: " << placedOrder.created_at << "\n";
        std::cout << "   Total: " << placedOrder.total_amount << "\n";
    }

    std::cout << "\n--- GHI NHAN TUONG TAC ---\n";
    interactMgr.AddInteraction("U001", "I001", "click");
    interactMgr.AddInteraction("U001", "I001", "add_cart");
    interactMgr.AddInteraction("U001", "I001", "purchase");

    std::cout << "\n==============================================\n";
    std::cout << "=  KET THUC DEMO                           =\n";
    std::cout << "==============================================\n";
=======
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


    crow::SimpleApp app;

  CROW_ROUTE(app, "/recommend/<string>")([](std::string userId){
    // Bước 1: Xử lý chuỗi để lấy phần số nếu hàm getRecommendationData của m cần số
    // Ví dụ: U001 -> 1. Nếu hàm của m nhận string thì m cứ để nguyên userId nhé!
    // auto items = getRecommendationData(std::stoi(userId.substr(1))); 
    
    // Gọi hàm với userId là string (m sửa hàm getRecommendationData nhận string là ổn nhất)
    auto items = getRecommendationData(userId); 
    
    crow::json::wvalue res;
    res["status"] = "success";
    res["user_id"] = userId;
    
    // Tạo danh sách recommendations
    crow::json::wvalue::list items_list;
    for (const auto& item : items) {
        items_list.push_back(item);
    }
    res["recommendations"] = std::move(items_list);
    
    crow::response r(res);
    r.add_header("Access-Control-Allow-Origin", "*");
    return r;
});
    // Chạy server ở luồng riêng (không chặn Menu)
    std::thread server_thread([&](){ app.port(8080).run(); });
    server_thread.detach();

    std::cout << ">>> Server API da khoi dong tai port 8080!\n";

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

>>>>>>> b8c1ae64c94847b04dee00ae1cc482c9f316487e
    return 0;
}
