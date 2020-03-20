#ifndef __RANKING_H
#define __RANKING_H

#include "ranking.h"
#include "hand.h"
#include "player.h"
#include <iostream>

class Ranking {
  public:
    void card_Ranking();
    bool Who_Won();
    void get_playerHand(Hand);
  private:
    Hand Player_1;
    Hand Player_2;
    Hand Player_3;
    Hand Player_4;
    Hand Player_5;
    Player Winner;
};

#endif
