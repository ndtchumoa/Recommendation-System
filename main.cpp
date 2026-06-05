#include <iostream>
#include <iomanip>
#include "data/CSVLoader.h"

int main() {
    std::cout << "========================================\n";
    std::cout << "  RECOMMENDATION SYSTEM - Data Loader Test\n";
    std::cout << "========================================\n\n";

    CSVLoader loader;

    // ── 1. Nạp Users ─────────────────────────────────────────────────
    std::vector<User> users;
    try {
        users = loader.loadUsers("datasets/users.csv");
    } catch (const std::exception& e) {
        std::cerr << "LOI: " << e.what() << "\n";
        return 1;
    }

    std::cout << "\n[Preview] 3 nguoi dung dau:\n";
    for (size_t i = 0; i < std::min((size_t)3, users.size()); ++i)
        users[i].print();

    // ── 2. Nạp Items ─────────────────────────────────────────────────
    std::vector<Item> items;
    try {
        items = loader.loadItems("datasets/items.csv");
    } catch (const std::exception& e) {
        std::cerr << "LOI: " << e.what() << "\n";
        return 1;
    }

    std::cout << "\n[Preview] 3 san pham dau:\n";
    for (size_t i = 0; i < std::min((size_t)3, items.size()); ++i)
        items[i].print();

    // ── 3. Nạp Interactions ──────────────────────────────────────────
    std::vector<Interaction> interactions;
    try {
        interactions = loader.loadInteractions("datasets/interactions.csv");
    } catch (const std::exception& e) {
        std::cerr << "LOI: " << e.what() << "\n";
        return 1;
    }

    std::cout << "\n[Preview] 5 tuong tac dau (kem computeScore):\n";
    for (size_t i = 0; i < std::min((size_t)5, interactions.size()); ++i)
        interactions[i].print();

    // ── 4. Tổng kết ──────────────────────────────────────────────────
    std::cout << "\n========================================\n";
    std::cout << "  Tong ket nap du lieu:\n";
    std::cout << "  Users        : " << users.size()        << "\n";
    std::cout << "  Items        : " << items.size()         << "\n";
    std::cout << "  Interactions : " << interactions.size()  << "\n";
    std::cout << "========================================\n";

    // ── 5. Demo computeScore cho một vài tương tác đặc biệt ──────────
    std::cout << "\n[Demo] Kiem tra cong thuc computeScore:\n";
    std::cout << std::fixed << std::setprecision(1);

    Interaction demo1("U001", "I001", 3, 1, 1, 5);
    // Ky vong: 3*0.5 + 1*1.5 + 1*4.0 + 5*1.0 = 1.5+1.5+4.0+5.0 = 12.0
    std::cout << "  click=3,cart=1,buy=1,stars=5 => Score = "
              << demo1.computeScore() << " (ky vong: 12.0)\n";

    Interaction demo2("U002", "I002", 5, 0, 0, 0);
    // Ky vong: 5*0.5 + 0 + 0 + 0 = 2.5
    std::cout << "  click=5,cart=0,buy=0,stars=0 => Score = "
              << demo2.computeScore() << " (ky vong: 2.5)\n";

    Interaction demo3("U003", "I003", 1, 2, 1, 4);
    // Ky vong: 0.5 + 3.0 + 4.0 + 4.0 = 11.5
    std::cout << "  click=1,cart=2,buy=1,stars=4 => Score = "
              << demo3.computeScore() << " (ky vong: 11.5)\n";

    return 0;
}
