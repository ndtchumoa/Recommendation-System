#pragma once
#include <string>
#include <vector>
#include <stdexcept>

// Forward-include các thực thể (entity) cần nạp
#include "../1_models/User.h"
#include "../1_models/Item.h"
#include "../1_models/Interaction.h"

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

private:
    /**
     * @brief Tách một dòng CSV thành các token theo dấu phẩy.
     *
     * Hỗ trợ trường hợp giá trị được bọc trong dấu nháy kép (quoted fields)
     * để xử lý các trường chứa dấu phẩy bên trong (vd: tên sản phẩm).
     *
     * @param line   Dòng văn bản thô từ file CSV.
     * @return vector<string> danh sách các token đã tách.
     */
    std::vector<std::string> parseLine(const std::string& line) const;

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