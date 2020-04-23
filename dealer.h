#ifndef __DEALER_H
#define __DEALER_H
#include <bits/stdc++.h> 
#include "dealer.h"
//#include "Player.h"
#include <iostream>


//Class Derivation
class Dealer{
  public:
    void Shuffle(std::string card[], int n); 
    void Swap_Cards(std::vector<std::string> hand, std::vector<int> swaps, std::string deck[]);
    bool Ante_paid(std::vector<int> ante, std::vector<std::string> player);
    int get_numPlayer(std::vector<std::string> player);
    bool Check_num_player(std::vector<std::string> player);
    std::string Deal_cards(std::string deck[]);
    bool betting_round_Check();
    int add_money_pot(int betting_amount);
    bool has_player_bet();
    bool has_player_check();
    std::vector<std::string> getHand(std::string deck[]);
  private:
    int money_pot=0;
    int ante;
    int betting_round_counter;
    int numPlayer;
    int betting_amount;
    int deck_index;
    std::string Deck_Cards[52] = {"Ace of Spades", "Two of Spades", "Three of Spades", "Four of Spades", "Five of Spades", "Six of Spades", "Seven of Spades","Eight of Spades", "Nine of Spades", "Ten of Spades", "Jack of Spades", "Queen of Spades", "King of Spades", "Ace of Clubs", "Two of Clubs", "Three of Clubs", "Four of Clubs", "Five of Clubs", "Six of Clubs", "Seven of Clubs", "Eight of Clubs", "Nine of Clubs", "Ten of Clubs", "Jack of Clubs", "Queen of Clubs", "King of Clubs","Ace of Diamonds", "Two of Diamonds", "Three of Diamonds", "Four of Diamonds", "Five of Diamonds", "Six of Diamonds", "Seven of Diamonds", "Eight of Diamonds", "Nine of Diamonds", "Ten of Diamonds", "Jack of Diamonds", "Queen of Diamonds", "King of Diamonds", "Ace of Hearts", "Two of Hearts", "Three of Hearts", "Four of Hearts", "Five of Hearts", "Six of Hearts",
 "Seven of Hearts", "Eight of Hearts", "Nine of Hearts", "Ten of Hearts", "Jack of Hearts", "Queen of Hearts", "King of Hearts"};
    
};

#endif
