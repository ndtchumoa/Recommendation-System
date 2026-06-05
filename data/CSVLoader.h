#pragma once
#include <string>
#include <vector>
#include <stdexcept>

// Forward-include các thực thể (entity) cần nạp
#include "../models/User.h"
#include "../models/Item.h"
#include "../models/Interaction.h"

/**
 * @class CSVLoader
 * @brief Tầng hạ tầng I/O – nạp dữ liệu từ các tệp CSV vào bộ nhớ.
 *
 * Lớp này chịu hoàn toàn trách nhiệm đọc file, tách cột (parse),
 * chuyển kiểu dữ liệu và trả về vector của các thực thể tương ứng.
 * Các lớp thuật toán (RatingMatrix, Recommender...) sẽ nhận dữ liệu
 * đã được parse sạch từ đây.
 *
 * ── Định dạng tệp đầu vào ──────────────────────────────────────────
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
};
