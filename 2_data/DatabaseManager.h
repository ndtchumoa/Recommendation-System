#pragma once

#include <mysql.h>

#include <string>
#include <vector>

#include "../1_models/User.h"
#include "../1_models/Item.h"
#include "../1_models/Interaction.h"

/**
 * @class DatabaseManager
 * @brief Tầng hạ tầng I/O – kết nối và đồng bộ dữ liệu với MySQL Server.
 *
 * Lớp này đóng vai trò tương tự CSVLoader nhưng nguồn/đích dữ liệu là
 * một MySQL database thay vì file CSV. Dùng chung các struct User, Item,
 * Interaction nên có thể đổi qua lại giữa CSV <-> Database dễ dàng.
 *
 * ── Schema kỳ vọng (được tự tạo bởi createTables()) ─────────────────
 *   users        : user_id (PK), name, created_at
 *   items        : item_id (PK), name, category, price
 *   interactions : id (PK, auto_increment), user_id (FK), item_id (FK),
 *                   click, add_cart, purchase, rating
 * ─────────────────────────────────────────────────────────────────────
 */
class DatabaseManager
{
private:
    MYSQL* conn;

    /**
     * @brief Escape một chuỗi để chèn an toàn vào câu lệnh SQL
     *        (chống SQL Injection cho các giá trị string).
     *
     * Dùng mysql_real_escape_string(). Nếu chưa kết nối, trả về chuỗi
     * gốc không đổi (caller nên kiểm tra isConnected() trước).
     */
    std::string escapeString(const std::string& s) const;

    /**
     * @brief Chuyển double -> chuỗi số dùng được trong SQL
     *        (tránh kiểu "5.000000" dài dòng của std::to_string).
     */
    static std::string doubleToSql(double value);

public:
    // Constructor & Destructor
    DatabaseManager();
    ~DatabaseManager();

    // =============================
    // Kết nối Database
    // =============================
    bool connect(
        const std::string& host,
        const std::string& user,
        const std::string& password,
        const std::string& database,
        unsigned int port = 3306);

    void disconnect();

    bool isConnected() const;

    MYSQL* getConnection();

    // =============================
    // Khởi tạo schema
    // =============================
    /**
     * @brief Tạo các bảng users, items, interactions nếu chưa tồn tại.
     * @return true nếu cả 3 bảng được tạo/đã tồn tại thành công.
     */
    bool createTables();

    // =============================
    // Đọc dữ liệu
    // =============================
    std::vector<User> loadUsers();

    std::vector<Item> loadItems();

    std::vector<Interaction> loadInteractions();

    // =============================
    // Thêm dữ liệu
    // =============================
    // insertUser/insertItem dùng "ON DUPLICATE KEY UPDATE" nên có thể
    // gọi lại nhiều lần (import lại CSV) mà không bị lỗi trùng khóa.
    bool insertUser(const User& user);

    bool insertItem(const Item& item);

    // interactions không có khóa duy nhất ngoài id tự tăng, nên mỗi lần
    // import sẽ thêm dòng mới (giống log lịch sử tương tác).
    bool insertInteraction(
        const Interaction& interaction);

    // =============================
    // Xóa dữ liệu
    // =============================
    bool deleteUser(
        const std::string& user_id);

    bool deleteItem(
        const std::string& item_id);

    bool deleteInteraction(
        const std::string& user_id,
        const std::string& item_id);

    // =============================
    // SQL tùy ý
    // =============================
    bool executeQuery(
        const std::string& query);
};