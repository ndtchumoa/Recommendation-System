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
├── models/                     # Tầng Thực thể (Entities): Định nghĩa cấu trúc dữ liệu nền tảng
│   ├── User.h                  # Khai báo cấu trúc/lớp User (user_id, name, created_at)
│   ├── User.cpp                # Định nghĩa phương thức bổ trợ của User (nếu có)
│   ├── Item.h                  # Khai báo cấu trúc/lớp Item (item_id, name)
│   ├── Item.cpp                # Định nghĩa phương thức bổ trợ của Item (nếu có)
│   ├── Interaction.h           # Khai báo hành vi người dùng (click, add_cart, purchase, rating)
│   └── Interaction.cpp         # Định nghĩa hàm computeScore() tính toán điểm tổng hợp có trọng số
├── data/                       # Tầng Hạ tầng (Infrastructure): Quản lý nạp dữ liệu thô từ file
│   ├── CSVLoader.h             # Khai báo lớp CSVLoader đọc dữ liệu từ các file CSV
│   └── CSVLoader.cpp           # Cài đặt logic parse chuỗi, nạp danh sách Users, Items, Interactions
├── core/                       # Tầng Giải thuật (Algorithms): Xử lý tính toán và logic gợi ý cốt lõi
│   ├── RatingMatrix.h          # Khai báo lớp quản lý ma trận điểm tương tác (User-Item Sparse Matrix)
│   ├── RatingMatrix.cpp        # Triển khai xây dựng ma trận từ danh sách Interactions (dùng Hash Map lồng)
│   ├── SimilarityMatrix.h      # Khai báo lớp quản lý ma trận độ tương đồng giữa các sản phẩm (Item-Item)
│   ├── SimilarityMatrix.cpp    # Triển khai chuẩn hóa ma trận và tính toán Cosine Similarity, lọc Top K
│   ├── Recommender.h           # Khai báo lớp điều phối gợi ý hệ thống
│   └── Recommender.cpp         # Cài đặt logic gợi ý cá nhân hóa choUser() và giải quyết Cold Start
├── datasets/                   # Nơi chứa các tệp dữ liệu đầu vào thực tế
│   ├── users.csv               # Dữ liệu tài khoản người dùng mẫu
│   ├── items.csv               # Dữ liệu danh mục sản phẩm mẫu
│   └── interactions.csv        # Lịch sử tương tác thô của người dùng với sản phẩm
├── main.cpp                    # Entry Point: Khởi chạy menu Console, điều phối luồng xử lý toàn cục
└── README.md                   # Tài liệu hướng dẫn, mô tả tổng quan đồ án và phân công nhóm