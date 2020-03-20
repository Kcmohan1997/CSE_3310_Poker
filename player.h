#ifndef PLAYER_H
#define PLAYER_H

// #include <gtkmm.h>

using namespace std;

class Player
{
  public:
    int Player;
    int money;
    Hand Player_Hand;
    int Get_Money();
    int Add_To_Money(int money);
    Hand Set_Hand(Hand);
    Hand Get_Hand(Hand);
    bool Check_Swap();
    bool Swap_Cards(Hand Player_Hand);
    int Check(int money);
    int Bet(int money);
    void Fold(Hand);
    bool Check_Hand(Hand);
    int Set_Player_Id();

  private:
    bool Swap_Allowed;
    bool Amount_Of_Cards;
}

#endif
