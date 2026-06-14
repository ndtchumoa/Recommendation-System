# Hướng dẫn cài đặt MySQL Connector/C (libmysql) cho project g++ trên Windows

Bạn đã cài **MySQL Server** nhưng chưa cấu hình để g++ (MinGW) có thể
`#include <mysql.h>` và link với thư viện client MySQL. Làm theo các bước dưới.

---

## 1. Tìm thư mục cài MySQL Server

Mặc định MySQL Server cài tại:

```
C:\Program Files\MySQL\MySQL Server 8.0\
├── include\      <- chứa mysql.h, mysql_version.h, ...
└── lib\           <- chứa libmysql.dll, libmysql.lib (MSVC import lib)
```

Đây chính là **MySQL Connector/C** — không cần cài thêm gì nếu bạn không
muốn tải bộ riêng. Nếu bạn muốn dùng bộ Connector/C độc lập (nhẹ hơn), có
thể tải "MySQL Connector/C 6.1" từ trang downloads của MySQL, giải nén ra
một thư mục bất kỳ, ví dụ `C:\mysql-connector-c\`.

---

## 2. Vấn đề: `libmysql.lib` là cho MSVC, không dùng được với MinGW g++

File `.lib` đi kèm MySQL Server được build cho MSVC (Visual Studio), MinGW
g++ **không link được trực tiếp**. Bạn cần tạo bản `.a` (import library)
tương thích MinGW từ file `libmysql.dll`.

### Cách tạo `libmysql.a` từ `libmysql.dll`

Mở **MSYS2 / MinGW shell** (đi kèm khi cài MinGW-w64), `cd` vào thư mục
chứa `libmysql.dll` (ví dụ `lib\`), rồi chạy:

```bash
gendef libmysql.dll
dlltool -d libmysql.def -l libmysql.a -D libmysql.dll
```

Kết quả: bạn có file `libmysql.a` — copy file này vào một thư mục dùng
chung, ví dụ `C:\mysql-connector-c\lib\`.

> Nếu máy chưa có `gendef`/`dlltool`, cài qua MSYS2:
> `pacman -S mingw-w64-x86_64-tools-git` (hoặc tương đương).

---

## 3. Sao chép header & lib vào project (gợi ý để gọn)

Để mọi máy trong nhóm build được giống nhau, copy 2 thứ sau vào project,
ví dụ tạo thư mục `third_party\mysql\`:

```
third_party/mysql/
├── include/      <- copy toàn bộ nội dung từ MySQL include (chứa mysql.h)
└── lib/
    ├── libmysql.dll
    └── libmysql.a
```

> `libmysql.dll` **phải nằm cùng thư mục với file .exe** khi chạy, vì đây
> là thư viện liên kết động (runtime dependency).

---

## 4. Cập nhật `c_cpp_properties.json` (IntelliSense)

```jsonc
{
    "configurations": [
        {
            "name": "Win32",
            "includePath": [
                "${workspaceFolder}/**",
                "${workspaceFolder}/third_party/mysql/include"
            ],
            "defines": [],
            "compilerPath": "C:/msys64/mingw64/bin/g++.exe",
            "cStandard": "c17",
            "cppStandard": "c++17",
            "intelliSenseMode": "gcc-x64"
        }
    ],
    "version": 4
}
```

---

## 5. Cập nhật `tasks.json` (lệnh build)

```jsonc
{
    "version": "2.0.0",
    "tasks": [
        {
            "label": "build-recommendation",
            "type": "shell",
            "command": "g++",
            "args": [
                "-std=c++17", "-O2", "-Wall",
                "-I${workspaceFolder}/third_party/mysql/include",
                "${workspaceFolder}/main.cpp",
                "${workspaceFolder}/1_models/User.cpp",
                "${workspaceFolder}/1_models/Item.cpp",
                "${workspaceFolder}/1_models/Interaction.cpp",
                "${workspaceFolder}/2_data/CSVLoader.cpp",
                "${workspaceFolder}/2_data/DatabaseManager.cpp",
                "${workspaceFolder}/3_core/RatingMatrix.cpp",
                "${workspaceFolder}/3_core/SimilarityMatrix.cpp",
                "${workspaceFolder}/3_core/Recommender.cpp",
                "-L${workspaceFolder}/third_party/mysql/lib",
                "-llibmysql",
                "-o", "${workspaceFolder}/recommendation.exe"
            ],
            "group": { "kind": "build", "isDefault": true },
            "problemMatcher": ["$gcc"]
        }
    ]
}
```

Hoặc chạy thủ công trên terminal (MinGW):

```bash
g++ -std=c++17 -O2 -Wall ^
  -Ithird_party/mysql/include ^
  main.cpp 1_models/*.cpp 2_data/*.cpp 3_core/*.cpp ^
  -Lthird_party/mysql/lib -llibmysql ^
  -o recommendation.exe
```

(Trên PowerShell, dùng dấu `` ` `` cuối dòng thay cho `^`, hoặc viết một dòng.)

---

## 6. Chạy chương trình

Trước khi chạy `recommendation.exe`, đảm bảo `libmysql.dll` nằm:
- cùng thư mục với `.exe`, **hoặc**
- nằm trong một thư mục có trong `PATH`.

Nếu thiếu, chương trình sẽ báo lỗi "không tìm thấy libmysql.dll" khi khởi
chạy (lỗi runtime, không phải lỗi compile).

---

## 7. Tạo Database & User trên MySQL Server

```sql
CREATE DATABASE IF NOT EXISTS recommendation_db
    CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;

-- (tuỳ chọn) tạo user riêng cho app
CREATE USER 'rec_app'@'localhost' IDENTIFIED BY 'your_password';
GRANT ALL PRIVILEGES ON recommendation_db.* TO 'rec_app'@'localhost';
FLUSH PRIVILEGES;
```

Chương trình (`main.cpp`) sẽ tự gọi `createTables()` để tạo các bảng
`users`, `items`, `interactions` nếu chưa tồn tại — bạn không cần tạo
bảng bằng tay.

Sau khi setup xong, sửa các hằng số kết nối ở đầu `main.cpp`
(`DB_HOST`, `DB_USER`, `DB_PASS`, `DB_NAME`, `DB_PORT`) cho khớp với máy
của bạn.
