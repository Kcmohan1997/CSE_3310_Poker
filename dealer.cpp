#include <string>
#include "dealer.h"

//dealer constructor
Player::Player(std::string player_uuid, std::string player_name, int ante)
{
	this->player_uuid = player_uuid;
	this->player_name = player_name;
	this->ante = ante;
}

//getter function to get player_id
std::string Player::get_player_uuid()
{
	return player_uuid;
}

//getter function to get player name
std::string Player::get_player_name()
{
	return player_name;
}


int Player::get_ante()
{
	return ante;
}

void Dealer::Shuffle(std::string card[52]) //just added setting deck index to 0
{
	// Initialize seed randomly
	srand(time(0));

	for (int i = 0; i < 52; i++)
	{
		// Random for remaining positions.
		int r = i + (rand() % (52 - i));
		swap(card[i], card[r]);
	}
	deck_index = 0;
}


void Dealer::Swap_Cards(std::vector<std::string>& new_card, int index, int card1, int card2, int card3, int card4, int card5) //index of player and card is passed
{
	/////////////////swaps card as preferred to the right player
	for (int i = 0; i < 5; i++)
	{
		if (card1 != -1)
		{
			new_card[0] = Deck_cards[index];
			card_Taken[index] = true;
			index++;
			card1 = -1;
		}
		else if (card2 != -1)
		{
			new_card[1] = Deck_cards[index];
			card_Taken[index] = true;
			index++;
			card2 = -1;
		}
		else if (card3 != -1)
		{
			new_card[2] = Deck_cards[index];
			card_Taken[index] = true;
			index++;
			card3 = -1;
		}
		else if (card4 != -1)
		{
			new_card[3] = Deck_cards[index];
			card_Taken[index] = true;
			index++;
			card4 = -1;
		}
		else if (card5 != -1)
		{
			new_card[4] = Deck_cards[index];
			card_Taken[index] = true;
			index++;
			card5 = -1;
		}
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//return new_card;
}

int Dealer::get_numPlayer() //when players enter their name we will push the name on a vector, size of vector is size of player
{
	int players = player_list.size();
	return players;
}


bool Dealer::Check_num_player2()
{
	if (player_list.size() < 2)
		return false;
	else
		return true;
}

bool Dealer::Check_num_player3()
{
	if (player_list.size() > 2)
		return true;
	else
		return false;
}

///deals card
std::vector<std::string> Dealer::deal_card(std::string deck[52], int player_index, int player_count) //made a vector for hand
{
	std::vector<std::string> Hand;
	int card_given_count = 0;
	int card_index = 0;
	bool all_card_dealt = true;

	///////////////////////////////////////////////////////////////////////////////////////////////////
	/////////deals card to the if there is 2 player on the server////////////////////////////////////////
	if (player_count == 2)
	{
		/////////deals card to the first player from left to right////////////////////////////////////////
		if (player_index == 1)
		{
			while (all_card_dealt)
			{
				card_Taken[card_index] = true;
				Hand.push_back(deck[card_index]);
				card_given_count++;
				card_index += 2;
				if (card_given_count == 5)
				{
					all_card_dealt = false;
				}
			}
		}

		//deals card to the second player from left to right
		else if (player_index == 2)
		{
			card_index = 1;
			while (all_card_dealt)
			{
				card_Taken[card_index] = true;
				Hand.push_back(deck[card_index]);
				card_given_count++;
				card_index += 2;
				if (card_given_count == 5)
				{
					all_card_dealt = false;
				}
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////////////////

	////////////////////deals 3 players from left to right/////////////////////////////////////////////////
	else if (player_count == 3)
	{
		//deals card to the first player from left to right
		if (player_index == 1)
		{
			while (all_card_dealt)
			{
				card_Taken[card_index] = true;
				Hand.push_back(deck[card_index]);
				card_given_count++;
				card_index += 3;
				if (card_given_count == 5)
				{
					all_card_dealt = false;
				}
			}
		}

		//deals card to the second player from left to right
		else if (player_index == 2)
		{
			card_index = 1;
			while (all_card_dealt)
			{
				card_Taken[card_index] = true;
				Hand.push_back(deck[card_index]);
				card_given_count++;
				card_index += 3;
				if (card_given_count == 5)
				{
					all_card_dealt = false;
				}
			}
		}
		//deals card to the third player from left to right
		else if (player_index == 3)
		{
			card_index = 2;
			while (all_card_dealt)
			{
				card_Taken[card_index] = true;
				Hand.push_back(deck[card_index]);
				card_given_count++;
				card_index += 3;
				if (card_given_count == 5)
				{
					all_card_dealt = false;
				}
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////////////////////////////////
	return Hand;
}

//gets player index from the player class from server and store -1 in the same index
void Dealer::store_fold(int winner_list[5], int player_index)
{
	winner_list[player_index] = -1;
}

//////////////////////////////////////////////////////////////////////
///checks the betting round upto two round ///////////////////////////
//when it reaches two round then winner is decided///////////////////
/////////////////////////////////////////////////////////////////////
bool Dealer::betting_round_Check()///****need to check this part
{
	bool status;
	if (player_list.size() <= 2)
	{
		if (betting_round_counter == 4)
			status = true;
		else
			status = false;
	}
	else if (player_list.size() >= 3)
	{
		if (betting_round_counter == 6)
			status = true;
		else
			status = false;
	}
	return status;
}

int Dealer::add_money_pot(int amount)
{
	money_pot += amount;
	return money_pot;
}

//////////////////////////////////////////////////////////////////////////////////////////////
// checks player hand for possible combination and assigns value to the winner_list    ///////
// high card has high value and pushed in the same order as the player joins the game  ///////
//////////////////////////////////////////////////////////////////////////////////////////////
int Dealer::find_best_hand(std::vector <std::string> PlayerHand)
{
	int high_card_value = 0;
	bool royal_flush = false, straight_flush = false, Four_of_Kind = false, Full_house = false, flush = false, straight = false;
	bool three_kind = false, two_pair = false, pair = false, highCard = false;
	int highCard_counter = 0;
	int straight_counter = 0;
	int counter = 0;
	cards_info cards_info;
	int Buckets[14];
	for (int n = 0; n < 14; n++)
	{
		Buckets[n] = 0;
	}
	for (int i = 0; i < 5; i++)
	{

		std::string temp = PlayerHand[i];
		int lengthstr = temp.length();
		char card[lengthstr + 1];
		strcpy(card, temp.c_str());
		char* ptr;
		char* card_s;
		ptr = strtok(card, "_");
		card_s = ptr;
		//std::cout <<"this is PTR  "<<ptr << '\n';
		//std::cout<<card_s<<"\n";
		if (strcmp(card_s, "C") == 0)
		{
			cards_info.Card_Suit[i] = card_s;
		}
		else if (strcmp(card_s, "D") == 0)
		{
			cards_info.Card_Suit[i] = card_s;
		}
		else if (strcmp(card_s, "H") == 0)
		{
			cards_info.Card_Suit[i] = card_s;
		}
		else if (strcmp(card_s, "S") == 0)
		{
			cards_info.Card_Suit[i] = card_s;
		}
		//  std::cout <<cards_info.Card_Suit[i] << '\n';
		while (ptr != NULL)
		{
			ptr = strtok(NULL, " ");
			//std::cout << ptr << '\n';
			if (atoi(ptr))
			{
				cards_info.card_number[counter] = atoi(ptr);
				//std::cout << cards_info.card_number[counter] << '\n';
				counter++;
			}
			else
			{
				if (strcmp(ptr, "K") == 0)
				{
					cards_info.card_number[counter] = 13;
					counter++;
				}
				else if (strcmp(ptr, "Q") == 0)
				{
					cards_info.card_number[counter] = 12;
					counter++;
				}
				else if (strcmp(ptr, "J") == 0)
				{
					cards_info.card_number[counter] = 11;
					counter++;
				}
			}
			ptr = strtok(NULL, " ");
		}


	}
	//this is starting to fill up bucket
	for (int j = 0; j < 14; j++)
	{
		int temp = cards_info.card_number[j];
		if (temp == 1)
		{
			Buckets[1] = Buckets[1] + 1;
		}
		else if (temp == 2)
		{
			Buckets[2] = Buckets[2] + 1;
		}
		else if (temp == 3)
		{
			Buckets[3] = Buckets[3] + 1;
		}
		else if (temp == 4)
		{
			Buckets[4] = Buckets[4] + 1;
		}
		else if (temp == 5)
		{
			Buckets[5] = Buckets[5] + 1;
		}
		else if (temp == 6)
		{
			Buckets[6] = Buckets[6] + 1;
		}
		else if (temp == 7)
		{
			Buckets[7] = Buckets[7] + 1;
		}
		else if (temp == 8)
		{
			Buckets[8] = Buckets[8] + 1;
		}
		else if (temp == 9)
		{
			Buckets[9] = Buckets[9] + 1;
		}
		else if (temp == 10)
		{
			Buckets[10] = Buckets[10] + 1;
		}
		else if (temp == 11)
		{
			Buckets[11] = Buckets[11] + 1;
		}
		else if (temp == 12)
		{
			Buckets[12] = Buckets[12] + 1;
		}
		else if (temp == 13)
		{
			Buckets[13] = Buckets[13] + 1;
		}
	}
	//this is checking the hands
	for (int k = 0; k < 14; k++)
	{
		if (Buckets[k] != 0)
		{

			if (Buckets[k] == 4)
			{
				Four_of_Kind = true;
			}
			else if (Buckets[k] == 3)
			{

				for (int j = 0; j < 5; j++)
				{
					if (cards_info.card_number[j] == 2)
					{
						Full_house = true;
					}
				}
				if (Full_house != true)
				{
					three_kind = true;
				}
			}
			else if (Buckets[k] == 2)
			{
				for (int j = 0; j < 5; j++)
				{
					if (cards_info.card_number[j] == 3)
					{
						//Full_house
						Full_house = true;
					}
					else if (cards_info.card_number[j] == 2)
					{
						//TWO pairs of pairs
						two_pair = true;
					}
				}
				if (two_pair == false)
				{
					pair = true;
				}
				if (Full_house == true)
				{
					pair = false;
				}
			}
			else if (Buckets[k] == 1)
			{
				highCard = true;
				highCard_counter++;
				//std::cout << "highCard_counter::" <<highCard_counter <<'\n';
				if ((highCard == true))
				{
					for (int j = 0; j < 5; j++)
					{
						int temp = cards_info.card_number[j];
						if ((temp < cards_info.card_number[j + 1]) && (temp + 1 == cards_info.card_number[j + 1]))
						{
							straight_counter++;
							if (straight_counter == 5)
							{
								//std::cout << "this is in if stright true" << '\n';
								straight = true;
								highCard = false;

							}
						}

					}
					if (straight == true)
					{
						break;
					}
				}
			}
			//std::cout << "this is card number "<<k<<"::";
			//std::cout << Buckets[k] << '\n';
		}
	}

	//returning corresponding value to the high card from 1 to 10
	if (royal_flush)
	{
		high_card_value = 10;
	}
	else if (straight_flush)
	{
		high_card_value = 9;
	}
	else if (Four_of_Kind)
	{
		high_card_value = 8;
	}
	else if (Full_house)
	{
		high_card_value = 7;
	}
	else if (flush)
	{
		high_card_value = 6;
	}
	else if (straight)
	{
		high_card_value = 5;
	}
	else if (three_kind)
	{
		high_card_value = 4;
	}
	else if (two_pair)
	{
		high_card_value = 3;
	}
	else if (pair)
	{
		high_card_value = 2;
	}
	else
	{
		high_card_value = 1;
	}



	return high_card_value;
}
