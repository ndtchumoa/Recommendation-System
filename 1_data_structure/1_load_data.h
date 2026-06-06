#ifndef LOAD_DATA_H
#define LOAD_DATA_H

#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

#include "2_struct.h"



// =========================
// COUNT LINES
// =========================

inline int countLines(string filename)
{
    ifstream file(filename);

    string line;

    int count = 0;

    while (getline(file, line))
    {
        count++;
    }

    file.close();

    return count;
}



// =========================
// LOAD USERS
// =========================

inline void loadUsers(

    string filename,

    User* users,

    int numUsers
)
{
    ifstream file(filename);

    string line;

    int index = 0;

    while (getline(file, line))
    {
        stringstream ss(line);

        string temp;


        // user_id

        getline(ss, temp, ',');

        users[index].user_id = stoi(temp);


        // name

        getline(ss, users[index].name, ',');


        // created_at

        getline(ss, users[index].created_at, ',');

        index++;
    }

    file.close();
}



// =========================
// LOAD ITEMS
// =========================

inline void loadItems(

    string filename,

    Item* items,

    int numItems
)
{
    ifstream file(filename);

    string line;

    int index = 0;

    while (getline(file, line))
    {
        stringstream ss(line);
        string temp;

        // item_id

        getline(ss, temp, ',');

        items[index].item_id = stoi(temp);


        // item name

        getline(ss,items[index].name,',');

        index++;
    }

    file.close();
}



// =========================
// LOAD INTERACTIONS
// =========================

inline void loadInteractions(
    string filename,
    Interaction* interactions,
    int numInteractions
)
{
    ifstream file(filename);
    string line;

    int index = 0;

    while (getline(file, line))
    {
        stringstream ss(line);
        string temp;

        // item_id

        getline(ss, temp, ',');

        interactions[index].item_id = stoi(temp);

        // user_id

        getline(ss, temp, ',');

        interactions[index].user_id = stoi(temp);

        // click

        getline(ss, temp, ',');

        interactions[index].click = stoi(temp);

        // add_cart

        getline(ss, temp, ',');

        interactions[index].add_cart = stoi(temp);

         // purchase

        getline(ss, temp, ',');

        interactions[index].purchase = stoi(temp);

        // rating

        getline(ss, temp, ',');

        interactions[index].rating = stod(temp);

        index++;
    }

    file.close();
}

#endif


