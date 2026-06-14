#include <iostream>
#include <iomanip>
#include <string>
#include <thread> 
#include <vector>
#include <limits>

// Khai báo thư viện Crow (API)
#define ASIO_STANDALONE
#include "include/crow.h"

// Khai báo các class của dự án
#include "1_models/Item.h"
#include "2_data/MyDataStructures.h"
#include "2_data/CSVLoader.h"
#include "6_services/ItemManager.h"
#include "6_services/InteractionManager.h"
#include "6_services/UserManager.h"

// Khai báo Core Recommender
#include "3_core/RatingMatrix.h"
#include "3_core/SimilarityMatrix.h"
#include "3_core/Recommender.h"

int main() {
    std::cout << "==============================================\n";
    std::cout << "=  RECOMMENDATION SYSTEM (FULL NO-STL VER)   =\n";
    std::cout << "==============================================\n\n";

    // ─────────────────────────────────────────────────────────────
    // 1. NẠP DỮ LIỆU
    // ─────────────────────────────────────────────────────────────
    std::cout << "[1] Dang doc du lieu tu thu muc 4_dataset...\n";
    CSVLoader loader;
    loader.loadUsers("4_dataset/users.csv");
    loader.loadItems("4_dataset/items.csv");
    loader.loadInteractions("4_dataset/interactions.csv");

    const MyVector<User>& users = loader.getUsers();
    const MyVector<Item>& items = loader.getItems();

    std::cout << "   Users: " << users.size()
              << " | Items: " << items.size() << "\n\n";


    // ─────────────────────────────────────────────────────────────
    // 2. KHỞI TẠO RECOMMENDER ENGINE (Dành cho API)
    // ─────────────────────────────────────────────────────────────
    const MyVector<Interaction>& interactions = loader.getInteractions();
    RatingMatrix rm; 
    rm.buildMatrix(interactions);
    
    SimilarityMatrix sm; 
    sm.build(rm);
    
    // Khởi tạo Engine chung
    Recommender engine(rm, sm, loader.loadItems("4_dataset/items.csv")); 

    // ─────────────────────────────────────────────────────────────
    // 3. KHỞI TẠO VÀ CHẠY API SERVER SONG SONG
    // ─────────────────────────────────────────────────────────────
    crow::SimpleApp app;
    app.loglevel(crow::LogLevel::Warning);

    CROW_ROUTE(app, "/recommend/<string>")([&engine](std::string userId){
        auto results = engine.forUser(userId, 5); // Lấy top 5
        
        crow::json::wvalue res;
        res["status"] = "success";
        res["user_id"] = userId;
        
        crow::json::wvalue::list items_list;
        for (const auto& pair : results) {
            items_list.push_back(pair.first); // Lấy Item ID
        }
        res["recommendations"] = std::move(items_list);
        
        crow::response r(res);
        r.add_header("Access-Control-Allow-Origin", "*");
        return r;
    });

    // Chạy server API trên luồng riêng để không làm khựng phần Demo bên dưới
    std::thread server_thread([&](){ app.port(8080).run(); });
    // server_thread.detach();

    std::cout << ">>> Server API da khoi dong tai http://localhost:8080/recommend/<user_id>\n\n";

    // ─────────────────────────────────────────────────────────────
    // 2. KHỞI TẠO CÁC MANAGER
    // ─────────────────────────────────────────────────────────────
    UserManager userMgr(users);
    ItemManager itemMgr(items);
    InteractionManager interactMgr(items);

    // ─────────────────────────────────────────────────────────────
    // 3. DEMO: AUTH (UserManager)
    // ─────────────────────────────────────────────────────────────
    std::cout << "==============================================\n";
    std::cout << "   AUTH: Dang ky & Dang nhap & Dang xuat\n";
    std::cout << "==============================================\n";

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
    std::cout << "==============================================\n";
    std::cout << "   ITEM: Tim kiem & Loc & Xem chi tiet\n";
    std::cout << "==============================================\n";

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
    std::cout << "\n==============================================\n";
    std::cout << "   CART: Them, cap nhat, xem, xoa\n";
    std::cout << "==============================================\n";

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
    std::cout << "\n>>> API Server van dang hoat dong. Nhan Ctrl+C de tat server.\n";
    server_thread.join();
    return 0;
}
