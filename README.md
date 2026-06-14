# Hệ Thống Gợi Ý Sản Phẩm (Item-Based Collaborative Filtering)

Dự án xây dựng một hệ thống gợi ý sản phẩm (Product Recommendation System) bằng ngôn ngữ **C++**, áp dụng phương pháp **Item-Based Collaborative Filtering**. Hệ thống phân tích hành vi người dùng, đánh giá ma trận tương tác và sử dụng toán học (Cosine Similarity) để đưa ra các gợi ý cá nhân hóa.

## 🌟 Tiêu Chí Đánh Giá & Tính Năng
Hệ thống tính điểm tương tác (Score) dựa trên việc kết hợp nhiều hành vi của người dùng thay vì chỉ dựa vào số sao đánh giá:
* **Hành vi đa dạng:** Bao gồm click (xem sản phẩm), thêm vào giỏ hàng (`add_cart`), mua hàng (`purchase`), và đánh giá (`rating`).
* **Trọng số hành vi:** Các hành vi như click vào xem hay thêm vào giỏ hàng sẽ có mức trọng số thấp hơn so với việc mua hàng hoặc để lại rating. Hệ thống từ đó có thể gợi ý trực tiếp các sản phẩm giống với sản phẩm người dùng hay xem.
* **Xử lý người dùng mới (Cold Start):** Khi người dùng mới mở web, hệ thống tự động hiển thị các sản phẩm thịnh hành (sản phẩm giống với sản phẩm gần nhất mình xem hoặc sản phẩm mà những người giống mình gần đây đã xem).

## 📂 Cấu Trúc Dữ Liệu (Entities)
Dữ liệu cốt lõi được xây dựng qua các `struct`/`class` sau:
* **User:** Quản lý thông tin người dùng (`user_id`, `name`, `created_at`).
* **Item:** Quản lý thông tin sản phẩm (`item_id`, `name`).
* **Interaction:** Quản lý hành vi tương tác giữa người dùng và sản phẩm (`item_id`, `user_id`, `click`, `add_cart`, `purchase`, `rating`). Hàm `computeScore()` được dùng để tính toán điểm tương tác tổng hợp.

## 🏗️ Kiến Trúc Hệ Thống
Hệ thống được chia thành 4 tầng chính:
1. **Infrastructure (I/O):** Lớp `CSVLoader` chịu trách nhiệm nạp dữ liệu từ các tệp `users.csv`, `items.csv` (hoặc `products.csv`), và `ratings.csv` (hoặc `interactions.csv`).
2. **Algorithm - Rating Matrix:** Lớp `RatingMatrix` xây dựng ma trận đánh giá (hàng là Users, cột là Items).
3. **Algorithm - Similarity Matrix:** Lớp `SimilarityMatrix` thực hiện chuẩn hóa ma trận và tính toán độ tương đồng giữa các sản phẩm (Item-Item) bằng **hàm Cosine**. Có chức năng lọc ra Top K sản phẩm tương đồng.
4. **Algorithm - Recommender:** Lớp `Recommender` kết hợp `RatingMatrix` và `SimilarityMatrix` để xử lý logic dự đoán cho từng người dùng (`forUser`) hoặc xử lý khởi đầu lạnh (`coldStart`).
5. **Entry Point:** Lớp `Main` làm cầu nối gọi các module.

## ⚙️ Quy Trình Hoạt Động (Workflow)
Chương trình thực thi trong file `main.cpp` theo luồng tuần tự sau:
1. `Load CSV`- Đọc dữ liệu đầu vào.
2. `Build Rating Matrix` - Đưa dữ liệu tương tác vào cấu trúc ma trận.
3. `Calculate Item Similarity` - Tính độ tương đồng giữa các Item.
4. `Recommend` - Đưa ra dự đoán gợi ý.
5. `Print Top N Products` - Trả kết quả ra màn hình.

## 🧠 Thuật Toán Gợi Ý (Recommendation Logic)

### 1. Đối với người dùng đã có lịch sử tương tác:
* **Bước 1:** Chọn User cần nhận gợi ý.
* **Bước 2:** Lấy danh sách các sản phẩm mà User này đã đánh giá cao.
* **Bước 3:** Quét `SimilarityMatrix` để tìm các sản phẩm có độ tương đồng cao với tập sản phẩm ở Bước 2.
* **Bước 4:** Tính toán điểm dự đoán cuối cùng và trả về danh sách gợi ý.

### 2. Đối với người dùng mới (Cold Start):
* **Bước 1:** Lấy bảng ma trận Rating hiện tại và tính tổng điểm tương tác của từng sản phẩm (bằng cách cộng tổng giá trị các hàng trong cùng một cột sản phẩm).
* **Bước 2:** Lọc và gợi ý thẳng **Top K** sản phẩm có điểm tổng hợp cao nhất để giới thiệu cho người dùng mới.

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
```

---

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
