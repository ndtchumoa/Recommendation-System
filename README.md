# Hệ Thống Gợi Ý Sản Phẩm (Recommendation System)

Dự án xây dựng một hệ thống gợi ý sản phẩm bằng ngôn ngữ **C++**, áp dụng phương pháp **Item-Based Collaborative Filtering**. Hệ thống phân tích hành vi người dùng, đánh giá ma trận tương tác và sử dụng **Cosine Similarity** để đưa ra các gợi ý cá nhân hóa.

<<<<<<< HEAD
> **Lưu ý:** Toàn bộ dự án **không sử dụng STL** (`<vector>`, `<map>`, `<unordered_map>`, `<algorithm>`). Thay vào đó, các cấu trúc dữ liệu `MyVector`, `MyMap`, `MyPair` được tự cài đặt bằng con trỏ và `new`/`delete`.
=======
## 🌟 Tiêu Chí Đánh Giá & Tính Năng
Hệ thống tính điểm tương tác (Score) dựa trên việc kết hợp nhiều hành vi của người dùng thay vì chỉ dựa vào số sao đánh giá:
* **Hành vi đa dạng:** Bao gồm click (xem sản phẩm), thêm vào giỏ hàng (`add_cart`), mua hàng (`purchase`), và đánh giá (`rating`).
* **Trọng số hành vi:** Các hành vi như click vào xem hay thêm vào giỏ hàng sẽ có mức trọng số thấp hơn so với việc mua hàng hoặc để lại rating. Hệ thống từ đó có thể gợi ý trực tiếp các sản phẩm giống với sản phẩm người dùng hay xem.
* **Xử lý người dùng mới (Cold Start):** Khi người dùng mới mở web, hệ thống tự động hiển thị các sản phẩm thịnh hành (sản phẩm giống với sản phẩm gần nhất mình xem hoặc sản phẩm mà những người giống mình gần đây đã xem).
>>>>>>> b8c1ae64c94847b04dee00ae1cc482c9f316487e

---

## 📋 Bảng Phân Rã Use Case & Hàm Nghiệp Vụ

### 1. Quản lý Tài khoản (Auth) — `UserManager`

| Use Case | Hàm | Input | Output |
|----------|-----|-------|--------|
| Đăng ký | `Register(fullName, email, password)` | fullName, email, password | `MyPair<bool, string>` (success, userId) |
| Đăng nhập | `Login(email, password)` | email, password | `MyPair<bool, string>` (success, token) |
| Đăng xuất | `Logout(userId)` | userId | `bool` (true/false) |

### 2. Duyệt & Lọc Sản phẩm — `ItemManager`

| Use Case | Hàm | Input | Output |
|----------|-----|-------|--------|
| Lấy DS Trang chủ | `GetHomeProducts(userId, limit)` | userId, limit | `MyVector<Item>` |
| Tìm kiếm SP | `SearchProducts(keyword)` | keyword | `MyVector<Item>` |
| Lọc & Sắp xếp | `FilterAndSortProducts(category, badge, minStar, sortBy)` | category, badge, minStar, sortBy | `MyVector<Item>` |
| Xem chi tiết SP | `GetProductDetail(productId)` | productId | `bool` + `Item&` |

### 3. Quản lý Giỏ hàng (Cart) — `InteractionManager`

| Use Case | Hàm | Input | Output |
|----------|-----|-------|--------|
| Thêm SP vào giỏ | `AddToCart(userId, productId, quantity)` | userId, productId, quantity | `MyPair<bool, string>` |
| Cập nhật số lượng | `UpdateCartItem(userId, productId, newQty)` | userId, productId, newQty | `MyPair<double, int>` (newTotal, newQty) |
| Xóa SP khỏi giỏ | `RemoveFromCart(userId, productId)` | userId, productId | `bool` |
| Lấy thông tin giỏ | `GetCartDetails(userId)` | userId | `MyPair<MyVector<CartItem>, double>` |
| Xóa giỏ hàng | `ClearCart(userId)` | userId | `void` |

### 4. Thanh toán (Checkout) — `InteractionManager`

| Use Case | Hàm | Input | Output |
|----------|-----|-------|--------|
| Tạo đơn hàng | `CreateOrder(userId, paymentMethod)` | userId, paymentMethod | `MyPair<bool, string>` (orderId) |
| Lấy chi tiết đơn | `GetOrderDetails(orderId)` | orderId | `bool` + `Order&` |

### 5. Cross-sell & Gợi ý — `InteractionManager`

| Use Case | Hàm | Input | Output |
|----------|-----|-------|--------|
| Gợi ý SP (sau mua) | `GetSuggestedProducts(orderId)` | orderId | `MyVector<Item>` |
| Ghi nhận tương tác | `AddInteraction(userId, itemId, type)` | userId, itemId, type | `void` |

---

## 🏗️ Kiến Trúc Hệ Thống & Cấu Trúc Thư Mục

```
Recommendation-System/
│
├── main.cpp                          # Entry Point — Demo pipeline đầy đủ
│
├── 1_models/                         # Tầng Models — Định nghĩa cấu trúc dữ liệu nền tảng
│   ├── User.h / User.cpp             # User (id, name, email, password, created_at)
│   ├── Item.h / Item.cpp             # Item (id, name, category, badge, price, star, sold)
│   └── Interaction.h / Interaction.cpp # Hành vi tương tác + tính Score
│
├── 2_data/                           # Tầng Data — Dữ liệu & Cấu trúc tự cài đặt
│   ├── MyDataStructures.h            # MyVector, MyMap, MyPair, MyHashHelper
│   ├── CSVLoader.h / CSVLoader.cpp   # Đọc Users, Items, Interactions từ CSV
│
├── 3_core/                           # Tầng Core — Thuật toán gợi ý cốt lõi
│   ├── RatingMatrix.h / .cpp         # Ma trận đánh giá User-Item + Mean-Centering
│   ├── SimilarityMatrix.h / .cpp     # Cosine Similarity Item-Item
│   └── Recommender.h / .cpp          # Item-Based CF + Cold Start
│
├── 6_services/                       # Tầng Service — Logic nghiệp vụ
│   ├── UserManager.h / .cpp          # Login, Register, Logout
│   ├── ItemManager.h / .cpp          # Search, Filter, Sort, Detail
│   └── InteractionManager.h / .cpp   # Cart, Order, Cross-sell, Logging
│
├── 4_dataset/                        # Dữ liệu CSV đầu vào
│   ├── users.csv
│   ├── items.csv
│   └── interactions.csv
│
├── report/                           # Báo cáo LaTeX
├── README.md
└── RUN_GUIDE.md
```

---

## 🧠 Thuật Toán Gợi Ý (Recommendation Logic)

### 1. Đối với người dùng đã có lịch sử tương tác (Item-Based CF):
- **Bước 1:** Lấy danh sách sản phẩm user đã tương tác.
- **Bước 2:** Tra `SimilarityMatrix` → tìm sản phẩm tương đồng.
- **Bước 3:** Tính điểm dự đoán: `predicted[j] = Σ(sim(i,j) × score(u,i)) / Σ|sim(i,j)|`
- **Bước 4:** Trả về Top-N sản phẩm có điểm cao nhất.

### 2. Đối với người dùng mới (Cold Start):
- **Bước 1:** Tính tổng điểm tương tác của từng sản phẩm trên toàn hệ thống.
- **Bước 2:** Gợi ý thẳng Top-K sản phẩm phổ biến nhất.

<<<<<<< HEAD
### 3. Cross-sell sau khi mua hàng (GetSuggestedProducts):
- Dựa vào danh mục sản phẩm đã mua trong đơn hàng → gợi ý sản phẩm cùng danh mục chưa mua.

---

## ⚙️ Cách Biên dịch & Chạy

### Yêu cầu:
- Trình biên dịch **MinGW-w64 GCC (g++)** phiên bản 12.1.0+

### Biên dịch (PowerShell):
```powershell
cd c:\Users\FPT\Downloads\PY\Recommendation-System
g++ -std=c++11 -o recommendation_system.exe main.cpp 1_models/User.cpp 1_models/Item.cpp 1_models/Interaction.cpp 6_services/UserManager.cpp 2_data/CSVLoader.cpp 6_services/ItemManager.cpp 3_core/RatingMatrix.cpp 3_core/SimilarityMatrix.cpp 3_core/Recommender.cpp 6_services/InteractionManager.cpp
```

### Chạy:
```powershell
.\recommendation_system.exe
=======
## 🌲 Direction Tree
```text
RECOMMENDATION_SYSTEMS/
├── 1_models/                   # Tầng Thực thể (Entities): Định nghĩa cấu trúc dữ liệu nền tảng
│   ├── User.h / User.cpp
│   ├── Item.h / Item.cpp
│   └── Interaction.h / Interaction.cpp
├── 2_data/                      # Tầng Hạ tầng (Infrastructure): I/O với CSV và MySQL
│   ├── CSVLoader.h / CSVLoader.cpp
│   ├── DatabaseManager.h / DatabaseManager.cpp
│   ├── db_config.example.h     # File MẪU cấu hình DB — commit lên git
│   └── db_config.h             # File cấu hình DB THẬT — KHÔNG commit (xem .gitignore)
├── 3_core/                       # Tầng Giải thuật (Algorithms)
│   ├── RatingMatrix.h / RatingMatrix.cpp
│   ├── SimilarityMatrix.h / SimilarityMatrix.cpp
│   └── Recommender.h / Recommender.cpp
├── 4_dataset/                    # Dữ liệu CSV đầu vào/đầu ra
│   ├── users.csv / items.csv / interactions.csv
│   └── export_users.csv / export_items.csv / export_interactions.csv  (sinh ra khi Export)
├── third_party/mysql/            # Header + lib MySQL Connector/C cho MinGW (xem mục Setup)
│   ├── include/                  # mysql.h, ...
│   └── lib/                       # libmysql.dll, libmysql.a
├── recommendationsystem_*.sql    # Bản dump schema + dữ liệu mẫu (dùng để seed DB cho team)
├── main.cpp                       # Entry Point: Menu Console
└── README.md
>>>>>>> b8c1ae64c94847b04dee00ae1cc482c9f316487e
```

---

<<<<<<< HEAD
## 🌟 Tính Năng Chính

- ✅ **Không STL** — Toàn bộ cấu trúc dữ liệu tự cài (`MyVector`, `MyMap`, `MyPair`)
- ✅ **Quản lý tài khoản** — Đăng ký, đăng nhập, đăng xuất với email/password + session token
- ✅ **Tìm kiếm sản phẩm** — Theo từ khóa, không phân biệt hoa thường
- ✅ **Lọc & Sắp xếp** — Theo danh mục, badge, đánh giá, giá (Bubble Sort)
- ✅ **Giỏ hàng** — Thêm, sửa, xóa, xem giỏ hàng
- ✅ **Đặt hàng** — Tạo đơn hàng với nhiều phương thức thanh toán
- ✅ **Cross-sell** — Gợi ý sản phẩm sau khi mua dựa trên danh mục
- ✅ **Item-Based CF** — Gợi ý cá nhân hóa dựa trên Cosine Similarity
- ✅ **Cold Start** — Gợi ý sản phẩm phổ biến cho người dùng mới
- ✅ **Ghi log tương tác** — Ghi nhận click, add_cart, purchase

---

## 📊 Cấu Trúc Dữ Liệu Tự Cài (MyDataStructures.h)

| Cấu trúc | Thay thế cho | Mô tả |
|----------|--------------|-------|
| `MyPair<K, V>` | `std::pair` | Cặp key-value |
=======
## 🚀 Hướng Dẫn Cho Thành Viên Nhóm (Setup Lần Đầu Sau Khi `git clone`)

Project dùng MySQL **chạy local trên máy mỗi người**, nên mỗi thành viên cần
tự cấu hình kết nối DB của riêng mình (mật khẩu KHÔNG được đưa lên GitHub).
Làm theo các bước sau **một lần** sau khi clone repo:

### Bước 1 — Cài MySQL Server (nếu chưa có)
Đảm bảo MySQL Server đang chạy trên `127.0.0.1:3306` (hoặc port khác bạn
tự chọn). Ghi nhớ user/password bạn dùng để đăng nhập (vd: `root`).

### Bước 2 — Tạo Database
Mở MySQL client (Workbench / CLI) và chạy:
```sql
CREATE DATABASE IF NOT EXISTS recommendationsystem
    CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;
```

### Bước 3 — Tạo file cấu hình DB cá nhân (`db_config.h`)
File này chứa mật khẩu MySQL của riêng bạn, **không được commit**.
```powershell
Copy-Item 2_data\db_config.example.h 2_data\db_config.h
```
Mở `2_data/db_config.h` vừa tạo, sửa `DB_USER`, `DB_PASS`, `DB_NAME`,
`DB_PORT` cho khớp với MySQL trên máy bạn.

### Bước 4 — Kiểm tra MySQL Connector/C (libmysql)
Nếu thư mục `third_party/mysql/` đã có sẵn trong repo (đã được commit
kèm `include/` và `lib/libmysql.dll`, `lib/libmysql.a`) → **bỏ qua bước
này**, bạn đã sẵn sàng build.

Nếu chưa có (hoặc bị thiếu) → làm theo hướng dẫn chi tiết trong
[`SETUP_MYSQL_CONNECTOR.md`](./SETUP_MYSQL_CONNECTOR.md) để tạo
`libmysql.a` từ MinGW và đặt vào `third_party/mysql/`.

### Bước 5 — Build
Trong VSCode: nhấn `Ctrl+Shift+B` (chạy task `build`, đã tự copy
`libmysql.dll` ra thư mục gốc sau khi build).

Hoặc build thủ công bằng terminal MinGW:
```bash
g++ -std=c++17 -O2 -Wall -Ithird_party/mysql/include \
  main.cpp 1_models/*.cpp 2_data/*.cpp 3_core/*.cpp \
  -Lthird_party/mysql/lib -llibmysql -o recommendation.exe

cp third_party/mysql/lib/libmysql.dll .
```

### Bước 6 — Đưa dữ liệu mẫu vào MySQL của bạn
Có 2 cách (chọn 1):

**Cách A — Dùng menu trong chương trình (khuyên dùng):**
```
./recommendation.exe
→ chọn "2. Import CSV -> MySQL Database"
```
Chương trình sẽ tự tạo bảng (`createTables()`) và import dữ liệu từ
`4_dataset/users.csv`, `items.csv`, `interactions.csv`.

**Cách B — Import trực tiếp từ file `.sql` dump (có sẵn dữ liệu mẫu)::**
```bash
mysql -u root -p recommendationsystem < recommendationsystem_users.sql
mysql -u root -p recommendationsystem < recommendationsystem_items.sql
mysql -u root -p recommendationsystem < recommendationsystem_interactions.sql
```
(Thứ tự quan trọng: users, items trước, interactions sau — vì có FOREIGN KEY.)

### Bước 7 — Chạy thử
```
./recommendation.exe
→ chọn "1" để chạy gợi ý từ CSV
→ chọn "3" để export dữ liệu từ MySQL ra 4_dataset/export_*.csv (kiểm tra đồng bộ)
```

---

### ⚠️ Lưu ý khi làm việc nhóm với Git

* **Không bao giờ** sửa trực tiếp giá trị trong `db_config.example.h` thành
  mật khẩu thật của bạn rồi commit — hãy luôn dùng file `db_config.h`
  (đã bị `.gitignore` chặn).
* Nếu lỡ commit nhầm `db_config.h` chứa mật khẩu thật, hãy đổi mật khẩu
  MySQL đó và xoá file khỏi lịch sử git (`git rm --cached`, rồi commit
  lại + thêm vào `.gitignore`).
* `4_dataset/export_*.csv` được sinh ra khi chạy menu "Export" — không
  cần commit, mỗi người tự export từ DB của mình khi cần kiểm tra.
* Nếu cả nhóm muốn **chia sẻ chung 1 bộ dữ liệu mới nhất** (sau khi ai đó
  sửa dữ liệu trong DB), hãy: chạy "Export" → commit lại
  `4_dataset/export_*.csv` hoặc tạo file `.sql` dump mới
  (`mysqldump -u root -p recommendationsystem > recommendationsystem_full.sql`)
  để người khác import lại.
>>>>>>> b8c1ae64c94847b04dee00ae1cc482c9f316487e
