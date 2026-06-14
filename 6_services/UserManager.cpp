#include "UserManager.h"
#include <iostream>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <cstdlib> // cho rand()

// --- Constructor ---
UserManager::UserManager(const MyVector<User>& loaded_users) {
    users = loaded_users;
    logged_in_user_id = "";
}

// --- Hỗ trợ: Tạo token ngẫu nhiên ---
std::string UserManager::generateToken() const {
    std::stringstream ss;
    ss << "TOKEN_" << std::time(nullptr) << "_" << (rand() % 10000);
    return ss.str();
}

// --- Hỗ trợ: Lấy thời gian hiện tại ---
std::string UserManager::getCurrentDateTime() const {
    std::time_t t = std::time(nullptr);
    std::tm* now = std::localtime(&t);
    std::stringstream ss;
    ss << (now->tm_year + 1900) << '-'
       << std::setfill('0') << std::setw(2) << (now->tm_mon + 1) << '-'
       << std::setfill('0') << std::setw(2) << now->tm_mday;
    return ss.str();
}

// --- Hỗ trợ: Tạo userId tự động (U011, U012, ...) ---
std::string UserManager::generateUserId() const {
    int maxNum = 0;
    for (int i = 0; i < users.size(); ++i) {
        std::string uid = users[i].getUserId();
        // uid có dạng "U001" → lấy phần số
        if (uid.length() > 1 && uid[0] == 'U') {
            int num = std::stoi(uid.substr(1));
            if (num > maxNum) maxNum = num;
        }
    }
    std::stringstream ss;
    ss << "U" << std::setfill('0') << std::setw(3) << (maxNum + 1);
    return ss.str();
}

// --- 1. Register: Đăng ký tài khoản mới ---
// Input: fullName, email, password
// Output: MyPair<bool, string> (true, userId) hoặc (false, lý do)
MyPair<bool, std::string> UserManager::Register(const std::string& fullName,
                                                  const std::string& email,
                                                  const std::string& password) {
    // Kiểm tra email đã tồn tại chưa
    for (int i = 0; i < users.size(); ++i) {
        if (users[i].getEmail() == email) {
            return MyPair<bool, std::string>(false, "Email da duoc dang ky!");
        }
    }

    // Kiểm tra dữ liệu đầu vào
    if (fullName.empty() || email.empty() || password.empty()) {
        return MyPair<bool, std::string>(false, "Vui long dien day du thong tin!");
    }

    // Tạo userId mới
    std::string newId = generateUserId();
    std::string createdAt = getCurrentDateTime();

    // Tạo User mới
    User newUser(newId, fullName, email, password, createdAt);
    users.push_back(newUser);

    std::cout << "[+] Dang ky thanh cong! User ID: " << newId << "\n";
    return MyPair<bool, std::string>(true, newId);
}

// --- 2. Login: Đăng nhập bằng email + password ---
// Output: MyPair<bool, string> (true, token) hoặc (false, lý do)
MyPair<bool, std::string> UserManager::Login(const std::string& email,
                                               const std::string& password) {
    for (int i = 0; i < users.size(); ++i) {
        if (users[i].getEmail() == email) {
            if (users[i].checkPassword(password)) {
                // Tạo token session
                std::string token = generateToken();
                std::string uid = users[i].getUserId();
                sessions[uid] = token;
                logged_in_user_id = uid;

                std::cout << "[+] Dang nhap thanh cong! Xin chao "
                          << users[i].getName() << "\n";
                return MyPair<bool, std::string>(true, token);
            } else {
                return MyPair<bool, std::string>(false, "Sai mat khau!");
            }
        }
    }
    return MyPair<bool, std::string>(false, "Email khong ton tai!");
}

// --- 3. Logout: Đăng xuất ---
bool UserManager::Logout(const std::string& userId) {
    // Xóa session của userId này
    if (sessions.remove(userId)) {
        if (logged_in_user_id == userId) {
            logged_in_user_id = "";
        }
        std::cout << "[+] Dang xuat thanh cong!\n";
        return true;
    }
    std::cout << "[!] User " << userId << " chua dang nhap!\n";
    return false;
}

// --- Hỗ trợ ---

std::string UserManager::getLoggedInUserId() const {
    return logged_in_user_id;
}

const MyVector<User>& UserManager::getUsers() const {
    return users;
}

bool UserManager::isEmailExist(const std::string& email) const {
    for (int i = 0; i < users.size(); ++i) {
        if (users[i].getEmail() == email) {
            return true;
        }
    }
    return false;
}

bool UserManager::getUserById(const std::string& userId, User& out) const {
    for (int i = 0; i < users.size(); ++i) {
        if (users[i].getUserId() == userId) {
            out = users[i];
            return true;
        }
    }
    return false;
}