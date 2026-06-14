#include "DatabaseManager.h"
#include <iostream>
#include <sstream>
#include <vector>
#include <cstring>

DatabaseManager::DatabaseManager()
{
    conn = nullptr;
}

DatabaseManager::~DatabaseManager()
{
    disconnect();
}

// ─────────────────────────────────────────────────────────────────────
// Kết nối / Ngắt kết nối
// ─────────────────────────────────────────────────────────────────────
bool DatabaseManager::connect(
    const std::string& host,
    const std::string& user,
    const std::string& password,
    const std::string& database,
    unsigned int port)
{
    // Đảm bảo không leak nếu connect() được gọi lại khi đã có kết nối cũ
    disconnect();

    MYSQL* handle = mysql_init(nullptr);
    if (!handle)
    {
        std::cerr << "[DatabaseManager] mysql_init() that bai.\n";
        return false;
    }

    MYSQL* result = mysql_real_connect(
        handle,
        host.c_str(),
        user.c_str(),
        password.c_str(),
        database.c_str(),
        port,
        nullptr,
        0);

    if (!result)
    {
        // Quan trọng: handle do mysql_init() cấp phát phải được giải
        // phóng nếu mysql_real_connect() thất bại, tránh leak handle.
        std::cerr << "[DatabaseManager] Connect Error: "
                  << mysql_error(handle) << '\n';
        mysql_close(handle);
        conn = nullptr;
        return false;
    }

    conn = result; // == handle khi thành công
    return true;
}

void DatabaseManager::disconnect()
{
    if (conn)
    {
        mysql_close(conn);
        conn = nullptr;
    }
}

bool DatabaseManager::isConnected() const
{
    return conn != nullptr;
}

MYSQL* DatabaseManager::getConnection()
{
    return conn;
}

// ─────────────────────────────────────────────────────────────────────
// Helpers nội bộ
// ─────────────────────────────────────────────────────────────────────
std::string DatabaseManager::escapeString(const std::string& s) const
{
    if (!conn) return s; // không có connection để escape -> trả nguyên

    // Theo doc MySQL: buffer cần tối thiểu (length * 2 + 1) bytes
    std::vector<char> buffer(s.size() * 2 + 1);

    unsigned long len = mysql_real_escape_string(
        conn, buffer.data(), s.c_str(), (unsigned long)s.size());

    return std::string(buffer.data(), len);
}

std::string DatabaseManager::doubleToSql(double value)
{
    std::ostringstream oss;
    oss << value; // dùng định dạng mặc định, gọn hơn to_string("5.000000")
    return oss.str();
}

// ─────────────────────────────────────────────────────────────────────
// Khởi tạo schema
// ─────────────────────────────────────────────────────────────────────
bool DatabaseManager::createTables()
{
    if (!conn) return false;

    const char* createUsers =
        "CREATE TABLE IF NOT EXISTS users ("
        "  user_id    VARCHAR(20)  PRIMARY KEY,"
        "  name       VARCHAR(100) NOT NULL,"
        "  created_at DATE"
        ") ENGINE=InnoDB DEFAULT CHARSET=utf8mb4";

    const char* createItems =
        "CREATE TABLE IF NOT EXISTS items ("
        "  item_id  VARCHAR(20)  PRIMARY KEY,"
        "  name     VARCHAR(150) NOT NULL,"
        "  category VARCHAR(50),"
        "  price    DOUBLE NOT NULL DEFAULT 0"
        ") ENGINE=InnoDB DEFAULT CHARSET=utf8mb4";

    const char* createInteractions =
        "CREATE TABLE IF NOT EXISTS interactions ("
        "  id        INT AUTO_INCREMENT PRIMARY KEY,"
        "  user_id   VARCHAR(20) NOT NULL,"
        "  item_id   VARCHAR(20) NOT NULL,"
        "  click     INT NOT NULL DEFAULT 0,"
        "  add_cart  INT NOT NULL DEFAULT 0,"
        "  purchase  INT NOT NULL DEFAULT 0,"
        "  rating    DOUBLE NOT NULL DEFAULT 0,"
        "  FOREIGN KEY (user_id) REFERENCES users(user_id)"
        "    ON DELETE CASCADE ON UPDATE CASCADE,"
        "  FOREIGN KEY (item_id) REFERENCES items(item_id)"
        "    ON DELETE CASCADE ON UPDATE CASCADE"
        ") ENGINE=InnoDB DEFAULT CHARSET=utf8mb4";

    // Thứ tự quan trọng: users, items phải tạo trước interactions vì
    // interactions có FOREIGN KEY tham chiếu tới 2 bảng này.
    bool ok = true;
    ok &= executeQuery(createUsers);
    ok &= executeQuery(createItems);
    ok &= executeQuery(createInteractions);

    if (ok)
        std::cout << "[DatabaseManager] Da kiem tra/tao xong cac bang "
                     "users, items, interactions.\n";

    return ok;
}

// ─────────────────────────────────────────────────────────────────────
// Đọc dữ liệu
// ─────────────────────────────────────────────────────────────────────
std::vector<User> DatabaseManager::loadUsers()
{
    std::vector<User> users;

    if (!conn)
        return users;

    // DATE_FORMAT để trả về đúng dạng YYYY-MM-DD giống CSV
    const char* query =
        "SELECT user_id, name, DATE_FORMAT(created_at, '%Y-%m-%d') "
        "FROM users";

    if (mysql_query(conn, query))
    {
        std::cerr << "MySQL Error: "
                  << mysql_error(conn)
                  << '\n';
        return users;
    }

    MYSQL_RES* result = mysql_store_result(conn);

    if (!result)
    {
        std::cerr << "Store Result Error: "
                  << mysql_error(conn)
                  << '\n';
        return users;
    }

    MYSQL_ROW row;

    while ((row = mysql_fetch_row(result)))
    {
        std::string id = row[0] ? row[0] : "";
        std::string name = row[1] ? row[1] : "";
        std::string created_at = row[2] ? row[2] : "";
        
        users.push_back(
            User(
                id,
                name,
                id + "@email.com", // tự động tạo email
                "password123",     // tự động tạo password
                created_at
            )
        );
    }

    mysql_free_result(result);

    return users;
}

std::vector<Item> DatabaseManager::loadItems()
{
    std::vector<Item> items;

    if (!conn)
        return items;

    const char* query =
        "SELECT item_id, name, category, price FROM items";

    if (mysql_query(conn, query))
    {
        std::cerr << "MySQL Error: "
                  << mysql_error(conn)
                  << '\n';
        return items;
    }

    MYSQL_RES* result = mysql_store_result(conn);

    if (!result)
    {
        std::cerr << "Store Result Error: "
                  << mysql_error(conn)
                  << '\n';
        return items;
    }

    MYSQL_ROW row;

    while ((row = mysql_fetch_row(result)))
    {
        std::string id = row[0] ? row[0] : "";
        std::string name = row[1] ? row[1] : "";
        std::string category = row[2] ? row[2] : "";
        double price = row[3] ? std::stod(row[3]) : 0.0;

        // Sinh tự động các tham số còn thiếu cho Item
        std::string badge = "";
        if (price >= 10000000) badge = "Hot";
        else if (category == "Books") badge = "Sale";
        
        double star = 4.0;
        int sold = 100;
        std::string img = category + "/" + id + ".jpg";

        items.push_back(
            Item(
                id,
                name,
                category,
                badge,
                price,
                star,
                sold,
                img
            )
        );
    }

    mysql_free_result(result);

    return items;
}

std::vector<Interaction>
DatabaseManager::loadInteractions()
{
    std::vector<Interaction> interactions;

    if (!conn)
        return interactions;

    const char* query =
        "SELECT user_id,item_id,"
        "click,add_cart,purchase,rating "
        "FROM interactions";

    if (mysql_query(conn, query))
    {
        std::cerr
            << "MySQL Error: "
            << mysql_error(conn)
            << '\n';

        return interactions;
    }

    MYSQL_RES* result =
        mysql_store_result(conn);

    if (!result)
    {
        std::cerr
            << "Store Result Error: "
            << mysql_error(conn)
            << '\n';

        return interactions;
    }

    MYSQL_ROW row;

    while ((row = mysql_fetch_row(result)))
    {
        interactions.push_back(
            Interaction(
                row[0] ? row[0] : "",
                row[1] ? row[1] : "",
                row[2] ? std::stoi(row[2]) : 0,
                row[3] ? std::stoi(row[3]) : 0,
                row[4] ? std::stoi(row[4]) : 0,
                row[5] ? std::stod(row[5]) : 0.0
            )
        );
    }

    mysql_free_result(result);

    return interactions;
}

// ─────────────────────────────────────────────────────────────────────
// Thêm dữ liệu
// ─────────────────────────────────────────────────────────────────────
bool DatabaseManager::insertUser(const User& user)
{
    if (!conn)
        return false;

    // ON DUPLICATE KEY UPDATE: cho phép import lại CSV nhiều lần mà
    // không bị lỗi "Duplicate entry" với user_id đã tồn tại — dữ liệu
    // sẽ được cập nhật theo bản mới nhất.
    std::string query =
        "INSERT INTO users(user_id,name,created_at) VALUES('"
        + escapeString(user.user_id) + "','"
        + escapeString(user.name) + "','"
        + escapeString(user.created_at) + "')"
        " ON DUPLICATE KEY UPDATE"
        " name=VALUES(name), created_at=VALUES(created_at)";

    return executeQuery(query);
}

bool DatabaseManager::insertItem(const Item& item)
{
    if (!conn)
        return false;

    std::string query =
        "INSERT INTO items(item_id,name,category,price) VALUES('"
        + escapeString(item.item_id) + "','"
        + escapeString(item.name) + "','"
        + escapeString(item.category) + "',"
        + doubleToSql(item.price)
        + ")"
        " ON DUPLICATE KEY UPDATE"
        " name=VALUES(name), category=VALUES(category), price=VALUES(price)";

    return executeQuery(query);
}

bool DatabaseManager::insertInteraction(
    const Interaction& interaction)
{
    if (!conn)
        return false;

    std::string query =
        "INSERT INTO interactions("
        "user_id,item_id,click,add_cart,purchase,rating)"
        " VALUES('"
        + escapeString(interaction.user_id) + "','"
        + escapeString(interaction.item_id) + "',"
        + std::to_string(interaction.click_count) + ","
        + std::to_string(interaction.add_cart_count) + ","
        + std::to_string(interaction.purchase_count) + ","
        + doubleToSql(interaction.rating)
        + ")";

    return executeQuery(query);
}

// ─────────────────────────────────────────────────────────────────────
// Xóa dữ liệu
// ─────────────────────────────────────────────────────────────────────
bool DatabaseManager::deleteUser(
    const std::string& user_id)
{
    if (!conn)
        return false;

    std::string query =
        "DELETE FROM users WHERE user_id='"
        + escapeString(user_id) + "'";

    return executeQuery(query);
}

bool DatabaseManager::deleteItem(
    const std::string& item_id)
{
    if (!conn)
        return false;

    std::string query =
        "DELETE FROM items WHERE item_id='"
        + escapeString(item_id) + "'";

    return executeQuery(query);
}

bool DatabaseManager::deleteInteraction(
    const std::string& user_id,
    const std::string& item_id)
{
    if (!conn)
        return false;

    std::string query =
        "DELETE FROM interactions "
        "WHERE user_id='" + escapeString(user_id) +
        "' AND item_id='" + escapeString(item_id) + "'";

    return executeQuery(query);
}

// ─────────────────────────────────────────────────────────────────────
// SQL tùy ý
// ─────────────────────────────────────────────────────────────────────
bool DatabaseManager::executeQuery(
    const std::string& query)
{
    if (!conn)
        return false;

    if (mysql_query(conn, query.c_str()))
    {
        std::cerr
            << "MySQL Error: "
            << mysql_error(conn)
            << '\n';

        return false;
    }

    return true;
}