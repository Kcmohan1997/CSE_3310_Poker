#ifndef __DEALER_H
#define __DEALER_H
#include <bits/stdc++.h> 
#include "dealer.h"
#include "Player.h"
#include <iostream>


//Class Derivation
class Dealer{
  public:
    void Shuffle(std::string card[], int n) 
    {	 
        // Initialize seed randomly 
        srand(time(0)); 
  
        for (int i=0; i<n ;i++) 
        { 
           // Random for remaining positions. 
           int r = i + (rand() % (52 -i)); 
           swap(card[i], card[r]); 
        }  
    } 

    }
    std::string Swap_Cards();
    bool Ante_paid();
    int get_numPlayer();
    bool Check_num_player();
    std::string Deal_cards();
    bool betting_round_Check();
    int add_money_pot(int betting_amount)
    {
	Money_pot += betting_amount;		
    }
    bool has_player_bet();
    bool see_player_check();
  private:
    int Money_Pot=0;
    int ante;
    int betting_round_counter;
    int numPlayer;
    int betting_amount;
    std::string Deck_Cards[] = std::string aDeck[52] = {"Ace of Spades", "Two of Spades", "Three of Spades", "Four of Spades", "Five of Spades", "Six of Spades", "Seven of Spades","Eight of Spades", "Nine of Spades", "Ten of Spades", "Jack of Spades", "Queen of Spades", "King of Spades", "Ace of Clubs", "Two of Clubs", "Three of Clubs", "Four of Clubs", "Five of Clubs", "Six of Clubs", "Seven of Clubs", "Eight of Clubs", "Nine of Clubs", "Ten of Clubs", "Jack of Clubs", "Queen of Clubs", "King of Clubs","Ace of Diamonds", "Two of Diamonds", "Three of Diamonds", "Four of Diamonds", "Five of Diamonds", "Six of Diamonds", "Seven of Diamonds", "Eight of Diamonds", "Nine of Diamonds", "Ten of Diamonds", "Jack of Diamonds", "Queen of Diamonds", "King of Diamonds", "Ace of Hearts", "Two of Hearts", "Three of Hearts", "Four of Hearts", "Five of Hearts", "Six of Hearts",
 "Seven of Hearts", "Eight of Hearts", "Nine of Hearts", "Ten of Hearts", "Jack of Hearts", "Queen of Hearts", "King of Hearts"};
    
};

#endif
