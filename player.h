#ifndef PLAYER_H
#define PLAYER_H

// #include <gtkmm.h>

class Player
{
  private:
    int PlayerID;
    Hand Player_Hand;
    string message;
    int money;
    bool Swap_Allowed;
    bool Amount_Of_Cards;

  public:
    int Get_Money();
    int Get_Message();
    int Get_PlayerID();
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
    int Amount_Of_Cards();
    int Set_Money();
    string Chat_Box();
};

#endif
