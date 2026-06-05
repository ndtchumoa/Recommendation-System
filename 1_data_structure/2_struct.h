#cấu trúc: user: userid, timestamp
product: id
score được tính bằng interation: userid, product id, click, add_cart, purchase, buy, rating
2_struct.h

struct User
{
    int user_id;
    string name;
    string created_at;
};

struct Product
{
    int product_id;
    string name;
};

struct Interaction
{
    int item_id;
    int user_id;

    int click;
    int add_cart;
    int purchase;

    double rating;
};