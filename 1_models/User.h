#ifndef USER_H
#define USER_H

#include <string>
#include <iostream>

class User {
private:
    std::string user_id;
    std::string name;
    std::string email;
    std::string password;   // Lưu ý: Trong thực tế cần mã hóa password
    std::string created_at;

public:
    // 1. Khởi tạo
    User();
    User(std::string id, std::string n, std::string e, std::string pwd, std::string date);

    // 2. Getters
    std::string getUserId() const;
    std::string getName() const;
    std::string getEmail() const;
    std::string getPassword() const;   // Dùng để xác thực
    std::string getCreatedAt() const;

    // 3. Setters
    void setName(const std::string& n);
    void setEmail(const std::string& e);
    void setPassword(const std::string& pwd);
    void setCreatedAt(const std::string& date);

    // 4. Hỗ trợ xác thực
    bool checkPassword(const std::string& pwd) const;

    // 5. Xuất thông tin
    void displayInfo() const;
};

#endif // USER_H