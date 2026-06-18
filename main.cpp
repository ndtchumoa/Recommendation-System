#include <iostream>
#include <iomanip>
#include <string>
#include <thread> 
#include <vector>
#include <limits>
#include <fstream>
#include <ctime>
#include <sstream>

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
#include "2_data/DatabaseManager.h"

// Khai báo Core Recommender
#include "3_core/RatingMatrix.h"
#include "3_core/SimilarityMatrix.h"
#include "3_core/Recommender.h"

struct cors_middleware {
    struct context {};
    void before_handle(crow::request& /*req*/, crow::response& /*res*/, context& /*ctx*/) {}
    void after_handle(crow::request& req, crow::response& res, context& /*ctx*/) {
        res.add_header("Access-Control-Allow-Origin", "*");
        res.add_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS, PUT, DELETE");
        res.add_header("Access-Control-Allow-Headers", "Content-Type, Authorization, Accept");
        
        // Nếu là lệnh thăm dò OPTIONS, trả về 204 ngay lập tức
        if (req.method == crow::HTTPMethod::OPTIONS) {
            res.code = 204;
            res.end();
        }
    }
};

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
    // 1.5 ĐỒNG BỘ DỮ LIỆU VÀO MYSQL (TỰ ĐỘNG NẠP CSV VÀO DB)
    // ─────────────────────────────────────────────────────────────
    std::cout << "[1.5] Dang dong bo du lieu CSV vao MySQL...\n";
    DatabaseManager dbSetup;
    // MẬT KHẨU XAMPP THƯỜNG LÀ RỖNG "", ĐỔI TÊN DB THÀNH CỦA M
    if (dbSetup.connect("localhost", "root", "", "recommendation_db", 3306)) {
        dbSetup.createTables(); // Tự động tạo bảng nếu chưa có
        
        // Bơm toàn bộ sản phẩm vào DB
        for (int i = 0; i < items.size(); ++i) {
            dbSetup.insertItem(items[i]);
        }
        std::cout << "   => Nap thanh cong " << items.size() << " san pham vao Database!\n\n";
    } else {
        std::cout << "   => LOI: Khong the ket noi MySQL. Kiem tra lai XAMPP!\n\n";
    }          

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

    UserManager userMgr(users);
    ItemManager itemMgr(items);
    InteractionManager interactMgr(items);

    // ─────────────────────────────────────────────────────────────
    // 3. KHỞI TẠO VÀ CHẠY API SERVER SONG SONG
    // ─────────────────────────────────────────────────────────────
    crow::App<cors_middleware> app;
    app.loglevel(crow::LogLevel::Warning);

    CROW_ROUTE(app, "/recommend/<string>")([&engine](std::string userId){
        auto results = engine.forUser(userId, 20); // Lấy top 20
        
        crow::json::wvalue res;
        res["status"] = "success";
        res["user_id"] = userId;
        
        crow::json::wvalue::list items_list;
        for (const auto& pair : results) {
            items_list.push_back(pair.first); // Lấy Item ID
        }
        res["recommendations"] = std::move(items_list);
        
        crow::response r(res);
        return r;
    });

    CROW_ROUTE(app, "/products")([](){
    DatabaseManager dbManager; 
    dbManager.connect("localhost", "root", "", "recommendation_db", 3306);
    std::cout << "[LOG] Co nguoi dang goi API /products" << std::endl;
    
    std::vector<Item> items = dbManager.loadItems(); 
    
    crow::json::wvalue x;
    crow::json::wvalue::list items_list;
    for (size_t i = 0; i < items.size(); ++i) {
        crow::json::wvalue item;
        // Đã sửa thành hàm get()
        item["id"] = items[i].getItemId(); 
        item["name"] = items[i].getName();
        item["category"] = items[i].getCategory();
        item["price"] = items[i].getPrice();
        items_list.push_back(item);
    }
    x["products"] = std::move(items_list);
    
    crow::response res(x);
    return res;
});

CROW_ROUTE(app, "/trending")([&interactions](){
        std::cout << "[LOG] Co nguoi dang goi API /trending. Dang tinh toan xu huong..." << std::endl;
        
        // 1. Tính tổng điểm Trending cho từng Item
        // Công thức: 1 Click = 1đ, 1 Add Cart = 2đ, 1 Purchase = 3đ
        MyMap<std::string, int> scores;
        for (int i = 0; i < interactions.size(); ++i) {
            std::string itemId = interactions[i].item_id;
            int score = (interactions[i].click_count * 1) + 
                        (interactions[i].add_cart_count * 2) + 
                        (interactions[i].purchase_count * 3);
            
            int currentScore = 0;
            scores.find(itemId, currentScore); // Lấy điểm hiện tại (nếu có)
            scores[itemId] = currentScore + score; // Cộng dồn
        }

        // 2. Đổ dữ liệu ra mảng để chuẩn bị xếp hạng
        MyVector<std::string> keys = scores.keys();
        MyVector<MyPair<std::string, int>> sortedList;
        for (int i = 0; i < keys.size(); ++i) {
            int val = 0;
            scores.find(keys[i], val);
            sortedList.push_back(MyPair<std::string, int>(keys[i], val));
        }

        // 3. Sắp xếp giảm dần (Bubble Sort)
        for (int i = 0; i < sortedList.size(); ++i) {
            for (int j = i + 1; j < sortedList.size(); ++j) {
                if (sortedList[i].second < sortedList[j].second) {
                    MyPair<std::string, int> temp = sortedList[i];
                    sortedList[i] = sortedList[j];
                    sortedList[j] = temp;
                }
            }
        }

        // 4. Cắt lấy đúng TOP 20 cao điểm nhất
        crow::json::wvalue res;
        res["status"] = "success";
        crow::json::wvalue::list items_list;
        
        int limit = sortedList.size() > 20 ? 20 : sortedList.size();
        for (int i = 0; i < limit; ++i) {
            items_list.push_back(sortedList[i].first); // Chỉ đẩy Item ID lên React
        }
        res["trending_items"] = std::move(items_list);
        
        crow::response r(res);
        return r;
    });

    // ─────────────────────────────────────────────────────────────
// 1. API ĐĂNG KÝ (REGISTER) - BỌC GIÁP CORS
// ─────────────────────────────────────────────────────────────
CROW_ROUTE(app, "/register").methods(crow::HTTPMethod::POST, crow::HTTPMethod::OPTIONS)([&userMgr](const crow::request& req){
    // XỬ LÝ LỆNH THĂM DÒ (OPTIONS)
    if (req.method == crow::HTTPMethod::OPTIONS) {
        std::cout << ">>> [DEBUG] SERVER DA NHAN LENH OPTIONS MOI NHAT!" << std::endl;
        crow::response r(204);

        r.add_header("Access-Control-Allow-Methods", "POST, OPTIONS");
        r.add_header("Access-Control-Allow-Headers", "Content-Type, Authorization, Accept");
        return r;
    }

    auto body = crow::json::load(req.body);
    crow::json::wvalue res;
    
    // TRƯỜNG HỢP LỖI
    if (!body) {
        res["status"] = "error";
        res["message"] = "Du lieu khong hop le";
        crow::response r(400, res);
        return r;
    }

    // Lấy dữ liệu an toàn
    std::string name = "";
    std::string email = "";
    std::string password = "";

    if (body.has("name")) name = body["name"].s();
    if (body.has("email")) email = body["email"].s();
    if (body.has("password")) password = body["password"].s();

    // Gọi UserManager để xử lý đăng ký
    MyPair<bool, std::string> result = userMgr.Register(name, email, password);
    
    if (result.first) {
        res["status"] = "success";
        res["user_id"] = result.second; // result.second chính là mã U01x

        // ========================================================
        // ĐOẠN THÊM MỚI: GHI USER VÀO FILE USERS.CSV
        // ========================================================
        // 1. Lấy ngày tháng hiện tại (Format: YYYY-MM-DD)
        std::time_t t = std::time(nullptr);
        std::tm* now = std::localtime(&t);
        std::stringstream ss;
        ss << (now->tm_year + 1900) << "-"
           << std::setfill('0') << std::setw(2) << (now->tm_mon + 1) << "-"
           << std::setfill('0') << std::setw(2) << now->tm_mday;
        std::string currentDate = ss.str();

        // 2. Mở file users.csv để ghi thêm (append)
        std::ofstream file("4_dataset/users.csv", std::ios::app);
        if (file.is_open()) {
            // SỬA LẠI DÒNG NÀY: Ghi đủ 5 cột (user_id, name, created_at, email, password)
            file << result.second << "," << name << "," << currentDate << "," << email << "," << password << "\n";
            
            file.close();
            std::cout << "[+] Da luu thong tin user " << result.second << " vao file users.csv\n";
        } else {
            std::cout << "[-] Loi: Khong the mo users.csv de ghi!\n";
        }
        // ========================================================

    } else {
        res["status"] = "error";
        res["message"] = result.second; 
    }

    // TRƯỜNG HỢP THÀNH CÔNG
    crow::response r(res);
    return r;
});

CROW_ROUTE(app, "/login").methods(crow::HTTPMethod::POST, crow::HTTPMethod::OPTIONS)([&userMgr](const crow::request& req){
    // 1. XỬ LÝ LỆNH THĂM DÒ (OPTIONS) BẮT BUỘC CHO REACT
    if (req.method == crow::HTTPMethod::OPTIONS) {
        std::cout << ">>> [DEBUG] LOGIN OPTIONS CALL" << std::endl;
        crow::response r(204);
        r.add_header("Access-Control-Allow-Methods", "POST, OPTIONS");
        r.add_header("Access-Control-Allow-Headers", "Content-Type, Authorization, Accept");
        return r;
    }

    auto body = crow::json::load(req.body);
    crow::json::wvalue res;
    
    // 2. BẮT LỖI BODY RỖNG
    if (!body) {
        res["status"] = "error";
        res["message"] = "Du lieu khong hop le";
        crow::response r(400, res);
        return r;
    }

    // 3. LẤY DỮ LIỆU AN TOÀN
    std::string email = "";
    std::string password = "";

    if (body.has("email")) email = body["email"].s();
    if (body.has("password")) password = body["password"].s();

    // 4. GỌI USER_MANAGER ĐỂ CHECK LOGIC ĐĂNG NHẬP
    MyPair<bool, std::string> result = userMgr.Login(email, password);
    
    if (result.first) {
        // Đăng nhập thành công
        res["status"] = "success";
        res["user_id"] = userMgr.getLoggedInUserId(); 
        res["token"] = result.second; // Trả thêm token nếu React cần dùng
        std::cout << "[+] User " << userMgr.getLoggedInUserId() << " dang nhap thanh cong!\n";
    } else {
        // Đăng nhập thất bại
        res["status"] = "error";
        res["message"] = result.second; // Trả về thông báo "Sai mat khau" hoac "Email khong ton tai"
        std::cout << "[-] Dang nhap that bai: " << email << "\n";
    }

    crow::response r(res);
    return r;
});
    // ─────────────────────────────────────────────────────────────
    // 4. API THÊM GIỎ HÀNG (ADD TO CART)
    // ─────────────────────────────────────────────────────────────
    CROW_ROUTE(app, "/cart/add").methods(crow::HTTPMethod::POST, crow::HTTPMethod::OPTIONS)([&interactMgr](const crow::request& req){
        if (req.method == crow::HTTPMethod::OPTIONS) {
            crow::response r(204);
            r.add_header("Access-Control-Allow-Methods", "POST, OPTIONS");
            r.add_header("Access-Control-Allow-Headers", "Content-Type");
            return r;
        }

        auto body = crow::json::load(req.body);
        crow::json::wvalue res;
        
        if (!body) {
            res["status"] = "error";
            crow::response r(400, res);
            
            return r;
        }

        std::string user_id = body["user_id"].s();
        std::string item_id = body["item_id"].s();
        int quantity = body.has("quantity") ? body["quantity"].i() : 1;

        interactMgr.AddToCart(user_id, item_id, quantity);
        
        res["status"] = "success";
        res["message"] = "Da them vao gio hang";

        crow::response r(res);
        
        return r;
    });

    CROW_ROUTE(app, "/logout").methods(crow::HTTPMethod::POST, crow::HTTPMethod::OPTIONS)([&userMgr](const crow::request& req){
    if (req.method == crow::HTTPMethod::OPTIONS) {
        crow::response r(204);
        r.add_header("Access-Control-Allow-Methods", "POST, OPTIONS");
        r.add_header("Access-Control-Allow-Headers", "Content-Type");
        return r;
    }

    auto body = crow::json::load(req.body);
    std::string user_id = "";
if (body.has("user_id")) {
    user_id = body["user_id"].s();
}
    
    bool ok = userMgr.Logout(user_id);
    
    crow::json::wvalue res;
    res["status"] = ok ? "success" : "error";
    res["message"] = ok ? "Dang xuat thanh cong" : "Khong tim thay user";

    crow::response r(res);
    return r;
});

CROW_ROUTE(app, "/product/<string>").methods(crow::HTTPMethod::GET, crow::HTTPMethod::OPTIONS)([&itemMgr](const crow::request& req, std::string itemId){
    if (req.method == crow::HTTPMethod::OPTIONS) {
        crow::response r(204);
        r.add_header("Access-Control-Allow-Methods", "GET, OPTIONS");
        r.add_header("Access-Control-Allow-Headers", "Content-Type");
        return r;
    }

    Item detail;
    crow::json::wvalue res;

    if (itemMgr.GetProductDetail(itemId, detail)) {
        res["status"] = "success";
        
        crow::json::wvalue prod;
        // Đã sửa thành hàm get()
        prod["id"] = detail.getItemId(); 
        prod["name"] = detail.getName();
        prod["category"] = detail.getCategory();
        prod["price"] = detail.getPrice();
        
        res["product"] = std::move(prod);

        crow::response r(res);
        return r;
    } else {
        res["status"] = "error";
        res["message"] = "Product not found";
        
        crow::response r(404, res);
        return r;
    }
});

CROW_ROUTE(app, "/cart/remove").methods(crow::HTTPMethod::POST, crow::HTTPMethod::OPTIONS)([&interactMgr](const crow::request& req){
    if (req.method == crow::HTTPMethod::OPTIONS) {
        crow::response r(204);
        r.add_header("Access-Control-Allow-Methods", "POST, OPTIONS");
        r.add_header("Access-Control-Allow-Headers", "Content-Type");
        return r;
    }

    auto body = crow::json::load(req.body);
    std::string user_id = body["user_id"].s();
    std::string item_id = body["item_id"].s();

    // Lưu ý: Thay "RemoveFromCart" bằng tên hàm thực tế trong InteractionManager của m
    interactMgr.RemoveFromCart(user_id, item_id); 
    
    crow::json::wvalue res;
    res["status"] = "success";
    res["message"] = "Da xoa san pham khoi gio";

    crow::response r(res);
    return r;
});

CROW_ROUTE(app, "/cart/get").methods(crow::HTTPMethod::POST, crow::HTTPMethod::OPTIONS)([&interactMgr](const crow::request& req){
    // 1. XỬ LÝ LỆNH THĂM DÒ CORS
    if (req.method == crow::HTTPMethod::OPTIONS) {
        crow::response r(204);
        r.add_header("Access-Control-Allow-Methods", "POST, OPTIONS");
        r.add_header("Access-Control-Allow-Headers", "Content-Type");
        return r;
    }

    auto body = crow::json::load(req.body);
    crow::json::wvalue res;

    // Bắt lỗi an toàn để server không sập nếu React gửi thiếu data
    if (!body || !body.has("user_id")) {
        res["status"] = "error";
        res["message"] = "Thieu user_id";
        crow::response r(400, res);
        return r;
    }

    std::string user_id = body["user_id"].s();

    // Lấy thông tin giỏ hàng (Trả về MyPair<MyVector<CartItem>, double>)
    auto cartInfo = interactMgr.GetCartDetails(user_id);
    
    res["status"] = "success";
    
    // 2. VÒNG LẶP ĐỔ DỮ LIỆU C++ VÀO JSON CHO REACT
    crow::json::wvalue::list items_list;
    for (int i = 0; i < cartInfo.first.size(); ++i) {
        crow::json::wvalue item;
        item["item_id"] = cartInfo.first[i].item_id; 
        item["name"] = cartInfo.first[i].item_name;
        item["price"] = cartInfo.first[i].price;
        item["quantity"] = cartInfo.first[i].quantity;
        item["subtotal"] = cartInfo.first[i].subtotal;
        items_list.push_back(item);
    }
    
    // Đẩy mảng items và tổng tiền vào response
    res["items"] = std::move(items_list); 
    res["total"] = cartInfo.second; 

    crow::response r(res);
    return r;
});

CROW_ROUTE(app, "/track").methods(crow::HTTPMethod::POST, crow::HTTPMethod::OPTIONS)([&interactMgr](const crow::request& req){
    // Xử lý CORS thăm dò
    if (req.method == crow::HTTPMethod::OPTIONS) {
        crow::response r(204);
        r.add_header("Access-Control-Allow-Methods", "POST, OPTIONS");
        r.add_header("Access-Control-Allow-Headers", "Content-Type");
        return r;
    }

    auto body = crow::json::load(req.body);
    crow::json::wvalue res;

    if (!body) {
        res["status"] = "error";
        res["message"] = "Du lieu khong hop le";
        crow::response r(400, res);
        return r;
    }

    // Lấy dữ liệu an toàn
    std::string user_id = body.has("user_id") ? std::string(body["user_id"].s()) : "";
    std::string item_id = body.has("item_id") ? std::string(body["item_id"].s()) : "";
    std::string action = body.has("action") ? std::string(body["action"].s()) : "click"; // click, add_cart, purchase

    if (user_id != "" && item_id != "") {
        // Ghi nhận vào hệ thống C++ (lưu vào Database/RAM)
        interactMgr.AddInteraction(user_id, item_id, action);
        res["status"] = "success";
        res["message"] = "Da luu tracking";
    } else {
        res["status"] = "error";
        res["message"] = "Thieu user_id hoac item_id";
    }

    crow::response r(res);
    return r;
});

// API TẠO ĐƠN HÀNG
CROW_ROUTE(app, "/order/create").methods(crow::HTTPMethod::POST, crow::HTTPMethod::OPTIONS)([&interactMgr](const crow::request& req){
    if (req.method == crow::HTTPMethod::OPTIONS) {
        crow::response r(204);
        r.add_header("Access-Control-Allow-Methods", "POST, OPTIONS");
        r.add_header("Access-Control-Allow-Headers", "Content-Type");
        return r;
    }
    
    auto body = crow::json::load(req.body);
    crow::json::wvalue res;

    if (!body || !body.has("user_id")) {
        res["status"] = "error";
        res["message"] = "Thieu user_id";
        crow::response r(400, res);
        return r;
    }

    std::string user_id = body["user_id"].s();
    
    // TRƯỚC KHI TẠO ĐƠN, HÃY KIỂM TRA GIỎ HÀNG THỰC TẾ
    auto cartInfo = interactMgr.GetCartDetails(user_id);
    if (cartInfo.first.size() == 0) {
        res["status"] = "error";
        res["message"] = "Gio hang trong, khong the tao don!";
        return crow::response(res);
    }

    // 1. Tạo đơn hàng
    MyPair<bool, std::string> orderResult = interactMgr.CreateOrder(user_id, "COD");

    if (orderResult.first) {
        // 2. TRACKING TỰ ĐỘNG
        for (int i = 0; i < cartInfo.first.size(); ++i) {
            interactMgr.AddInteraction(user_id, cartInfo.first[i].item_id, "purchase");
            std::cout << "[DEBUG] Da them vao RAM: User " << user_id << " | Item " << cartInfo.first[i].item_id << std::endl;
        }

        // 3. XÓA GIỎ HÀNG SAU KHI MUA THÀNH CÔNG
        // Nếu m có hàm này, hãy gọi nó để giỏ hàng trống sau khi mua
        // interactMgr.ClearCart(user_id); 

        res["status"] = "success";
        res["order_id"] = orderResult.second;
    } else {
        res["status"] = "error";
        res["message"] = "He thong tu choi tao don hang";
    }

    crow::response r(res);
    return r;
});
    // Chạy server API trên luồng riêng để không làm khựng phần Demo bên dưới
    std::thread server_thread([&](){ app.port(8080).multithreaded().run(); });
    // server_thread.detach();

    std::cout << ">>> Server API da khoi dong tai http://localhost:8080/recommend/<user_id>\n\n";

   

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
