#include "Coin.h"
#include "LinkedList.h"
#include "Input.h"
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#define RESET   "\033[0m"
//#define colourGreen   "\033[32m"
//#define colourRed "\033[31m"
// #define YELLOW "\033[33m"

bool colourEnhancement = false;
std::string colourGreen;
std::string colourRed;
std::string colourYellow;


// Find an item in the list by item ID.
// Returns an iterator to the item if found, otherwise returns an iterator to the end of the list.
LinkedList::Iterator findItemByID(LinkedList &itemsList, const std::string &itemId);

// Remove leading and trailing whitespace from a string.
// std::string &str_trim(std::string &str);

// Check if a string contains only digits.
// bool is_valid_number(const std::string &str);

// Save and Quit functionality for stock.dat
void saveAndQuit_stock(LinkedList &itemsList);
// Save and Quit functionality for coins.dat
void saveAndQuit_coins(const std::vector<Coin> &coins);

// Split a string into tokens using the given delimiters.
// std::vector<std::string> tokenize(const std::string &str, const std::string &delims);

// Print main menu
void mainMenu(LinkedList &itemsList, std::vector<Coin> &coins);

// Print Itemlist
void printItemsList(LinkedList &itemsList);

// Reset Stock Count
void resetStockConut(LinkedList &list1, LinkedList &list2);

// Reset Coin Count
void resetCoinCount(std::vector<Coin> &coinList, const std::vector<Coin> &defaultCoinList);

// Print Coin Summary
void printCoinSummary(const std::vector<Coin> &coins);

// When user enters '2' from main menu:
void purchaseItem(LinkedList &itemsList, std::vector<Coin>& coinVector);
// After user selects item, the payment process begins:
bool payForItem(int price, std::vector<Coin>& coinVector);
// Converts cents to dollars in a string with format: $XX.XX
std::string centsToDollars(int cents);
// Converts cents to dollars in a string with format: XX.XX (FOR FILE OUTPUT ONLY)
std::string centsToDollars_FileOutput(int cents);

// Gives the user change if applicable
bool giveChange(std::vector<Coin>& coinVector, int changeDue);

// Add an item to this system.
void addItem(LinkedList &itemsList);
// Remove an item from a category and delete it from the system, including free memory that is no longer being used.
void removeItem(LinkedList &itemsList);

//Enable/Disable enhancements:
void enableEnhancements();

// Load the stock data from the given file.
// Returns true if the stock data was loaded successfully, otherwise false.
bool load_stock(LinkedList &stocklist, const char *stockfile)
{
    // open the stock file
    std::ifstream file(stockfile);
    if (!file.is_open())
    {
        std::cerr << "Error: cannot open file '" << stockfile << "'." << std::endl;
        return false;
    }

    // initialise the stock list
    stocklist.clear();

    std::string line;
    while (std::getline(file, line))
    {
        // Format:  [ItemID]|[ItemName]|[ItemDesc]|[Price]|[NumberOnHand]
        // Example: I0001|Meat Pie|Yummy Beef in Gravy surrounded by pastry|3.50|50

        // split the line into tokens
        std::vector<std::string> tokens = tokenize(line, "|");

        // check if we have the correct number of tokens
        if (tokens.size() != 5)
        {
            std::cerr << "Error: invalid stock line '" << line << "'." << std::endl;
            return false;
        }

        std::string price_str = str_trim(tokens[3]);
        std::string on_hand_str = str_trim(tokens[4]);

        // check if the price is valid
        std::vector<std::string> price_tokens = tokenize(price_str, ".");
        if (price_tokens.size() != 2 || !is_valid_number(price_tokens[0]) || !is_valid_number(price_tokens[1]))
        {
            std::cerr << "Error: invalid price '" << price_str << "'." << std::endl;
            return false;
        }

        // check if the number on hand is valid
        if (!is_valid_number(on_hand_str))
        {
            std::cerr << "Error: invalid number on hand '" << on_hand_str << "'." << std::endl;
            return false;
        }

        unsigned dollars = std::stoi(price_tokens[0]);
        unsigned cents = std::stoi(price_tokens[1]);

        unsigned on_hand = std::stoi(on_hand_str);

        // create the stock
        Stock *stock = new Stock{str_trim(tokens[0]),
                                 str_trim(tokens[1]),
                                 str_trim(tokens[2]),
                                 Price(dollars, cents),
                                 on_hand};

        // add the stock to the list
        stocklist.insert(stock);
    }

    return true;
}

// Load the coins data from the given file.
// Returns true if the coins data was loaded successfully, otherwise false.
bool load_coins(std::vector<Coin> &coins, const char *coinsfile)
{
    // open the coins file
    std::ifstream file(coinsfile);
    if (!file.is_open())
    {
        std::cerr << "Error: cannot open file '" << coinsfile << "'." << std::endl;
        return false;
    }

    std::vector<bool> seen(NUM_DENOMINATIONS, false);

    // initialise the coins vector
    coins = std::vector<Coin>(NUM_DENOMINATIONS);
    for (int i = 0; i < NUM_DENOMINATIONS; ++i)
    {
        coins[i].count = 0;
        coins[i].denom = (Denomination)i;
    }

    std::string line;
    while (std::getline(file, line))
    {
        // Example: 1000,3

        // find the comma
        size_t comma = line.find(',');
        if (comma == std::string::npos)
        {
            std::cerr << "Error: invalid coin line '" << line << "'." << std::endl;
            return false;
        }

        // get the denomination
        std::string denom_str = line.substr(0, comma);
        // get the count
        std::string count_str = line.substr(comma + 1);

        // trim whitespace
        str_trim(denom_str);
        str_trim(count_str);

        // check if the denomination and count are valid numbers
        if (!is_valid_number(denom_str) || !is_valid_number(count_str))
        {
            std::cerr << "Error: invalid coin line '" << line << "'." << std::endl;
            return false;
        }

        int denom = std::stoi(denom_str);
        unsigned int count = std::stoi(count_str);

        enum Denomination denom_type = get_denom_type(denom);

        // check if the denomination is valid
        if (denom_type == (Denomination)-1)
        {
            std::cerr << "Error: invalid denomination '" << denom << "'." << std::endl;
            return false;
        }

        // check if we have already seen this denomination
        if (seen[denom_type])
        {
            std::cerr << "Error: duplicate denomination '" << denom << "'." << std::endl;
            return false;
        }

        coins[denom_type].count = count;
        seen[denom_type] = true;
    }

    return true;
}

// Print Main Menu
void mainMenu(LinkedList &itemsList, std::vector<Coin> &coins)
{

    std::string input;
    int choice = 0;
    std::vector<Coin> originCoins = coins;

    LinkedList originList;

    // Duplicate the itemslist so the original can be used to restock items
    for (LinkedList::Iterator it = itemsList.begin(); it != itemsList.end(); ++it) {
        Stock *stock = new Stock(*it); 
        // Add the duplicate Stock  to the new list
        originList.insert(stock);
    }



while (true)
{
        std::cout << "Main Menu:" << std::endl;
        std::cout << "\t1." << "Display Items" << std::endl;
        std::cout << "\t2." << "Purchase Items" <<  std::endl;
        std::cout << "\t3." << "Save and Exit" << std::endl;
        std::cout << "Administrator-Only Menu:" << std::endl;
        std::cout << "\t4." << "Add Item" << std::endl;
        std::cout << "\t5." << "Remove Item" << std::endl;
        std::cout << "\t6." << "Display Coins" << std::endl;
        std::cout << "\t7." << "Reset Stock" << std::endl;
        std::cout << "\t8." << "Reset Coins" << std::endl;
        std::cout << "\t9." << "Abort Program" << std::endl;
        std::cout << "\t10." << "Enable Enhancements" << std::endl;
        std::cout << "Select your option (1-10): ";

    getline(std::cin, input);

    if (input != "10" && (input.size() != 1 || input[0] < '1' || input[0] > '9'))  // check if input is invalid
    {
        std::cout << "Invalid option. Please try again." << std::endl << std::endl;
        continue;  // go back to beginning of loop to display menu message and prompt user again
    }

    choice = stoi(input);

    if (choice == 1)
    {
        std::cout << "Display Items" << std::endl;
        printItemsList(itemsList);
    }
    else if (choice == 2)
    {
        std::cout << "Purchase Items" << std::endl;
        purchaseItem(itemsList, coins);
    }
    else if (choice == 3)
    {
        std::cout << "Save and Exit" << std::endl;
        saveAndQuit_stock(itemsList);
        saveAndQuit_coins(coins);
        exit(0);
    }
    else if (choice == 4)
    {
        std::cout << "Add Item" << std::endl;
        addItem(itemsList);
    }
    else if (choice == 5)
    {
        std::cout << "Remove Item" << std::endl;
        removeItem(itemsList);

    }
    else if (choice == 6)
    {
        std::cout << "Display Coins" << std::endl;
        printCoinSummary(coins);
    }
    else if (choice == 7)
    {
        std::cout << "Reset Stock" << std::endl;
        resetStockConut(itemsList, originList);
    }
    else if (choice == 8)
    {
        std::cout << "Reset Coins" << std::endl;
        resetCoinCount(coins, originCoins);
    }
    else if (choice == 9)
    {
        std::cout << "Abort Program" << std::endl;
        exit(0);
    }else if (choice == 10)
    {
        std::cout << "You entered 10" << std::endl;
        enableEnhancements();
    }


    std::cout << std::endl;
    }
}

// Print stock info
void printItemsList(LinkedList &itemsList)
{
    std::cout << std::left << std::setw(5) << std::setfill(' ') << "ID" << "| " 
              << std::left << std::setw(29) << std::setfill(' ') << "Name" << "|" 
              << std::right << std::setw(10) << std::setfill(' ') << "Available" << "|" 
              << std::right << std::setw(6) << std::setfill(' ') << "Price" << std::endl;
    std::cout << "-----|------------------------------|----------|------" << std::endl;

    for (LinkedList::Iterator it1 = itemsList.begin(); it1 != itemsList.end(); ++it1) {
        for (LinkedList::Iterator it2 = it1; it2 != itemsList.end(); ++it2) {
            Stock &stock1 = it1.operator*();
            Stock &stock2 = it2.operator*();
            if (stock1.name > stock2.name) {
                std::swap(stock1, stock2);
            }
        }
        Stock currentStock = it1.operator*();
        std::cout << std::left << std::setw(5) << std::setfill(' ') << currentStock.id << "|" 
                  << std::left << std::setw(30) << std::setfill(' ') << currentStock.name << "|"
                  << std::left << std::setw(10) << std::setfill(' ') << currentStock.on_hand << "|";
        int totalcents;
        totalcents = (100*currentStock.price.dollars) + (currentStock.price.cents);
        std::cout << centsToDollars(totalcents) << "" << std::endl;
    }
}


//Req 6: Save and Quit for stock.dat output

void saveAndQuit_stock(LinkedList &itemsList) {

    std::ofstream outfile("testOut_stock.dat");

    for (LinkedList::Iterator saveAndExit_output = itemsList.begin(); saveAndExit_output != itemsList.end(); ++saveAndExit_output) {
        Stock currentStock = saveAndExit_output.operator*();

        int totalcents;
        totalcents = (100*currentStock.price.dollars) + (currentStock.price.cents);

         outfile << std::setfill('|') << currentStock.id << "|" 
                 << std::setfill('|') << currentStock.name << "|"
                 << std::setfill('|') << currentStock.description << "|"
                 << std::setfill('|') << centsToDollars_FileOutput(totalcents) << "|"
                 << std::setfill('|') << currentStock.on_hand << std::endl;

    }
}

// Req6: save and quit for coins.dat output
void saveAndQuit_coins(const std::vector<Coin> &coins)
{

    std::ofstream outfile("testOut_coins.dat");
    for (int denomination : DENOMINATIONS)
    {
        for (const Coin &coin : coins)
        {
            if (coin.denom == get_denom_type(denomination))
            {
                outfile << denomination<< "," << coin.count << std::endl;
            }
        }
    }
}



// Reset Stock Count
void resetStockConut(LinkedList& list1, LinkedList& list2)
{
    for (LinkedList::Iterator it1 = list1.begin(); it1 != list1.end(); ++it1)
    {
        Stock& stock1 = it1.operator*();
        bool found = false;
        LinkedList::Iterator it2 = list2.begin();
        while (!found && it2 != list2.end())
        {
            Stock& stock2 = it2.operator*();
            if (stock1.name == stock2.name)
            {
                stock1.on_hand = stock2.on_hand;
                found = true;
            }
            std::cout << "All stock have been reset to the default level of " << stock2.on_hand << std::endl;
            ++it2;
        }
        if (!found)
        {
            stock1.on_hand = 0;
        }
        std::cout << "All stock have been reset to the default level of " << 0 << std::endl;
    }
}

// Reset Coin Count
void resetCoinCount(std::vector<Coin> &coinList, const std::vector<Coin> &defaultCoinList)
{
    for (size_t i = 0; i < coinList.size(); ++i)
    {
        for (size_t j = 0; j < defaultCoinList.size(); ++j)
        {
            if (coinList[i].denom == defaultCoinList[j].denom)
            {
                coinList[i].count = defaultCoinList[j].count;
                std::cout << "All coins have been reset to the default level of " << defaultCoinList[j].count << std::endl;
            }
        }
    }
}

// Print Coin Summary
void printCoinSummary(const std::vector<Coin> &coins)
{
    std::cout << "Coins Summary\n";
    std::cout << "-------------------------------\n";
    std::cout << "Denomination |           Count\n";
    std::cout << "--------------------------------\n";
    for (int denomination : DENOMINATIONS)
    {
        for (const Coin &coin : coins)
        {
            if (coin.denom == get_denom_type(denomination))
            {
                printf("%-12d |%16d\n", denomination, coin.count);
                break;
            }
        }
    }
}

/**
 * manages the running of the program, initialises data structures, loads
 * data, display the main menu, and handles the processing of options.
 * Make sure free memory and close all files before exiting the program.
 **/
int main(int argc, char **argv)
{
    /* validate command line arguments */
    if (argc != 3)
    {
        std::cerr << "Usage: " << argv[0] << " d <stockfile> <coinsfile>" << std::endl;
        return EXIT_FAILURE;
    }

    /* load the stock file */
    LinkedList stocklist;
    if (!load_stock(stocklist, argv[1]))
    {
        return EXIT_FAILURE;
    }

    /* load the coins file */
    std::vector<Coin> coins;
    if (!load_coins(coins, argv[2]))
    {
        return EXIT_FAILURE;
    }

    mainMenu(stocklist, coins);

    /* display the stock for debugging only */
    std::cout << "Stock:" << std::endl;
    for (LinkedList::Iterator it = stocklist.begin(); it != stocklist.end(); ++it)
    {
        std::cout << it->id << " " << it->name << " " << it->price.dollars << "." << std::setfill('0') << std::setw(2)
                  << it->price.cents << std::setfill(' ') << " " << it->on_hand << std::endl;
    }
    std::cout << std::endl;

    /* display the coins for debugging only */
    std::cout << "Coins:" << std::endl;
    for (int i = 0; i < NUM_DENOMINATIONS; ++i)
    {
        std::cout << DENOMINATIONS[coins[i].denom] << " " << coins[i].count << std::endl;
    }

    return EXIT_SUCCESS;
}

// std::string &str_trim(std::string &str)
// {
//     // remove trailing whitespace
//     while (str.size() && isspace(str.back()))
//     {
//         str.pop_back();
//     }

//     // remove leading whitespace
//     size_t i = 0;
//     while (i < str.size() && isspace(str[i]))
//     {
//         ++i;
//     }
//     str = str.substr(i);

//     return str;
// }

// bool is_valid_number(const std::string &str)
// {
//     for (char c : str)
//     {
//         if (!std::isdigit(c))
//         {
//             return false;
//         }
//     }
//     return true;
// }

// std::vector<std::string> tokenize(const std::string &str, const std::string &delims)
// {
//     std::vector<std::string> tokens;
//     std::string token;
//     for (char c : str)
//     {
//         if (delims.find(c) != std::string::npos)
//         {
//             if (token.size())
//             { // found a token
//                 tokens.push_back(token);
//                 token.clear();
//             }
//         }
//         else
//         {
//             token.push_back(c);
//         }
//     }
//     if (token.size())
//     { // the very last token
//         tokens.push_back(token);
//     }
//     return tokens;
// }

void purchaseItem(LinkedList &itemsList, std::vector<Coin>& coinVector){
    std::string user_input;
    bool itemSelected=false;
    bool itemPurchaseSucess = false;
    bool quit = false;
    do
    {
        std::cout << "Purchase Item" << std::endl;
        std::cout << "-------------" << std::endl;
        //Ask for input while the user does not press enter or ctrl-d
        std::cout << "Please enter the ID of the item you wish to purchase: ";
    
        getline(std::cin, user_input);
        if (std::cin.eof() || user_input.empty() || std::cin.fail()){
            std::cin.clear();
            quit = true;
        }
        else {
            //Iterate through the available items to see if it is available, and if the ID entered is correct
            for (LinkedList::Iterator it = itemsList.begin(); it != itemsList.end(); ++it) {
                Stock& currentStock = *it;
                if (user_input == currentStock.id && currentStock.on_hand > 0) {
                    std::cout << "You have selected \"" << currentStock.name << " - ";
                    std::cout << currentStock.description << "\"";
                    std::cout << " This will cost you $" << currentStock.price.dollars << "." << std::setw(2) << std::setfill('0') << currentStock.price.cents << std::endl;
                    itemSelected = true;
                    //Payment begins...
                    itemPurchaseSucess = payForItem((currentStock.price.dollars * 100)+(currentStock.price.cents), coinVector);
                    
                    if (itemPurchaseSucess)
                    {       //if payforItem returns true, it means the purchase was successful
                            //colourReduce the availability of purchased item:
                        std::cout << "One less " << currentStock.name << " available now." << std::endl;
                        std::cout << currentStock.name << " - previously available: "<< currentStock.on_hand << std::endl;
                        currentStock.on_hand = currentStock.on_hand - 1;
                        std::cout << "Now available: "<< currentStock.on_hand << std::endl;
                    }
                    quit = true;
                }
            }
            if (!itemSelected) {
                std::cout << "Please select a valid ID" << std::endl;
            }
        }
        
    } while (!quit);
    
}

bool payForItem(int price, std::vector<Coin>& coinVector){
    int toBePaid = price;
    int amountPaid;
    bool validAmount = false;
    bool successfulPayment = false;     //to be returned - tracks if payment was successful
    int totalAmountPaid = 0;            //Keeps track of total amount paid incase we need to return the money

    //Booleans used to get input: - eofselected should be true if user presses enter, or if user inputs ctrl+d.
    //                            - validInt should be true once a user enters an integer
    bool eofSelected = false;
    bool validInt = false;
    std::string amountPaidString;

    std::cout << "Price: " << centsToDollars(price) << std::endl;

    do{
        std::cout << "You still need to give us: " << colourGreen << centsToDollars(toBePaid) << ": " << RESET;
        validAmount = false;

        do
        {
            eofSelected = false;
            validInt = false;
            std::getline(std::cin, amountPaidString);
            if (std::cin.eof() || amountPaidString.empty() || std::cin.fail()){
                std::cin.clear();
                eofSelected = true;
            }else 
            {
                if (is_valid_number(amountPaidString))
                {
                    amountPaid = std::stoi(amountPaidString);
                    validInt = true;
                } else {
                    std::cout << colourRed << "\""<< amountPaidString << "\"" << RESET << "is not a valid denomination. Please enter a number: " << RESET;// << std::endl; 
                    validInt = false;
                }
            }
        } while (validInt == false && eofSelected == false);

        if (validInt)
        {
            for (int i = 0; i < NUM_DENOMINATIONS; ++i)
            {
                if (amountPaid == DENOMINATIONS[coinVector[i].denom])
                {
                    toBePaid = toBePaid - amountPaid;
                    validAmount = true;
                    coinVector[i].count = coinVector[i].count + 1; // add coin to vending machine
                    totalAmountPaid = totalAmountPaid + amountPaid;
                }
            }
            if (!validAmount)
            {
                std::cout << colourRed << centsToDollars(amountPaid) << RESET << " is not a valid denomination of money. Please try again." << std::endl;
            }
        }
    } while (toBePaid > 0 && eofSelected == false);

    //If paid amount is below 0, change is due. 
    if (toBePaid == 0)
    {
        successfulPayment = true;
    } else if (toBePaid < 0)
    {
        std::cout <<  " Change due... Please collect your change shortly..." <<std::endl;
        //calls give change function. If returns true, that means it was successful. if false, then unsuccessul.
        successfulPayment = giveChange(coinVector, toBePaid);
    }
    if (successfulPayment == false)
    {
        // return the amount user has paid if it was unsuccessful.
        if (totalAmountPaid > 0)
        {
            //call give Change function which will return the amount that has already been paid.
            totalAmountPaid = totalAmountPaid * -1;
            giveChange(coinVector, totalAmountPaid);
        }
        
    }
    
    return successfulPayment;
}


std::string centsToDollars(int cents)
{
    //Takes in cents and converts it to a string that holds it in dollars $XX.XX
    std::ostringstream dollars;
    dollars << "$" << std::fixed << std::setprecision(2) << static_cast<double>(cents) / 100;
    return dollars.str();
}

std::string centsToDollars_FileOutput(int cents)
{
    //Takes in cents and converts it to a string that holds it in dollars XX.XX
    // Differs from above by removing the "$" for file formatting and output to file.
    std::ostringstream dollars;
    dollars << std::fixed << std::setprecision(2) << static_cast<double>(cents) / 100;
    return dollars.str();
}




bool giveChange(std::vector<Coin>& coinVector, int changeDue){
    changeDue *= -1;                    //convert it to positive value.
    std::cout << "** * Amount due to customer: " << centsToDollars(changeDue) << " * ** "<< std::endl;
    int changeDueCheck = changeDue;     //use this variable to see if it is possible to get exact change
    bool changeGiven = false;

    for (int i = NUM_DENOMINATIONS - 1; i >= 0; --i)
        {
            //check if coin is available:
            if (coinVector[i].count > 0)
            {
                //while the specific coin is larger than or equal too to amount due, and while there are coins available, it should go through
                while (DENOMINATIONS[coinVector[i].denom] <= changeDueCheck && coinVector[i].count > 0)
                {
                    changeDueCheck = changeDueCheck - DENOMINATIONS[coinVector[i].denom];
                }
                
            }
            changeGiven = false;
        }
    if (changeDueCheck == 0){
        std::cout << "Please collect your change: " << colourYellow;
            for (int i = NUM_DENOMINATIONS - 1; i >= 0; --i)
            {
                //If the denomination is available:
                if (coinVector[i].count > 0)
                {
                    //while the specific coin is larger than or equal too to amount due, and while there are coins available, it should go through
                    while (DENOMINATIONS[coinVector[i].denom] <= changeDue && coinVector[i].count > 0){
                        // print the coin:
                        std::cout << "" << centsToDollars(DENOMINATIONS[coinVector[i].denom]) << " "; // << std::endl;
                        changeDue = changeDue - DENOMINATIONS[coinVector[i].denom];
                        // update coins:
                        coinVector[i].count = coinVector[i].count - 1;
                    }
                }
            }
            std::cout << RESET << std::endl;
            changeGiven = true;
    }
    return changeGiven;
}

// Add an item to this system.
// TODO: please rewrite this function
void addItem(LinkedList &itemsList)
{
    // Generate the next available item ID
    int nextItemId = itemsList.size() + 1;
    std::string itemId = "I" + std::string(4 - std::to_string(nextItemId).length(), '0') + std::to_string(nextItemId);

    // Prompt the user for item details
    std::cout << "Enter the item name: ";
    std::string itemName;
    std::getline(std::cin >> std::ws, itemName); // TODO: use Input::getline

    std::cout << "Enter the item description: ";
    std::string itemDesc;
    std::getline(std::cin >> std::ws, itemDesc); // TODO: use Input::getline

    std::cout << "Enter the price for the item: ";
    double itemPrice;
    std::cin >> itemPrice; // TODO: use Input::getline

    // Create the new item and add it to the list
    Stock *stock = new Stock(itemId, itemName, itemDesc, Price(0, 0), 0); // TODO
    itemsList.insert(stock);

    // Print confirmation message
    std::cout << "This item \"" << itemName << " - " << itemDesc << "\" has now been added to the menu." << std::endl;
}

// Remove an item from a category and delete it from the system, including free memory that is no longer being used.
void removeItem(LinkedList &itemsList)
{
    Input input(std::cin);
    bool quit = false;

    while (!quit)
    {
        std::cout << "Enter the item id of the item to remove from the menu: ";
        std::string itemId;
        if (!input.getline(itemId))
        {
            break; // EOF reached
        }

        // find the item in the list
        LinkedList::Iterator it = findItemByID(itemsList, itemId);

        if (it != itemsList.end())
        { // item found
            std::cout << "\"" << it->id << " - " << it->name << " - " << it->description << "\""
                      << " has been removed from the system." << std::endl;

            itemsList.remove(it);

            quit = true;
        }
        else
        { // item not found
            std::cout << "Error: invalid item id, please try again." << std::endl;
        }
    }
}

// Find an item in the list by item ID.
// Returns an iterator to the item if found, otherwise returns an iterator to the end of the list.
LinkedList::Iterator findItemByID(LinkedList &itemsList, const std::string &itemId)
{
    // Iterate through the available items to see if it is available, and if the ID entecolourRed is correct
    for (LinkedList::Iterator it = itemsList.begin(); it != itemsList.end(); ++it)
    {
        Stock &stock = *it;
        if (itemId == stock.id)
        {
            return it;
        }
    }
    return itemsList.end(); // not found
}


void enableEnhancements(){
    
    std::string userInput;
    do{
        //Get Input and check if is correct
        std::cout << std::endl << "Which enhancement would you like to Enable/Disable: " << std::endl;
        std::cout << std::endl << "\t 1. Colour: ";
        if (colourEnhancement)
        {
            std::cout << "Enabled" << std::endl;
        }
        else if (!colourEnhancement)
        {
            std::cout << "Disabled" << std::endl;
        }
        
        
        std::cout << "\t 2. Coming soon... " << std::endl;
        std::cout << "\t 3. Return to menu " << std::endl;
        std::getline(std::cin, userInput);
        std::cout << std::endl;
        if (userInput.size() != 1|| userInput.empty() || std::cin.fail()){
            std::cin.clear();
            std::cout << "Invalid Input. Try again!" << std::endl;
        }else{
            if (userInput == "1")
            {
                std::cout << "You entered   1." << std::endl;
                colourEnhancement = !colourEnhancement;
            }
            else if (userInput == "2"){
                std::cout << "You entecolourRed   2." << std::endl;
            }
            else if (userInput == "3"){
                std::cout << "Returning to Main Menu." << std::endl;
            }
            else{
                std::cout << "Invalid Input. Try again!" << std::endl;
            }
        }
    } while (userInput != "3");

    if (colourEnhancement)
    {
        colourGreen = "\033[32m";
        colourRed = "\033[31m";
        colourYellow = "\033[33m";
    }else if (!colourEnhancement)
    {
        colourGreen = "\033[0m";
        colourRed = "\033[0m";
        colourYellow = "\033[0m";
    }
    


}