#include<iostream>
#include<stdlib.h>
#include"dealer.h"
#include"hand.h"
//#include"client.h"
void Dealer::Shuffle(std::string card[], int n) 
    {	 
        // Initialize seed randomly 
        srand(time(0)); 
  
        for (int i=0; i<n ;i++) 
        { 
           // Random for remaining positions. 
           int r = i + (rand() % (52 -i)); 
           swap(card[i], card[r]); 
        }  
        deck_index =0;
    } 

void Dealer::Swap_Cards(std::vector<std::string> hand, std::vector<int> swaps, std::string deck[])
{
    for(int i=0; i < swaps.size(); i++)
        {
            hand[swaps[i]] = Deal_cards(deck);
        }
}

bool Dealer::Ante_paid(std::vector<std::string> player)
{
    if(player.size() == 0)
    return false;
    else
    return true;
}
int Dealer::get_numPlayer(std::vector<std::string> player)
{
    int players = player.size();
    return players;
}
bool Dealer::Check_num_player(std::vector<std::string> player)
{
    if(player.size() < 2 || player.size() > 5)
    return false;
    else
    return true;
}



std::string Dealer::Deal_cards(std::string deck[])
{
    if (deck_index > 52)
    {
        deck_index = 0;
    }
    return deck[deck_index++];
}
/*
bool Dealer::betting_round_Check();
{

}
*/
int Dealer::add_money_pot(int betting_amount)
    {
	money_pot += betting_amount;
        return money_pot;		
    }
/*
bool Dealer::has_player_bet();
{

}
bool Dealer::see_player_check();
{

}
*/
int main(int argc, char* argv[]) {
    Dealer dealer;
    std::string Deck[52] = {"Ace of Spades", "Two of Spades", "Three of Spades", "Four of Spades", "Five of Spades", "Six of Spades", "Seven of Spades","Eight of Spades", "Nine of Spades", "Ten of Spades", "Jack of Spades", "Queen of Spades", "King of Spades", "Ace of Clubs", "Two of Clubs", "Three of Clubs", "Four of Clubs", "Five of Clubs", "Six of Clubs", "Seven of Clubs", "Eight of Clubs", "Nine of Clubs", "Ten of Clubs", "Jack of Clubs", "Queen of Clubs", "King of Clubs","Ace of Diamonds", "Two of Diamonds", "Three of Diamonds", "Four of Diamonds", "Five of Diamonds", "Six of Diamonds", "Seven of Diamonds", "Eight of Diamonds", "Nine of Diamonds", "Ten of Diamonds", "Jack of Diamonds", "Queen of Diamonds", "King of Diamonds", "Ace of Hearts", "Two of Hearts", "Three of Hearts", "Four of Hearts", "Five of Hearts", "Six of Hearts",
 "Seven of Hearts", "Eight of Hearts", "Nine of Hearts", "Ten of Hearts", "Jack of Hearts", "Queen of Hearts", "King of Hearts"};
    std::cout << "Before Shuffle" << std::endl;
    std::cout << dealer.Deal_cards(Deck) << std::endl;
    std::cout << dealer.Deal_cards(Deck) << std::endl;
    std::cout << dealer.Deal_cards(Deck) << std::endl;
    std::cout << dealer.Deal_cards(Deck) << std::endl;
    std::cout << dealer.Deal_cards(Deck) << std::endl;
    dealer.Shuffle(Deck, 52);
    std::cout << "After Shuffle" << std::endl;
    std::cout << dealer.Deal_cards(Deck) << std::endl;
    std::cout << dealer.Deal_cards(Deck) << std::endl;
    std::cout << dealer.Deal_cards(Deck) << std::endl;
    std::cout << dealer.Deal_cards(Deck) << std::endl;
    std::cout << dealer.Deal_cards(Deck) << std::endl;
    std::vector<std::string> Deck2;
    std::cout << "Testing with vector" << std::endl;
    Deck2.push_back(dealer.Deal_cards(Deck));
    for(int i=0; i < Deck2.size(); i++)
      std::cout << Deck2.at(i) << std::endl;
    return 0;
}

