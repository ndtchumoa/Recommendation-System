#ifndef INTERACTION_MANAGER_H
#define INTERACTION_MANAGER_H

#include <string>
#include "../2_data/MyDataStructures.h"
#include "../1_models/Item.h"
#include "../1_models/Interaction.h"
struct CartItem {
    std::string item_id;
    std::string item_name;
    double price;
    int quantity;
    double subtotal;

    CartItem() : item_id(""), item_name(""), price(0.0), quantity(0), subtotal(0.0) {}
};

struct Order {
    std::string order_id;
    std::string user_id;
    MyVector<CartItem> items;
    double total_amount;
    std::string status;
    std::string created_at;
    std::string payment_method;

    Order() : order_id(""), user_id(""), total_amount(0.0), status("Pending") {}
};

class InteractionManager {
private:
    MyMap<std::string, MyVector<CartItem>> carts;
    MyMap<std::string, Order> orders;
    MyVector<Item> items;

    MyVector<Interaction> all_interactions;

    std::string generateOrderId() const;
    std::string getCurrentDateTime() const;
    bool findItemById(const std::string& itemId, Item& out) const;

public:
    InteractionManager(const MyVector<Item>& loaded_items, const MyVector<Interaction>& loaded_interactions);
    InteractionManager(const MyVector<Item>& loaded_items);

    MyPair<bool, std::string> AddToCart(const std::string& userId,
                                        const std::string& productId,
                                        int quantity);

    MyPair<double, int> UpdateCartItem(const std::string& userId,
                                       const std::string& productId,
                                       int newQuantity);

    bool RemoveFromCart(const std::string& userId,
                        const std::string& productId);

    MyPair<MyVector<CartItem>, double> GetCartDetails(const std::string& userId) const;

    void ClearCart(const std::string& userId);

    MyPair<bool, std::string> CreateOrder(const std::string& userId,
                                          const std::string& paymentMethod);

    bool GetOrderDetails(const std::string& orderId, Order& out) const;

    MyVector<Item> GetSuggestedProducts(const std::string& orderId) const;

    void AddInteraction(const std::string& userId,
                        const std::string& itemId,
                        const std::string& type);
const MyVector<Interaction>& getInteractions() const {
        return all_interactions;
    }
};

#endif // INTERACTION_MANAGER_H