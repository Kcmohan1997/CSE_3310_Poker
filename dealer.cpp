#include<iostream>
#include<stdlib.h>
#include"dealer.h"
#include"hand.h"
//#include"client.h"
void Dealer::Shuffle(std::string card[], int n) //just added setting deck index to 0
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

void Dealer::Swap_Cards(std::vector<std::string> hand, std::vector<int> swaps, std::string deck[]) ///assuming we will create a vector for hand, store the desired swaps after parsing the entry, and have a deck made (could not figure out how to manipulate this private deck)
{
    for(int i=0; i < swaps.size(); i++)
        {
            hand[swaps[i]] = Deal_cards(deck);
        }
}

bool Dealer::Ante_paid(std::vector<int> ante, std::vector<std::string> player ) //Idea is when ante is paid we fill a vector if the size of ante is the same as players it's true
{
    if(ante.size() == get_numPlayer(player))
    return true;
    else
    return false;
}
int Dealer::get_numPlayer(std::vector<std::string> player) //when players enter their name we will push the name on a vector, size of vector is size of player
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
std::vector<std::string> Dealer::getHand(std::string deck[]) //made a vector for hand
{
    std::vector<std::string> Hand;
    for (int i =0; i<5; i++)
    {
        Hand.push_back(Deal_cards(deck));
    }
    return Hand;
}


std::string Dealer::Deal_cards(std::string deck[]) //Index up the deck until it reaches max then reset, assuming shuffle will happen before as at max only 50 cards can be dealt
{
    if (deck_index > 52)
    {
        deck_index = 0;
    }
    return deck[deck_index++];
}

bool Dealer::betting_round_Check(std::vector<std::string> player)
{
    if (betting_round_counter = get_numPlayer(player))
    return true;
    else return false;
    betting_round_counter = 0; /// reset counter for next round
}

int Dealer::add_money_pot(int betting_amount)
    {
	money_pot += betting_amount;
        betting_round_counter++;
        return money_pot;		
    }
/*
bool Dealer::has_player_bet();
{

}
bool Dealer::has_player_check();
{

}
*/
/*
int main(int argc, char* argv[]) {

Testing prints.

Dealer dealer;
std::string Deck[52] = {"Ace of Spades", "Two of Spades", "Three of Spades", "Four of Spades", "Five of Spades", "Six of Spades", "Seven of Spades","Eight of Spades", "Nine of Spades", "Ten of Spades", "Jack of Spades", "Queen of Spades", "King of Spades", "Ace of Clubs", "Two of Clubs", "Three of Clubs", "Four of Clubs", "Five of Clubs", "Six of Clubs", "Seven of Clubs", "Eight of Clubs", "Nine of Clubs", "Ten of Clubs", "Jack of Clubs", "Queen of Clubs", "King of Clubs","Ace of Diamonds", "Two of Diamonds", "Three of Diamonds", "Four of Diamonds", "Five of Diamonds", "Six of Diamonds", "Seven of Diamonds", "Eight of Diamonds", "Nine of Diamonds", "Ten of Diamonds", "Jack of Diamonds", "Queen of Diamonds", "King of Diamonds", "Ace of Hearts", "Two of Hearts", "Three of Hearts", "Four of Hearts", "Five of Hearts", "Six of Hearts",
 "Seven of Hearts", "Eight of Hearts", "Nine of Hearts", "Ten of Hearts", "Jack of Hearts", "Queen of Hearts", "King of Hearts"};
    std::vector<std::string> Deck2 = dealer.getHand(Deck);
    for(int i=0; i < Deck2.size(); i++)
      std::cout << Deck2.at(i) << std::endl;
 std::vector<std::string> Deck3 = dealer.getHand(Deck);
    for(int i=0; i < Deck3.size(); i++)
      std::cout << Deck3.at(i) << std::endl;
 std::vector<std::string> Deck4 = dealer.getHand(Deck);
    for(int i=0; i < Deck4.size(); i++)
      std::cout << Deck4.at(i) << std::endl;

    return 0;
}
*/

