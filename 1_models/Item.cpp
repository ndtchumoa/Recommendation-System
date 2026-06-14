#include "Item.h"

// --- Constructors ---

Item::Item()
    : item_id(""), name(""), category(""), badge(""),
      price(0.0), star(0.0), sold(0), image_url("") {}

Item::Item(std::string id, std::string n, std::string cat, std::string bdg,
           double p, double st, int s, std::string img)
    : item_id(id), name(n), category(cat), badge(bdg),
      price(p), star(st), sold(s), image_url(img) {}

// --- Getters ---

std::string Item::getItemId() const { return item_id; }
std::string Item::getName() const { return name; }
std::string Item::getCategory() const { return category; }
std::string Item::getBadge() const { return badge; }
double Item::getPrice() const { return price; }
double Item::getStar() const { return star; }
int Item::getSold() const { return sold; }
std::string Item::getImageUrl() const { return image_url; }

// --- Setters ---

void Item::setName(const std::string& n) { name = n; }
void Item::setCategory(const std::string& cat) { category = cat; }
void Item::setBadge(const std::string& bdg) { badge = bdg; }
void Item::setPrice(double p) { price = p; }
void Item::setStar(double st) { star = st; }
void Item::setSold(int s) { sold = s; }
void Item::setImageUrl(const std::string& img) { image_url = img; }

// --- displayInfo ---

void Item::displayInfo() const {
    std::cout << "Item ID: " << item_id
              << " | Name: " << name
              << " | Category: " << category
              << " | Badge: " << badge
              << " | Price: " << price
              << " | Star: " << star
              << " | Sold: " << sold << std::endl;
}