# Hệ Thống Gợi Ý Sản Phẩm (Recommendation System)

Dự án xây dựng một hệ thống gợi ý sản phẩm bằng ngôn ngữ **C++**, áp dụng phương pháp **Item-Based Collaborative Filtering**. Hệ thống phân tích hành vi người dùng, đánh giá ma trận tương tác và sử dụng **Cosine Similarity** để đưa ra các gợi ý cá nhân hóa.

> **Lưu ý:** Toàn bộ dự án **không sử dụng STL** (`<vector>`, `<map>`, `<unordered_map>`, `<algorithm>`). Thay vào đó, các cấu trúc dữ liệu `MyVector`, `MyMap`, `MyPair` được tự cài đặt bằng con trỏ và `new`/`delete`.

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
```

---

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
