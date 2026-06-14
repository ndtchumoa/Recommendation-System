#include "ItemManager.h"
#include <iostream>
#include <cctype> // for tolower

// Helper: chuyển chuỗi thành chữ thường (để so khớp không phân biệt hoa/thường)
static std::string toLowerStr(const std::string& str) {
    std::string result = str;
    for (size_t i = 0; i < result.length(); ++i) {
        result[i] = std::tolower(result[i]);
    }
    return result;
}

// --- Constructor ---
ItemManager::ItemManager(const MyVector<Item>& loaded_items) {
    items = loaded_items;
}

// --- 1. GetHomeProducts: Lấy danh sách trang chủ ---
MyVector<Item> ItemManager::GetHomeProducts(const std::string& userId, int limit) const {
    MyVector<Item> result;

    // Hiện tại: lấy tất cả SP và giới hạn số lượng
    // Có thể mở rộng: gợi ý theo userId (dùng Recommender)
    int count = 0;
    for (int i = 0; i < items.size() && count < limit; ++i) {
        result.push_back(items[i]);
        ++count;
    }

    return result;
}

// --- 2. SearchProducts: Tìm kiếm sản phẩm theo từ khóa ---
MyVector<Item> ItemManager::SearchProducts(const std::string& keyword) const {
    MyVector<Item> result;

    if (keyword.empty()) return result;

    std::string lowerKeyword = toLowerStr(keyword);

    for (int i = 0; i < items.size(); ++i) {
        std::string lowerName = toLowerStr(items[i].getName());
        // Kiểm tra từ khóa có xuất hiện trong tên sản phẩm không
        if (lowerName.find(lowerKeyword) != std::string::npos) {
            result.push_back(items[i]);
        }
    }

    return result;
}

// --- 3. FilterAndSortProducts: Lọc & Sắp xếp sản phẩm ---
MyVector<Item> ItemManager::FilterAndSortProducts(const std::string& category,
                                                    const std::string& badge,
                                                    double minStar,
                                                    const std::string& sortBy) const {
    MyVector<Item> result;

    // Bước 1: Lọc theo điều kiện
    for (int i = 0; i < items.size(); ++i) {
        const Item& item = items[i];

        // Lọc theo category
        if (!category.empty() && item.getCategory() != category) continue;

        // Lọc theo badge
        if (!badge.empty() && item.getBadge() != badge) continue;

        // Lọc theo minStar
        if (item.getStar() < minStar) continue;

        result.push_back(item);
    }

    // Bước 2: Sắp xếp (Bubble Sort)
    if (sortBy == "price_asc") {
        // Giá tăng dần
        for (int i = 0; i < result.size() - 1; ++i) {
            for (int j = 0; j < result.size() - i - 1; ++j) {
                if (result[j].getPrice() > result[j + 1].getPrice()) {
                    Item temp = result[j];
                    result[j] = result[j + 1];
                    result[j + 1] = temp;
                }
            }
        }
    } else if (sortBy == "price_desc") {
        // Giá giảm dần
        for (int i = 0; i < result.size() - 1; ++i) {
            for (int j = 0; j < result.size() - i - 1; ++j) {
                if (result[j].getPrice() < result[j + 1].getPrice()) {
                    Item temp = result[j];
                    result[j] = result[j + 1];
                    result[j + 1] = temp;
                }
            }
        }
    } else if (sortBy == "star") {
        // Đánh giá cao nhất
        for (int i = 0; i < result.size() - 1; ++i) {
            for (int j = 0; j < result.size() - i - 1; ++j) {
                if (result[j].getStar() < result[j + 1].getStar()) {
                    Item temp = result[j];
                    result[j] = result[j + 1];
                    result[j + 1] = temp;
                }
            }
        }
    } else if (sortBy == "sold") {
        // Bán chạy nhất
        for (int i = 0; i < result.size() - 1; ++i) {
            for (int j = 0; j < result.size() - i - 1; ++j) {
                if (result[j].getSold() < result[j + 1].getSold()) {
                    Item temp = result[j];
                    result[j] = result[j + 1];
                    result[j + 1] = temp;
                }
            }
        }
    }
    // Nếu sortBy rỗng: giữ nguyên thứ tự

    return result;
}

// --- 4. GetProductDetail: Xem chi tiết sản phẩm ---
bool ItemManager::GetProductDetail(const std::string& productId, Item& out) const {
    for (int i = 0; i < items.size(); ++i) {
        if (items[i].getItemId() == productId) {
            out = items[i];
            return true;
        }
    }
    return false;
}

// --- Hỗ trợ ---
const MyVector<Item>& ItemManager::getAllItems() const {
    return items;
}