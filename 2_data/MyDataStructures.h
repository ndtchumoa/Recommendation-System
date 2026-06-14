#ifndef MY_DATA_STRUCTURES_H
#define MY_DATA_STRUCTURES_H

#include <string>   // Chỉ dùng std::string, không dùng STL container
#include <iostream> // Chỉ dùng cho debug/print
#include <cstring>  // cho memcpy, memset

// ============================================================================
//  MyPair<K, V>  —  Thay thế std::pair
// ============================================================================
template <typename K, typename V>
struct MyPair {
    K first;
    V second;

    MyPair() : first(), second() {}
    MyPair(const K& f, const V& s) : first(f), second(s) {}
    MyPair(const MyPair& other) : first(other.first), second(other.second) {}

    MyPair& operator=(const MyPair& other) {
        if (this != &other) {
            first = other.first;
            second = other.second;
        }
        return *this;
    }

    bool operator==(const MyPair& other) const {
        return first == other.first && second == other.second;
    }
};


// ============================================================================
//  MyVector<T>  —  Mảng động tự co giãn (thay thế std::vector)
// ============================================================================
template <typename T>
class MyVector {
private:
    T* data;        // Con trỏ quản lý mảng động
    int sz;         // Số phần tử hiện tại
    int cap;        // Sức chứa tối đa

    // Cấp phát lại bộ nhớ khi cần mở rộng
    void reserveMore(int new_cap) {
        T* new_data = new T[new_cap];
        for (int i = 0; i < sz; ++i) {
            new_data[i] = data[i];
        }
        delete[] data;
        data = new_data;
        cap = new_cap;
    }

public:
    // ---------- Constructor & Destructor ----------
    MyVector() : data(nullptr), sz(0), cap(0) {}

    MyVector(int initial_size) : sz(initial_size), cap(initial_size) {
        data = new T[cap];
        for (int i = 0; i < sz; ++i) {
            data[i] = T();
        }
    }

    MyVector(const MyVector& other) : sz(other.sz), cap(other.cap) {
        data = new T[cap];
        for (int i = 0; i < sz; ++i) {
            data[i] = other.data[i];
        }
    }

    ~MyVector() {
        delete[] data;
        data = nullptr;
        sz = 0;
        cap = 0;
    }

    MyVector& operator=(const MyVector& other) {
        if (this != &other) {
            delete[] data;
            sz = other.sz;
            cap = other.cap;
            data = new T[cap];
            for (int i = 0; i < sz; ++i) {
                data[i] = other.data[i];
            }
        }
        return *this;
    }

    // ---------- Truy cập phần tử ----------
    T& operator[](int index) {
        return data[index];
    }

    const T& operator[](int index) const {
        return data[index];
    }

    T& at(int index) {
        if (index < 0 || index >= sz) {
            std::cerr << "MyVector::at: Index out of range!" << std::endl;
            exit(1);
        }
        return data[index];
    }

    const T& at(int index) const {
        if (index < 0 || index >= sz) {
            std::cerr << "MyVector::at: Index out of range!" << std::endl;
            exit(1);
        }
        return data[index];
    }

    T& front() { return data[0]; }
    const T& front() const { return data[0]; }

    T& back() { return data[sz - 1]; }
    const T& back() const { return data[sz - 1]; }

    // ---------- Kích thước ----------
    int size() const { return sz; }
    int capacity() const { return cap; }
    bool empty() const { return sz == 0; }

    // ---------- Thao tác mảng ----------
    void push_back(const T& value) {
        if (sz >= cap) {
            int new_cap = (cap == 0) ? 4 : cap * 2;
            reserveMore(new_cap);
        }
        data[sz] = value;
        ++sz;
    }

    void pop_back() {
        if (sz > 0) {
            --sz;
        }
    }

    void clear() {
        delete[] data;
        data = nullptr;
        sz = 0;
        cap = 0;
    }

    void reserve(int new_cap) {
        if (new_cap > cap) {
            reserveMore(new_cap);
        }
    }

    // ---------- Iterator hỗ trợ for-each ----------
    T* begin() { return data; }
    const T* begin() const { return data; }
    T* end() { return data + sz; }
    const T* end() const { return data + sz; }
};


// ============================================================================
//  MyHashHelper<T>  —  Hỗ trợ băm (dùng template specialization)
// ============================================================================
template <typename T>
struct MyHashHelper {
    static unsigned long hashValue(const T& key) {
        const unsigned char* p = reinterpret_cast<const unsigned char*>(&key);
        unsigned long h = 0;
        for (size_t i = 0; i < sizeof(T); ++i) {
            h = h * 31 + p[i];
        }
        return h;
    }
};

template <>
struct MyHashHelper<std::string> {
    static unsigned long hashValue(const std::string& key) {
        unsigned long h = 0;
        for (size_t i = 0; i < key.length(); ++i) {
            h = h * 31 + (unsigned char)key[i];
        }
        return h;
    }
};

template <>
struct MyHashHelper<int> {
    static unsigned long hashValue(int key) {
        return (unsigned long)(key < 0 ? -key : key);
    }
};

template <>
struct MyHashHelper<double> {
    static unsigned long hashValue(double key) {
        if (key < 0) key = -key;
        return (unsigned long)(key * 1000);
    }
};


// ============================================================================
//  MyMap<K, V>  —  Bảng băm (thay thế std::unordered_map)
// ============================================================================
template <typename K, typename V>
class MyMap {
private:
    // Mỗi nút trong danh sách liên kết (chaining)
    struct Node {
        K key;
        V value;
        Node* next;
        Node(const K& k, const V& v) : key(k), value(v), next(nullptr) {}
    };

    Node** buckets;    // Mảng các con trỏ đầu danh sách
    int bucket_count;  // Số lượng bucket
    int sz;            // Số cặp key-value

    // Hàm băm: chuyển key thành chỉ số bucket
    unsigned long hash(const K& key) const {
        return MyHashHelper<K>::hashValue(key) % bucket_count;
    }

    // Xóa toàn bộ danh sách
    void clearBuckets() {
        for (int i = 0; i < bucket_count; ++i) {
            Node* cur = buckets[i];
            while (cur != nullptr) {
                Node* temp = cur;
                cur = cur->next;
                delete temp;
            }
            buckets[i] = nullptr;
        }
    }

public:
    // ---------- Constructor & Destructor ----------
    MyMap(int initial_buckets = 16)
        : bucket_count(initial_buckets), sz(0) {
        buckets = new Node*[bucket_count];
        for (int i = 0; i < bucket_count; ++i) {
            buckets[i] = nullptr;
        }
    }

    MyMap(const MyMap& other) : bucket_count(other.bucket_count), sz(0) {
        buckets = new Node*[bucket_count];
        for (int i = 0; i < bucket_count; ++i) {
            buckets[i] = nullptr;
        }
        // Sao chép dữ liệu
        for (int i = 0; i < bucket_count; ++i) {
            Node* cur = other.buckets[i];
            while (cur != nullptr) {
                insert(cur->key, cur->value);
                cur = cur->next;
            }
        }
    }

    ~MyMap() {
        clearBuckets();
        delete[] buckets;
    }

    MyMap& operator=(const MyMap& other) {
        if (this != &other) {
            clearBuckets();
            delete[] buckets;
            bucket_count = other.bucket_count;
            sz = 0;
            buckets = new Node*[bucket_count];
            for (int i = 0; i < bucket_count; ++i) {
                buckets[i] = nullptr;
            }
            for (int i = 0; i < bucket_count; ++i) {
                Node* cur = other.buckets[i];
                while (cur != nullptr) {
                    insert(cur->key, cur->value);
                    cur = cur->next;
                }
            }
        }
        return *this;
    }

    // ---------- Thao tác cơ bản ----------
    void insert(const K& key, const V& value) {
        unsigned long idx = hash(key);
        Node* cur = buckets[idx];

        // Kiểm tra key đã tồn tại chưa
        while (cur != nullptr) {
            if (cur->key == key) {
                cur->value = value; // Cập nhật value
                return;
            }
            cur = cur->next;
        }

        // Thêm nút mới vào đầu danh sách
        Node* new_node = new Node(key, value);
        new_node->next = buckets[idx];
        buckets[idx] = new_node;
        ++sz;
    }

    // Tìm kiếm: trả về true nếu tìm thấy, lưu value vào out
    bool find(const K& key, V& out) const {
        unsigned long idx = hash(key);
        Node* cur = buckets[idx];
        while (cur != nullptr) {
            if (cur->key == key) {
                out = cur->value;
                return true;
            }
            cur = cur->next;
        }
        return false;
    }

    // Kiểm tra key có tồn tại không
    bool contains(const K& key) const {
        unsigned long idx = hash(key);
        Node* cur = buckets[idx];
        while (cur != nullptr) {
            if (cur->key == key) {
                return true;
            }
            cur = cur->next;
        }
        return false;
    }

    // Toán tử []: lấy value theo key (tự động tạo nếu chưa có)
    V& operator[](const K& key) {
        unsigned long idx = hash(key);
        Node* cur = buckets[idx];

        while (cur != nullptr) {
            if (cur->key == key) {
                return cur->value;
            }
            cur = cur->next;
        }

        // Key chưa tồn tại → tạo mới
        Node* new_node = new Node(key, V());
        new_node->next = buckets[idx];
        buckets[idx] = new_node;
        ++sz;
        return new_node->value;
    }

    // Xóa một key
    bool remove(const K& key) {
        unsigned long idx = hash(key);
        Node* cur = buckets[idx];
        Node* prev = nullptr;

        while (cur != nullptr) {
            if (cur->key == key) {
                if (prev == nullptr) {
                    buckets[idx] = cur->next;
                } else {
                    prev->next = cur->next;
                }
                delete cur;
                --sz;
                return true;
            }
            prev = cur;
            cur = cur->next;
        }
        return false;
    }

    int size() const { return sz; }
    bool empty() const { return sz == 0; }

    // ---------- Iterator đơn giản (chỉ hỗ trợ duyệt toàn bộ) ----------
    class Iterator {
    private:
        Node** buckets;
        int bucket_count;
        int current_bucket;
        Node* current_node;

        void advanceToNextValid() {
            while (current_node == nullptr && current_bucket < bucket_count) {
                ++current_bucket;
                if (current_bucket < bucket_count) {
                    current_node = buckets[current_bucket];
                }
            }
        }

    public:
        Iterator(Node** b, int bc, int start_bucket, Node* start_node)
            : buckets(b), bucket_count(bc), current_bucket(start_bucket), current_node(start_node) {
            advanceToNextValid();
        }

        MyPair<K, V> operator*() const {
            return MyPair<K, V>(current_node->key, current_node->value);
        }

        Iterator& operator++() {
            if (current_node != nullptr) {
                current_node = current_node->next;
            }
            advanceToNextValid();
            return *this;
        }

        bool operator!=(const Iterator& other) const {
            return current_node != other.current_node;
        }
    };

    Iterator begin() {
        return Iterator(buckets, bucket_count, 0, buckets[0]);
    }

    Iterator end() {
        return Iterator(buckets, bucket_count, bucket_count, nullptr);
    }

    // Lấy tất cả các key (cho trường hợp cần duyệt key)
    MyVector<K> keys() const {
        MyVector<K> result;
        for (int i = 0; i < bucket_count; ++i) {
            Node* cur = buckets[i];
            while (cur != nullptr) {
                result.push_back(cur->key);
                cur = cur->next;
            }
        }
        return result;
    }

    // Lấy tất cả các value
    MyVector<V> values() const {
        MyVector<V> result;
        for (int i = 0; i < bucket_count; ++i) {
            Node* cur = buckets[i];
            while (cur != nullptr) {
                result.push_back(cur->value);
                cur = cur->next;
            }
        }
        return result;
    }
};

// ============================================================================
//  THUẬT TOÁN TÌM KIẾM  —  Binary Search
// ============================================================================

// Binary Search trên mảng đã SẮP XẾP (tăng dần)
// Trả về index của phần tử tìm thấy, hoặc -1 nếu không có
template <typename T>
int myBinarySearch(const T arr[], int size, const T& target) {
    int low = 0;
    int high = size - 1;

    while (low <= high) {
        int mid = low + (high - low) / 2; // Tránh tràn số

        if (arr[mid] == target) {
            return mid;
        } else if (arr[mid] < target) {
            low = mid + 1;
        } else {
            high = mid - 1;
        }
    }
    return -1; // Không tìm thấy
}

// Binary Search — overload cho MyVector (đã sắp xếp)
template <typename T>
int myBinarySearch(const MyVector<T>& vec, const T& target) {
    return myBinarySearch(vec.begin(), vec.size(), target);
}


// ============================================================================
//  THUẬT TOÁN TÌM KIẾM TUẦN TỰ  —  Linear Search
// ============================================================================

// Tìm kiếm tuần tự (không yêu cầu sắp xếp)
template <typename T>
int myLinearSearch(const T arr[], int size, const T& target) {
    for (int i = 0; i < size; ++i) {
        if (arr[i] == target) {
            return i;
        }
    }
    return -1;
}

template <typename T>
int myLinearSearch(const MyVector<T>& vec, const T& target) {
    return myLinearSearch(vec.begin(), vec.size(), target);
}

#endif // MY_DATA_STRUCTURES_H