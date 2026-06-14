#include "User.h"

// --- Hàm khởi tạo (Constructors) ---

User::User() : user_id(""), name(""), email(""), password(""), created_at("") {}

User::User(std::string id, std::string n, std::string e, std::string pwd, std::string date)
    : user_id(id), name(n), email(e), password(pwd), created_at(date) {}

// --- Getters ---

std::string User::getUserId() const { return user_id; }
std::string User::getName() const { return name; }
std::string User::getEmail() const { return email; }
std::string User::getPassword() const { return password; }
std::string User::getCreatedAt() const { return created_at; }

// --- Setters ---

void User::setName(const std::string& n) { name = n; }
void User::setEmail(const std::string& e) { email = e; }
void User::setPassword(const std::string& pwd) { password = pwd; }
void User::setCreatedAt(const std::string& date) { created_at = date; }

// --- Xác thực mật khẩu ---

bool User::checkPassword(const std::string& pwd) const {
    return (password == pwd);
}

// --- Xuất thông tin ---

void User::displayInfo() const {
    std::cout << "User ID: " << user_id
              << " | Name: " << name
              << " | Email: " << email
              << " | Created At: " << created_at << std::endl;
}