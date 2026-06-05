#include "Interaction.h"
#include <iostream>
#include <iomanip>

Interaction::Interaction(const std::string& user_id,
                         const std::string& item_id,
                         int click,
                         int add_cart,
                         int purchase,
                         double rating)
    : user_id(user_id),
      item_id(item_id),
      click(click),
      add_cart(add_cart),
      purchase(purchase),
      rating(rating) {}

double Interaction::computeScore() const {
    return static_cast<double>(click)    * W_CLICK
         + static_cast<double>(add_cart) * W_ADD_CART
         + static_cast<double>(purchase) * W_PURCHASE
         + rating                        * W_RATING;
}

void Interaction::print() const {
    std::cout << std::fixed << std::setprecision(1);
    std::cout << "[Interaction]"
              << " User="     << user_id
              << " | Item="   << item_id
              << " | Click="  << click
              << " | Cart="   << add_cart
              << " | Buy="    << purchase
              << " | Stars="  << rating
              << " | Score="  << computeScore()
              << "\n";
}
