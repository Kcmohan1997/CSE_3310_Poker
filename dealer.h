#ifndef __DEALER_H
#define __DEALER_H
#include <bits/stdc++.h>
#include "dealer.h"
#include <iostream>
#include <cstring>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <vector>

struct cards_info
{
	std::string Card_Suit[5];
	int card_number[5];
};

//Class Derivation
class Player {
private:
	std::string player_uuid;
	std::string player_name;
	int ante;

public:
	Player(std::string, std::string, int);
	//virtual ~ Player();
	std::string get_player_uuid();
	std::string get_player_name();
	int get_ante();
	int get_bet_Amount();
	std::string get_input();
};

//Class Derivation
class Dealer {
public:
	//int player1_point =0;

	std::vector<Player> player_list;
	std::vector <std::string> dealt_cards_player1;
	std::vector <std::string> dealt_cards_player2;
	std::vector <std::string> dealt_cards_player3;
	std::vector <std::string> dealt_cards_player4;
	std::vector <std::string> dealt_cards_player5;
	int winner_list[5];//interger value is stored if -1 means folded 
	void Shuffle(std::string card[52]);
	void Swap_Cards(std::vector<std::string>& new_card, int, int, int, int, int, int);
	void store_fold(int winner_list[5], int);
	int get_numPlayer();
	bool Check_num_player2();
	bool Check_num_player3();
	bool betting_round_Check();
	int find_best_hand(std::vector<std::string> PlayerHand);
	int add_money_pot(int);
	//bool has_player_bet();
	bool has_player_check();
	std::vector<std::string> deal_card(std::string deck[52], int, int);
	int money_pot = 0;
	int betting_round_counter = 0;
	int numPlayer;
	int bet_amount;
	int deck_index;
	std::string Deck_cards[52] = { "C_2", "D_2", "H_2", "S_2", "C_3", "D_3", "H_3","S_3", "C_4", "D_4", 
		"H_4", "S_4", "C_5", "D_5", "H_5", "S_5", "C_6", "D_6", "H_6", "S_6", "C_7", "D_7", "H_7", "S_7", 
		"C_8", "D_8","H_8", "S_8", "C_9", "D_9", "H_9", "S_9", "C_10", "D_10", "H_10", "S_10", "C_A", "D_A", 
		"H_A", "S_A", "C_J", "D_J", "H_J", "S_J", "C_Q","D_Q","H_Q","S_Q","C_K","D_K","H_K","S_K" };

	//keep tracks of taken card
	bool card_Taken[52] = { false, false, false, false,false, false, false, false,false, false, false, false,false, false, false, false,
	false, false, false, false, false, false, false, false,false, false, false, false,false, false, false, false, false, false, false, false,
	 false, false, false, false,false, false, false, false,	false, false, false, false,false,false,false,false };


};

#endif
