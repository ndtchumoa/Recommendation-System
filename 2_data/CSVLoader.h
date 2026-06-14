#ifndef CSV_LOADER_H
#define CSV_LOADER_H

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

// Nhúng các cấu trúc dữ liệu tự cài đặt (thay thế STL)
#include "MyDataStructures.h"

// Nhúng các thực thể đã tạo từ tầng Models
#include "../1_models/User.h"
#include "../1_models/Item.h"
#include "../1_models/Interaction.h"

<<<<<<< HEAD
class CSVLoader {
=======
/**
 * @class CSVLoader
 * @brief Tầng hạ tầng I/O – nạp/ghi dữ liệu giữa file CSV và bộ nhớ.
 *
 * Lớp này chịu hoàn toàn trách nhiệm đọc/viết file, tách cột (parse),
 * chuyển kiểu dữ liệu và trả về/nhận vector của các thực thể tương ứng.
 * Các lớp thuật toán (RatingMatrix, Recommender...) sẽ nhận dữ liệu
 * đã được parse sạch từ đây.
 *
 * Các hàm save*() dùng cho luồng "Export DB ra CSV": dữ liệu đọc từ
 * DatabaseManager (vector<User>/<Item>/<Interaction>) có thể được ghi
 * lại thành file CSV với cùng định dạng mà loadUsers/loadItems/
 * loadInteractions() đọc vào, nên có thể nạp lại bằng chính CSVLoader.
 *
 * ── Định dạng tệp ──────────────────────────────────────────────────
 *  users.csv       : user_id, name, created_at
 *  items.csv       : item_id, name, category, price
 *  interactions.csv: user_id, item_id, click, add_cart, purchase, rating
 * ───────────────────────────────────────────────────────────────────
 *
 * Ví dụ sử dụng:
 * @code
 *   CSVLoader loader;
 *   auto users    = loader.loadUsers("datasets/users.csv");
 *   auto items    = loader.loadItems("datasets/items.csv");
 *   auto interact = loader.loadInteractions("datasets/interactions.csv");
 *
 *   // Export ngược lại CSV (vd: sau khi loadUsers() từ DatabaseManager)
 *   loader.saveUsers("datasets/export_users.csv", users);
 * @endcode
 */
class CSVLoader {
public:
    CSVLoader() = default;

    /**
     * @brief Nạp danh sách người dùng từ tệp CSV.
     * @param filepath  Đường dẫn tới tệp users.csv
     * @return vector<User> chứa tất cả người dùng đã parse.
     * @throws std::runtime_error nếu không mở được file.
     */
    std::vector<User> loadUsers(const std::string& filepath) const;

    /**
     * @brief Nạp danh sách sản phẩm từ tệp CSV.
     * @param filepath  Đường dẫn tới tệp items.csv
     * @return vector<Item> chứa tất cả sản phẩm đã parse.
     * @throws std::runtime_error nếu không mở được file.
     */
    std::vector<Item> loadItems(const std::string& filepath) const;

    /**
     * @brief Nạp lịch sử tương tác từ tệp CSV.
     * @param filepath  Đường dẫn tới tệp interactions.csv
     * @return vector<Interaction> chứa tất cả bản ghi tương tác đã parse.
     * @throws std::runtime_error nếu không mở được file.
     */
    std::vector<Interaction> loadInteractions(const std::string& filepath) const;

    // ─────────────────────────────────────────────────────────────────
    // Ghi dữ liệu ra CSV (dùng cho luồng Export DB -> CSV)
    // ─────────────────────────────────────────────────────────────────

    /**
     * @brief Ghi danh sách User ra tệp CSV (có dòng header).
     * @param filepath Đường dẫn tệp đích (sẽ bị ghi đè nếu đã tồn tại).
     * @param users    Danh sách người dùng cần ghi.
     * @return true nếu ghi thành công.
     */
    bool saveUsers(const std::string& filepath,
                   const std::vector<User>& users) const;

    /**
     * @brief Ghi danh sách Item ra tệp CSV (có dòng header).
     */
    bool saveItems(const std::string& filepath,
                  const std::vector<Item>& items) const;

    /**
     * @brief Ghi danh sách Interaction ra tệp CSV (có dòng header).
     */
    bool saveInteractions(const std::string& filepath,
                          const std::vector<Interaction>& interactions) const;

>>>>>>> b8c1ae64c94847b04dee00ae1cc482c9f316487e
private:
    MyVector<User> users;
    MyVector<Item> items;
    MyVector<Interaction> interactions;

<<<<<<< HEAD
    // Hàm hỗ trợ để cắt khoảng trắng thừa (trim) nếu dữ liệu CSV bị lỗi format
    std::string trim(const std::string& str);

public:
    CSVLoader();

    // 1. Các hàm đọc dữ liệu từ file
    bool loadUsers(const std::string& filepath);
    bool loadItems(const std::string& filepath);
    bool loadInteractions(const std::string& filepath);

    // 2. Getters — trả về tham chiếu hằng để tránh sao chép
    const MyVector<User>& getUsers() const;
    const MyVector<Item>& getItems() const;
    const MyVector<Interaction>& getInteractions() const;
};

#endif // CSV_LOADER_H
=======
    /**
     * @brief Loại bỏ khoảng trắng và ký tự xuống dòng ở đầu/cuối chuỗi.
     * @param s  Chuỗi cần trim.
     * @return Chuỗi đã được trim.
     */
    std::string trim(const std::string& s) const;

    /**
     * @brief Chuẩn bị một giá trị để ghi an toàn vào một ô CSV.
     *
     * Nếu giá trị chứa dấu phẩy, dấu ngoặc kép hoặc ký tự xuống dòng,
     * hàm sẽ bọc giá trị trong dấu nháy kép và escape các dấu nháy kép
     * bên trong (theo chuẩn RFC 4180), tương ứng với cách parseLine()
     * đọc ngược lại.
     */
    std::string escapeField(const std::string& field) const;
};
>>>>>>> b8c1ae64c94847b04dee00ae1cc482c9f316487e
