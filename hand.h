#ifndef HAND_H
#define HAND_H

// #include <gtkmm.h>
#include "hand.h"
#include "card.h"
#include <iostream>
#include <string> 
class Hand
{
  public:
    Hand();
  private:
    std::string card_1;
    std::string card_2;
    std::string card_3;
    std::string card_4;
    std::string card_5;
};

#endif

