#pragma once
#include <string>

// ═══════════════════════════════════════════════════════════════════════
// db_config.example.h  —  FILE MẪU cấu hình kết nối MySQL
//
// HƯỚNG DẪN CHO MỖI THÀNH VIÊN (làm 1 lần khi mới clone repo):
//   1. Copy file này -> đặt tên "db_config.h" (CÙNG thư mục 2_data/)
//        Windows (PowerShell):
//            Copy-Item 2_data\db_config.example.h 2_data\db_config.h
//        hoặc đơn giản là Copy/Paste trong VSCode rồi đổi tên.
//
//   2. Mở "db_config.h" vừa tạo, sửa DB_USER / DB_PASS / DB_NAME / DB_PORT
//      cho khớp với MySQL Server đang chạy TRÊN MÁY BẠN.
//
//   3. KHÔNG commit "db_config.h" lên Git — file này đã được khai báo
//      trong .gitignore. Mỗi người giữ mật khẩu riêng, không lộ lên repo.
//
//   4. main.cpp sẽ tự #include "2_data/db_config.h" — nếu chưa tạo file
//      này, chương trình sẽ KHÔNG compile được (báo lỗi thiếu file).
// ═══════════════════════════════════════════════════════════════════════

static const std::string DB_HOST = "127.0.0.1";
static const std::string DB_USER = "root";
static const std::string DB_PASS = "your_mysql_password_here";
static const std::string DB_NAME = "recommendation_db";
static const unsigned int DB_PORT = 3306;
