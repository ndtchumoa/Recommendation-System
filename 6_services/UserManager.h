#ifndef USER_MANAGER_H
#define USER_MANAGER_H

#include <string>
#include "../2_data/MyDataStructures.h"
#include "../1_models/User.h"

/*
 * UserManager - Quản lý tài khoản người dùng
 * Chức năng:
 *   - Login(email, password)      : Đăng nhập → token + thông tin User
 *   - Register(fullName, email, pwd) : Đăng ký tài khoản mới
 *   - Logout(userId)              : Đăng xuất, xóa session
 */
class UserManager {
private:
    MyVector<User> users;
    MyMap<std::string, std::string> sessions;
    std::string logged_in_user_id;

    std::string generateToken() const;
    std::string getCurrentDateTime() const;
    std::string generateUserId() const;

public:
    UserManager(const MyVector<User>& loaded_users);

    MyPair<bool, std::string> Register(const std::string& fullName,
                                       const std::string& email,
                                       const std::string& password);

    MyPair<bool, std::string> Login(const std::string& email,
                                    const std::string& password);

    bool Logout(const std::string& userId);

    std::string getLoggedInUserId() const;
    const MyVector<User>& getUsers() const;
    bool isEmailExist(const std::string& email) const;
    bool getUserById(const std::string& userId, User& out) const;
};

#endif // USER_MANAGER_H