#ifndef __PLAYER_H
#define __PLAYER_H

// #include <gtkmm.h>
#include "hand.h"
#include "player.h"
#include "Dealer.h"
#include <iostream>

class Player
{
  private:
    int PlayerID;
    Hand Player_Hand;
    std::string message;
    int money = 100;
    bool Swap_Allowed;
    bool AmountOfCards;

  public:
    int Get_Money()
    {
	return money;
    }
    int Get_Message();
    int Get_PlayerID()
    {
	    return PlayerID;
    }
    int Add_To_Money(int money,Dealer.money_pot)
    {
	money += Dealer.money_pot;
    }
    Hand Set_Hand(Hand);
    Hand Get_Hand(Hand);
    bool Check_Swap();
    bool Swap_Cards(Hand Player_Hand);
    int Check(int money);
    int Bet(int money);
    void Fold(Hand);
    bool Check_Hand(Hand);
    int Set_Player_Id();
    int Amount_Of_Cards();
    int Set_Money();
    std::string Chat_Box();
};

#endif
