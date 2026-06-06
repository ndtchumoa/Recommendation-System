//phải có sự phân loại user để dùng timestamp rồi gợi ý 

#include "1_data_structure/2_struct.h"

#include "1_data_structure/1_load_data.h"

//#include "1_data_structure/3_matrix.h"

//#include "2_similarity/cosine.h"

//#include "3_recommendation/item-item.h"

int main()
{
    //count data
    int numUsers = countLines("raw/users.csv");
    int numItems = countLines("raw/items.csv");
    int numInteractions = countLines("raw/interactions.csv");

    //tạo bộ nhớ động
    User* users = new User[numUsers];
    Item* items = new Item[numItems];
    Interaction* interactions = new Interaction[numInteractions];
    
    //load data
    loadUsers("raw/users.csv",
                users,
                numUsers);
    loadItems("raw/items.csv",
                items,
                numItems);
    loadInteractions("raw/interactions.csv",
                      interactions,
                      numInteractions);

}

/*if (isNewUser(userIndex, ratingMatrix))
{
    recommendTrendingProducts(
        ratingMatrix,
        products,
        numProducts,
        topK
    );
}
else
{
    recommendProducts(
        userIndex,
        ratingMatrix,
        itemSimilarityMatrix,
        products,
        numProducts,
        topK
    );
}*/