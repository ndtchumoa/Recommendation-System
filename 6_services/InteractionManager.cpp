#include "InteractionManager.h"
#include <iostream>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <cstdlib>

// --- Constructor ---
InteractionManager::InteractionManager(const MyVector<Item>& loaded_items, const MyVector<Interaction>& loaded_interactions) {    
    items = loaded_items;
    all_interactions = loaded_interactions;
}

// --- Hỗ trợ: Tạo orderId ---
std::string InteractionManager::generateOrderId() const {
    std::stringstream ss;
    ss << "ORD_" << std::time(nullptr) << "_" << (rand() % 1000);
    return ss.str();
}

// --- Hỗ trợ: Lấy thời gian ---
std::string InteractionManager::getCurrentDateTime() const {
    std::time_t t = std::time(nullptr);
    std::tm* now = std::localtime(&t);
    std::stringstream ss;
    ss << (now->tm_year + 1900) << '-'
       << std::setfill('0') << std::setw(2) << (now->tm_mon + 1) << '-'
       << std::setfill('0') << std::setw(2) << now->tm_mday
       << ' '
       << std::setfill('0') << std::setw(2) << now->tm_hour << ':'
       << std::setfill('0') << std::setw(2) << now->tm_min << ':'
       << std::setfill('0') << std::setw(2) << now->tm_sec;
    return ss.str();
}

// --- Hỗ trợ: Tìm sản phẩm theo itemId ---
bool InteractionManager::findItemById(const std::string& itemId, Item& out) const {
    for (int i = 0; i < items.size(); ++i) {
        if (items[i].getItemId() == itemId) {
            out = items[i];
            return true;
        }
    }
    return false;
}

// ====================================================================
//  CART OPERATIONS
// ====================================================================

// --- AddToCart ---
MyPair<bool, std::string> InteractionManager::AddToCart(const std::string& userId,
                                                          const std::string& productId,
                                                          int quantity) {
    if (quantity <= 0) {
        return MyPair<bool, std::string>(false, "So luong phai > 0!");
    }

    // Kiểm tra sản phẩm có tồn tại không
    Item foundItem;
    if (!findItemById(productId, foundItem)) {
        return MyPair<bool, std::string>(false, "San pham khong ton tai!");
    }

    // Lấy giỏ hàng của user (tự động tạo nếu chưa có)
    MyVector<CartItem>& cart = carts[userId];

    // Kiểm tra sản phẩm đã có trong giỏ chưa
    for (int i = 0; i < cart.size(); ++i) {
        if (cart[i].item_id == productId) {
            // Đã có → tăng số lượng
            cart[i].quantity += quantity;
            cart[i].subtotal = cart[i].price * cart[i].quantity;
            std::cout << "[+] Da cap nhat so luong SP " << productId
                      << " trong gio: " << cart[i].quantity << "\n";
            return MyPair<bool, std::string>(true, "Da cap nhat so luong!");
        }
    }

    // Chưa có → thêm mới
    CartItem newItem;
    newItem.item_id = foundItem.getItemId();
    newItem.item_name = foundItem.getName();
    newItem.price = foundItem.getPrice();
    newItem.quantity = quantity;
    newItem.subtotal = newItem.price * newItem.quantity;

    cart.push_back(newItem);
    std::cout << "[+] Da them SP " << productId << " vao gio hang!\n";

    return MyPair<bool, std::string>(true, "Da them vao gio hang!");
}

// --- UpdateCartItem ---
MyPair<double, int> InteractionManager::UpdateCartItem(const std::string& userId,
                                                         const std::string& productId,
                                                         int newQuantity) {
    MyVector<CartItem> cart;
    if (!carts.find(userId, cart)) {
        return MyPair<double, int>(0.0, 0); // Giỏ hàng không tồn tại
    }

    double newTotal = 0.0;
    bool found = false;

    for (int i = 0; i < cart.size(); ++i) {
        if (cart[i].item_id == productId) {
            if (newQuantity <= 0) {
                // Nếu quantity = 0 → xóa khỏi giỏ
                MyVector<CartItem> updatedCart;
                for (int j = 0; j < cart.size(); ++j) {
                    if (j != i) {
                        updatedCart.push_back(cart[j]);
                    }
                }
                carts[userId] = updatedCart;
                cart = updatedCart;
            } else {
                cart[i].quantity = newQuantity;
                cart[i].subtotal = cart[i].price * newQuantity;
                carts[userId] = cart;
            }
            found = true;
            break;
        }
    }

    if (!found) {
        return MyPair<double, int>(0.0, 0);
    }

    // Tính tổng tiền mới
    cart = carts[userId]; // Lấy lại giỏ đã cập nhật
    for (int i = 0; i < cart.size(); ++i) {
        newTotal += cart[i].subtotal;
    }

    return MyPair<double, int>(newTotal, newQuantity);
}

// --- RemoveFromCart ---
bool InteractionManager::RemoveFromCart(const std::string& userId,
                                         const std::string& productId) {
    MyVector<CartItem> cart;
    if (!carts.find(userId, cart)) {
        return false;
    }

    MyVector<CartItem> updatedCart;
    bool removed = false;

    for (int i = 0; i < cart.size(); ++i) {
        if (cart[i].item_id == productId) {
            removed = true;
            std::cout << "[-] Da xoa SP " << productId << " khoi gio hang!\n";
        } else {
            updatedCart.push_back(cart[i]);
        }
    }

    if (removed) {
        carts[userId] = updatedCart;
    }
    return removed;
}

// --- GetCartDetails ---
MyPair<MyVector<CartItem>, double> InteractionManager::GetCartDetails(const std::string& userId) const {
    MyVector<CartItem> cart;
    double total = 0.0;

    if (carts.find(userId, cart)) {
        for (int i = 0; i < cart.size(); ++i) {
            total += cart[i].subtotal;
        }
    }

    return MyPair<MyVector<CartItem>, double>(cart, total);
}

// --- ClearCart ---
void InteractionManager::ClearCart(const std::string& userId) {
    carts[userId] = MyVector<CartItem>(); // Gán giỏ hàng rỗng
    std::cout << "[+] Da xoa toan bo gio hang cua user " << userId << "\n";
}

// ====================================================================
//  ORDER OPERATIONS
// ====================================================================

// --- CreateOrder ---
MyPair<bool, std::string> InteractionManager::CreateOrder(const std::string& userId,
                                                            const std::string& paymentMethod) {
    // Lấy giỏ hàng
    MyVector<CartItem> cart;
    if (!carts.find(userId, cart) || cart.size() == 0) {
        return MyPair<bool, std::string>(false, "Gio hang trong!");
    }

    // Tính tổng tiền
    double total = 0.0;
    for (int i = 0; i < cart.size(); ++i) {
        total += cart[i].subtotal;
    }

    // Tạo đơn hàng
    Order newOrder;
    newOrder.order_id = generateOrderId();
    newOrder.user_id = userId;
    newOrder.items = cart;
    newOrder.total_amount = total;
    newOrder.status = "Pending";
    newOrder.created_at = getCurrentDateTime();
    newOrder.payment_method = paymentMethod;

    // Lưu đơn hàng
    orders[newOrder.order_id] = newOrder;

    // Xóa giỏ hàng
    ClearCart(userId);

    std::cout << "[+] Tao don hang thanh cong! Order ID: " << newOrder.order_id << "\n";
    std::cout << "    Tong tien: " << total << "\n";
    std::cout << "    Phuong thuc thanh toan: " << paymentMethod << "\n";

    return MyPair<bool, std::string>(true, newOrder.order_id);
}

// --- GetOrderDetails ---
bool InteractionManager::GetOrderDetails(const std::string& orderId, Order& out) const {
    return orders.find(orderId, out);
}

// ====================================================================
//  RECOMMENDATION & INTERACTION LOGGING
// ====================================================================

// --- GetSuggestedProducts: Cross-sell dựa trên danh mục SP đã mua ---
MyVector<Item> InteractionManager::GetSuggestedProducts(const std::string& orderId) const {
    MyVector<Item> result;

    Order order;
    if (!orders.find(orderId, order)) {
        return result; // Không tìm thấy đơn hàng
    }

    // Thu thập danh mục từ các SP đã mua trong đơn hàng
    MyMap<std::string, bool> boughtCategories;
    MyMap<std::string, bool> boughtIds;

    for (int i = 0; i < order.items.size(); ++i) {
        boughtIds[order.items[i].item_id] = true;
        Item item;
        if (findItemById(order.items[i].item_id, item)) {
            boughtCategories[item.getCategory()] = true;
        }
    }

    // Tìm các SP cùng danh mục nhưng không phải SP đã mua
    MyVector<std::string> catKeys = boughtCategories.keys();
    for (int c = 0; c < catKeys.size(); ++c) {
        std::string category = catKeys[c];
        for (int i = 0; i < items.size(); ++i) {
            if (items[i].getCategory() == category) {
                // Bỏ qua SP đã mua
                if (!boughtIds.contains(items[i].getItemId())) {
                    result.push_back(items[i]);
                }
            }
            if (result.size() >= 5) break; // Giới hạn 5 SP gợi ý
        }
        if (result.size() >= 5) break;
    }

    return result;
}

// --- AddInteraction: Ghi log tương tác ra console (mô phỏng) ---
// --- AddInteraction: Lưu vào RAM (all_interactions) ---
// --- AddInteraction: Lưu vào RAM (all_interactions) ---
void InteractionManager::AddInteraction(const std::string& userId,
                                         const std::string& itemId,
                                         const std::string& type) {
    // 1. In Log ra màn hình 
    std::string timestamp = getCurrentDateTime();
    std::cout << "[LOG] User " << userId << " | " << type << " | Item " << itemId << " | " << timestamp << "\n";

    // 2. Tìm xem cặp User - Item này đã từng có lịch sử tương tác chưa
    bool found = false;
    for (int i = 0; i < all_interactions.size(); ++i) {
        if (all_interactions[i].user_id == userId && all_interactions[i].item_id == itemId) {
            // Đã từng có tương tác -> Chỉ cộng dồn hành vi (Click, Add Cart, Purchase)
            if (type == "click") all_interactions[i].click_count++;
            else if (type == "add_cart") all_interactions[i].add_cart_count++;
            else if (type == "purchase") all_interactions[i].purchase_count++;
            
            // Đã xóa phần tự động tính rating ở đây để bảo toàn cột sao đánh giá

            found = true;
            break;
        }
    }

    // 3. Nếu chưa từng tương tác bao giờ -> Tạo record mới
    if (!found) {
        // Khởi tạo record mới với điểm rating mặc định luôn là 0.0
        Interaction newInteract(userId, itemId, 0, 0, 0, 0.0);
        
        if (type == "click") newInteract.click_count = 1;
        else if (type == "add_cart") newInteract.add_cart_count = 1;
        else if (type == "purchase") newInteract.purchase_count = 1;

        // Đã xóa phần set rating 0.1, 0.5, 1.0 ở đây

        all_interactions.push_back(newInteract);
        std::cout << "[DEBUG] Da them vao RAM: User " << userId << " | Item " << itemId << "\n";
    }
}