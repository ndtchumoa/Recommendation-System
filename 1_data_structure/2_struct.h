//cấu trúc: user: userid, timestamp
//product: id
//score được tính bằng interation: userid, product id, click, add_cart, purchase, buy, rating
#pragma once
#include <string>       
using namespace std;
struct User
{
    int user_id;
    string name;
    string created_at;
};

struct Item
{
    int item_id;
    string name;
    string category;
    float price;
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
