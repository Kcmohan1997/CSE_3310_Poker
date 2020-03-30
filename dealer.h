#ifndef __DEALER_H
#define __DEALER_H

#include "dealer.h"
#include <iostream>


//Class Derivation
class Dealer{
  public:
    void Shuffle();
    std::string Swap_Cards();
    bool Ante_paid();
    int get_numPlayer();
    bool Check_num_player();
    std::string Deal_cards();
    bool betting_round_Check();
    int add_money_pot(int betting_amount);
    bool has_player_bet();
    bool see_player_check();
  private:
    int Money_Pot;
    int ante;
    int betting_round_counter;
    int numPlayer;
    int betting_amount;
    std::string Deck_Cards[];
    
};

#endif
