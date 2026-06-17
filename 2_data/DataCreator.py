import pandas as pd
import random
from faker import Faker
from datetime import datetime

fake = Faker("vi_VN")

# ==============================
# CẤU HÌNH
# ==============================
NUM_USERS = 10000
NUM_ITEMS = 500
NUM_INTERACTIONS = 100000

print("Đang sinh dữ liệu...")

# ==============================
# USERS
# ==============================
users = []

import string

for i in range(1, NUM_USERS + 1):

    email = fake.email()

    password = ''.join(
        random.choices(
            string.ascii_letters +
            string.digits,
            k=12
        )
    )

    users.append([
        f"U{i:05d}",
        fake.name(),
        fake.date_between(
            start_date=datetime(2024,1,1),
            end_date=datetime(2026,1,1)
        ),
        email,
        password
    ])

df_users = pd.DataFrame(
    users,
    columns=[
        "user_id",
        "name",
        "created_at",
        "email",
        "password"
    ]
)

# ==============================
# ITEMS
# ==============================
categories = [
    "Electronics",
    "Computer Accessories",
    "Phones & Tablets",
    "Fashion Men",
    "Fashion Women",
    "Shoes",
    "Beauty",
    "Health",
    "Books",
    "Office",
    "Home Appliances",
    "Kitchen",
    "Furniture",
    "Sports",
    "Outdoor",
    "Pet Supplies",
    "Baby",
    "Food",
    "Beverage",
    "Toys",
    "Gaming",
    "Automotive",
    "Motorbike Accessories",
    "Jewelry",
    "Travel"
]

products = {
    "Electronics": [
        "Laptop Dell XPS 15",
        "Laptop Asus ROG Strix",
        "MacBook Air M3",
        "MacBook Pro M4",
        "Samsung Smart TV 55 inch",
        "LG OLED TV",
        "Sony Bravia TV",
        "Apple Watch Series 10",
        "Samsung Galaxy Watch 7",
        "Tai nghe Sony WH-1000XM5",
        "Loa JBL Charge 6",
        "Camera Canon EOS R50",
        "Camera Sony A6700"
    ],

    "Phones & Tablets": [
        "iPhone 16",
        "iPhone 16 Pro Max",
        "Samsung Galaxy S25 Ultra",
        "Xiaomi 15 Pro",
        "OPPO Find X8",
        "iPad Air M3",
        "iPad Pro M4",
        "Samsung Galaxy Tab S10"
    ],

    "Computer Accessories": [
        "Ban phim Logitech MX Keys",
        "Chuot Logitech MX Master 3S",
        "Ban phim co Razer BlackWidow",
        "SSD Samsung 990 Pro",
        "RAM Kingston Fury 16GB",
        "Webcam Logitech C920",
        "Router TP-Link AX3000"
    ],

    "Fashion Men": [
        "Ao Polo Uniqlo",
        "Ao So Mi Nam Aristino",
        "Quan Jean Levi's 511",
        "Ao Khoac Bomber",
        "Ao Thun Cotton Basic"
    ],

    "Fashion Women": [
        "Dam Cong So",
        "Chan Vay Midi",
        "Ao So Mi Nu",
        "Ao Len Cardigan",
        "Tui Xach Zara"
    ],

    "Shoes": [
        "Nike Air Max 270",
        "Adidas Ultraboost 5",
        "New Balance 530",
        "Puma RS-X",
        "Giay Da Nam"
    ],

    "Beauty": [
        "Son MAC Ruby Woo",
        "Kem Chong Nang Anessa",
        "Sua Rua Mat Cetaphil",
        "Nuoc Hoa Dior Sauvage",
        "Mat Na Innisfree"
    ],

    "Health": [
        "Vitamin C Blackmores",
        "Omega 3 Kirkland",
        "May Do Huyet Ap Omron",
        "Can Dien Tu Xiaomi"
    ],

    "Books": [
        "Dac Nhan Tam",
        "Atomic Habits",
        "Clean Code",
        "Python Cookbook",
        "Deep Learning",
        "Design Patterns",
        "Nha Gia Kim"
    ],

    "Office": [
        "But Bi Thien Long",
        "So Tay Moleskine",
        "May In HP LaserJet",
        "May Huy Tai Lieu"
    ],

    "Home Appliances": [
        "May Giat LG Inverter",
        "Tu Lanh Samsung",
        "May Lanh Daikin",
        "May Hut Bui Dyson"
    ],

    "Kitchen": [
        "Noi Chien Khong Dau Philips",
        "Am Sieu Toc Sunhouse",
        "Noi Com Dien Toshiba",
        "May Xay Sinh To Panasonic"
    ],

    "Furniture": [
        "Ban Lam Viec Go",
        "Ghe Cong Thai Hoc",
        "Ke Sach 5 Tang",
        "Tu Quan Ao 3 Canh"
    ],

    "Sports": [
        "May Chay Bo Kingsport",
        "Xe Dap Giant Escape",
        "Vot Cau Long Yonex",
        "Tham Yoga Adidas"
    ],

    "Outdoor": [
        "Leu Cam Trai 4 Nguoi",
        "Den Pin Sieu Sang",
        "Binh Nuoc Du Lich"
    ],

    "Pet Supplies": [
        "Thuc An Cho Meo Whiskas",
        "Thuc An Cho Cho Pedigree",
        "Cat Ve Sinh Me-O"
    ],

    "Baby": [
        "Ta Pampers",
        "Sua Bot Aptamil",
        "Xe Day Cho Be"
    ],

    "Food": [
        "Ca Phe Arabica",
        "Hat Dieu Rang Muoi",
        "Ngu Coc Dinh Duong",
        "Socola Den"
    ],

    "Beverage": [
        "Tra Xanh Lipton",
        "Nuoc Ep Cam",
        "Sua Hat Oc Cho"
    ],

    "Toys": [
        "Lego City",
        "Rubik 3x3",
        "Xe Dieu Khien Tu Xa"
    ],

    "Gaming": [
        "Tay Cam PS5",
        "Tai Nghe Gaming HyperX",
        "Ban Phim Gaming Corsair"
    ],

    "Automotive": [
        "Camera Hanh Trinh VietMap",
        "Bom Lop O To Michelin",
        "Gia Do Dien Thoai O To"
    ],

    "Motorbike Accessories": [
        "Mu Bao Hiem Fullface",
        "Gang Tay Xe May",
        "Ao Mua Cao Cap"
    ],

    "Jewelry": [
        "Nhan Bac 925",
        "Day Chuyen Bac",
        "Lac Tay Vang 18K"
    ],

    "Travel": [
        "Vali Keo Lock&Lock",
        "Balo Du Lich NatureHike",
        "Tui Xach Du Lich"
    ]
}

price_ranges = {
    "Electronics": (5000000, 80000000),
    "Phones & Tablets": (3000000, 50000000),
    "Computer Accessories": (100000, 10000000),
    "Fashion Men": (100000, 3000000),
    "Fashion Women": (100000, 5000000),
    "Shoes": (300000, 6000000),
    "Beauty": (50000, 3000000),
    "Health": (100000, 5000000),
    "Books": (30000, 500000),
    "Office": (10000, 10000000),
    "Home Appliances": (500000, 50000000),
    "Kitchen": (100000, 10000000),
    "Furniture": (500000, 30000000),
    "Sports": (100000, 50000000),
    "Outdoor": (100000, 10000000),
    "Pet Supplies": (50000, 3000000),
    "Baby": (50000, 10000000),
    "Food": (10000, 500000),
    "Beverage": (10000, 300000),
    "Toys": (50000, 5000000),
    "Gaming": (100000, 10000000),
    "Automotive": (100000, 10000000),
    "Motorbike Accessories": (50000, 5000000),
    "Jewelry": (500000, 100000000),
    "Travel": (100000, 10000000)
}

items = []
item_counter = 1

while len(items) < NUM_ITEMS:
    category = random.choice(list(products.keys()))

    name = f"{random.choice(products[category])} - SKU{random.randint(1000,9999)}"

    low, high = price_ranges[category]

    items.append([
        f"I{item_counter:05d}",
        name,
        category,
        random.randint(low, high)
    ])

    item_counter += 1

df_items = pd.DataFrame(
    items,
    columns=["item_id", "name", "category", "price"]
)

# ==============================
# INTERACTIONS
# ==============================
user_ids = df_users["user_id"].tolist()
item_ids = df_items["item_id"].tolist()

interactions = []

while len(interactions) < NUM_INTERACTIONS:

    user_id = random.choice(user_ids)
    item_id = random.choice(item_ids)

    click = random.choices(
        [0,1,2,3,4,5],
        weights=[10,30,25,15,10,10]
    )[0]

    add_cart = random.randint(0, click) if click > 0 else 0
    purchase = random.randint(0, add_cart) if add_cart > 0 else 0

    rating = (
        random.choices([1,2,3,4,5],
                       weights=[5,5,15,35,40])[0]
        if purchase > 0 and random.random() < 0.3
        else None
    )

    if click == 0 and add_cart == 0 and purchase == 0:
        continue

    interactions.append([
        user_id,
        item_id,
        click,
        add_cart,
        purchase,
        rating
    ])

df_interactions = pd.DataFrame(
    interactions,
    columns=[
        "user_id",
        "item_id",
        "click",
        "add_cart",
        "purchase",
        "rating"
    ]
)
    
# ==============================
# XUẤT FILE CSV
# ==============================
df_users.to_csv("4_dataset/users.csv", index=False)
df_items.to_csv("4_dataset/items.csv", index=False)
df_interactions.to_csv("4_dataset/interactions.csv", index=False)

print("Hoàn thành!")
print(f"Users: {len(df_users):,}")
print(f"Items: {len(df_items):,}")
print(f"Interactions: {len(df_interactions):,}")