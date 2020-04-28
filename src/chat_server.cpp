//
// chat_server.cpp
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2018 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <cstdlib>
#include <deque>
#include <iostream>
#include <list>
#include <memory>
#include <set>
#include <utility>
#include "asio.hpp"
#include "chat_message.hpp"
#include "dealer.h"
#include "json.hpp"

///////////////////////////////////
//setting value to track the game
//float win_credit = 0;			
//float betted_amount = 0;		
int player_count = 0;
int player_number = 0;
//////////////////////////////////

//dealer object
Dealer dealer;

using asio::ip::tcp;

//----------------------------------------------------------------------

typedef std::deque<chat_message> chat_message_queue;

//----------------------------------------------------------------------


class chat_participant
{
public:
	virtual ~chat_participant() {}
	virtual void deliver(const chat_message& msg) = 0;
};

typedef std::shared_ptr<chat_participant> chat_participant_ptr;

//----------------------------------------------------------------------

//----------------------------------------------------------------------


//----------------------------------------------------------------------
class chat_room
{
public:

	// this is where  the player joins the server
	void join(chat_participant_ptr participant)
	{
		participants_.insert(participant);
		for (auto msg : recent_msgs_)
			participant->deliver(msg);
	}

	void leave(chat_participant_ptr participant)
	{
		participants_.erase(participant);
	}

	void deliver(const chat_message& msg)
	{
		recent_msgs_.push_back(msg);
		while (recent_msgs_.size() > max_recent_msgs)
			recent_msgs_.pop_front();

		for (auto participant : participants_)
			participant->deliver(msg);
	}


private:
	std::set<chat_participant_ptr> participants_;
	enum { max_recent_msgs = 100 };
	chat_message_queue recent_msgs_;
};



//----------------------------------------------------------------------

class chat_session
	: public chat_participant,
	public std::enable_shared_from_this<chat_session>
{
public:
	chat_session(tcp::socket socket, chat_room& room)
		: socket_(std::move(socket)),
		room_(room)
	{
		player_count++;
		std::cout << "Creating a Poker++ Player " << player_count << std::endl;
	}

	//only happens when client joins the server
	void start()
	{
		room_.join(shared_from_this());
		do_read_header();
	}

	void deliver(const chat_message& msg)
	{
		bool write_in_progress = !write_msgs_.empty();
		write_msgs_.push_back(msg);
		if (!write_in_progress)
		{
			do_write();
		}
	}

private:
	void do_read_header()
	{
		auto self(shared_from_this());
		asio::async_read(socket_,
			asio::buffer(read_msg_.data(), chat_message::header_length),
			[this, self](std::error_code ec, std::size_t /*length*/)
			{
				if (!ec && read_msg_.decode_header())
				{
					do_read_body();
				}
				else
				{
					room_.leave(shared_from_this());
				}
			});
	}




	void do_read_body()
	{
		auto self(shared_from_this());
		asio::async_read(socket_,
			asio::buffer(read_msg_.body(), read_msg_.body_length()),
			[this, self](std::error_code ec, std::size_t /*length*/)
			{
				if (!ec)
				{
					//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					////////////////message received from the client/////////////////////////////////////////////////////////////////////
					////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					std::string client_msg(read_msg_.body());
					client_msg = client_msg.substr(0, read_msg_.body_length());

					nlohmann::json from_client = nlohmann::json::parse(client_msg);
					nlohmann::json to_player;

					////checking the player type when player hits play button and enters info
					if (from_client["mode"] == "player")
					{
						player_number++;
						std::cout << from_client["player_name"] << " has Joined the game. \n" << std::endl;
						Player player(from_client["uuid"], from_client["player_name"], from_client["ante"]);
						dealer.player_list.push_back(player);
						dealer.add_money_pot(from_client["ante"]);
						to_player["uuid"] = from_client["uuid"];
						to_player["player_name"] = from_client["player_name"];
						to_player["player_count"] = player_number;
						to_player["status"] = "joined";
						std::string t = to_player.dump();
						chat_message sending;
						memcpy(sending.body(), t.c_str(), t.size());
						sending.body_length(t.size());
						sending.encode_header();
						room_.deliver(sending);
						if (dealer.Check_num_player2() && player_count == 2)
						{
							//shuffling three time to make sure deck is shuffled well
							dealer.Shuffle(dealer.Deck_cards);
							dealer.Shuffle(dealer.Deck_cards);
							dealer.Shuffle(dealer.Deck_cards);

							//deals card to two player and saves in vector if there is two player in the server
							dealer.dealt_cards_player1 = dealer.deal_card(dealer.Deck_cards, 1, player_count);
							dealer.dealt_cards_player2 = dealer.deal_card(dealer.Deck_cards, 2, player_count);

							//sending cards to the player 1
							to_player["start_game"] = "start";
							to_player["uuid"] = dealer.player_list[0].get_player_uuid();
							to_player["player_name"] = dealer.player_list[0].get_player_name();
							to_player["bet_button"] = "true";
							to_player["player_count"] = player_count;
							to_player["card"] = dealer.dealt_cards_player1;
							std::string a = to_player.dump();
							chat_message sending1;
							memcpy(sending1.body(), a.c_str(), a.size());
							sending1.body_length(a.size());
							sending1.encode_header();
							room_.deliver(sending1);

							//sending cards to the player 2
							to_player["start_game"] = "start";
							to_player["uuid"] = dealer.player_list[1].get_player_uuid();
							to_player["player_name"] = dealer.player_list[1].get_player_name();
							to_player["player_count"] = player_count;
							to_player["bet_button"] = "false";
							to_player["card"] = dealer.dealt_cards_player2;
							std::string b = to_player.dump();
							chat_message sending2;
							memcpy(sending2.body(), b.c_str(), b.size());
							sending2.body_length(b.size());
							sending2.encode_header();
							room_.deliver(sending2);

						}

						///////////deals cards to three players if there is threee players in the server////////////
						else if (dealer.Check_num_player3() && player_count == 3)
						{
							dealer.Shuffle(dealer.Deck_cards);
							dealer.Shuffle(dealer.Deck_cards);
							dealer.Shuffle(dealer.Deck_cards);
							std::cout << "DECK has been shuffled. NOW dealing to the Players\n" << std::endl;

							//deals card to two player and saves in vector
							dealer.dealt_cards_player1 = dealer.deal_card(dealer.Deck_cards, 1, player_count);
							dealer.dealt_cards_player2 = dealer.deal_card(dealer.Deck_cards, 2, player_count);
							dealer.dealt_cards_player3 = dealer.deal_card(dealer.Deck_cards, 3, player_count);

							//sending cards to the player 1
							to_player["start_game"] = "start";
							to_player["uuid"] = dealer.player_list[0].get_player_uuid();
							to_player["player_name"] = dealer.player_list[0].get_player_name();
							to_player["player_count"] = player_count;
							to_player["bet_button"] = "true";
							to_player["card"] = dealer.dealt_cards_player1;
							std::string a = to_player.dump();
							chat_message sending1;
							memcpy(sending1.body(), a.c_str(), a.size());
							sending1.body_length(a.size());
							sending1.encode_header();
							room_.deliver(sending1);

							//sending cards to the player 2
							to_player["start_game"] = "start";
							to_player["uuid"] = dealer.player_list[1].get_player_uuid();
							to_player["player_name"] = dealer.player_list[1].get_player_name();
							to_player["player_count"] = player_count;
							to_player["bet_button"] = "false";
							to_player["card"] = dealer.dealt_cards_player2;
							std::string b = to_player.dump();
							chat_message sending2;
							memcpy(sending2.body(), b.c_str(), b.size());
							sending2.body_length(b.size());
							sending2.encode_header();
							room_.deliver(sending2);

							//sending cards to the player 3
							to_player["start_game"] = "start";
							to_player["uuid"] = dealer.player_list[2].get_player_uuid();
							to_player["player_name"] = dealer.player_list[2].get_player_name();
							to_player["player_count"] = player_count;
							to_player["bet_button"] = "false";
							to_player["card"] = dealer.dealt_cards_player3;
							std::string c = to_player.dump();
							chat_message sending3;
							memcpy(sending3.body(), c.c_str(), c.size());
							sending3.body_length(c.size());
							sending3.encode_header();
							room_.deliver(sending3);

						}
					}
					/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					///////////////////////////////implements when the player clicks bet button//////////////////////////////////////
					/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					else if (from_client["action"] == "bet")
					{
						std::cout << from_client["player_name"] << " has betted " << from_client["bet_amount"] << "\n" << std::endl;
						dealer.betting_round_counter++;
						//find the index of the player who wants to swap
						int find_player = 0;

						//finding index of the player in the vector to update game status accordingly
						for (std::size_t i = 0; i < dealer.player_list.size(); i++)
						{
							if (from_client["uuid"] == dealer.player_list[i].get_player_uuid())
							{
								find_player = i;
								break;
							}
						}

						//sending final results back to the player
						dealer.add_money_pot(from_client["bet_amount"]);
						to_player["uuid"] = from_client["uuid"];
						to_player["player_name"] = from_client["player_name"];
						to_player["action"] = "betted";
						to_player["bet_amount"] = from_client["bet_amount"];
						to_player["player_count"] = player_count;

						/////////////////////////////////////////////////////////////////////////////////////////////////////
						//////non better uuid send to the client to enable bet button/////////////////////////////////////////
						//////*its for two player game to disable bet button according to the round*///////////////////////////
						//////////////////////////////////////////////////////////////////////////////////////////////////////
						if (find_player == 0)
						{
							to_player["non_better_uuid"] = dealer.player_list[1].get_player_uuid();
						}
						if (find_player == 1)
						{
							to_player["non_better_uuid"] = dealer.player_list[0].get_player_uuid();
						}

						to_player["player_index"] = find_player;
						std::string t = to_player.dump();
						chat_message sending;
						memcpy(sending.body(), t.c_str(), t.size());
						sending.body_length(t.size());
						sending.encode_header();
						room_.deliver(sending);


						///////////////////////////////////////////////////////////////////////////////////////////////////
						////////////////////betting round check ////////////////////////////////////////////////////////////
						//when betting reaches *two* round, winner is decided and pot money is sent to the winning player///
						////////////////////////////////////////////////////////////////////////////////////////////////////
						bool decide_winner = dealer.betting_round_Check();
						if (decide_winner)
						{
							std::cout << "Betting round is complete. Now Deceiding the Winner\n" << std::endl;
							if (dealer.player_list.size() == 2)
							{

								if (dealer.winner_list[0] != -1)//checking if player has folded or not
								{
									int player1_hand_value = dealer.find_best_hand(dealer.dealt_cards_player1);
									dealer.winner_list[0] = player1_hand_value;
									std::cout << "player1 card Value: " << player1_hand_value << std::endl;
								}
								if (dealer.winner_list[1] != -1)//checking if the second player has folded or not
								{
									int player2_hand_value = dealer.find_best_hand(dealer.dealt_cards_player2);
									dealer.winner_list[1] = player2_hand_value;
									std::cout << "player 2 card Value: " << player2_hand_value << std::endl;
								}
								//std::cout<<"this is a check"<<std::endl;
							}

							if (dealer.winner_list[0] > dealer.winner_list[1])
							{
								//sending winner uuid and pot value;
								to_player["winner_uuid"] = dealer.player_list[0].get_player_uuid();
								to_player["game_status"] = "winner_decided";
								to_player["pot_money"] = dealer.money_pot;
								to_player["winner_index"] = 0;
								to_player["looser_index"] = 1;
								to_player["player_count"] = player_count;
								to_player["looser_uuid"] = dealer.player_list[1].get_player_uuid();
								std::string t = to_player.dump();
								chat_message sending;
								memcpy(sending.body(), t.c_str(), t.size());
								sending.body_length(t.size());
								sending.encode_header();
								room_.deliver(sending);
								//std::cout<<"player 1 winner check "<<std::endl;
							}

							if (dealer.winner_list[1] > dealer.winner_list[0])
							{
								//sending winner uuid and pot value;
								to_player["winner_uuid"] = dealer.player_list[1].get_player_uuid();
								to_player["game_status"] = "winner_decided";
								to_player["pot_money"] = dealer.money_pot;
								to_player["winner_index"] = 1;
								to_player["looser_index"] = 0;
								to_player["player_count"] = player_count;
								to_player["looser_uuid"] = dealer.player_list[0].get_player_uuid();
								std::string t = to_player.dump();
								chat_message sending;
								memcpy(sending.body(), t.c_str(), t.size());
								sending.body_length(t.size());
								sending.encode_header();
								room_.deliver(sending);
								//std::cout<<"player 2 winner check "<<std::endl;
							}

							if (dealer.winner_list[1] == dealer.winner_list[0])
							{
								//sending the tie message to the player & next round player will get 
								//to_player["winner_uuid"] = dealer.player_list[1].get_player_uuid();
								to_player["game_status"] = "tie";
								//to_player["looser_uuid"] = dealer.player_list[0].get_player_uuid();
								std::string t = to_player.dump();
								chat_message sending;
								memcpy(sending.body(), t.c_str(), t.size());
								sending.body_length(t.size());
								sending.encode_header();
								room_.deliver(sending);
								std::cout << "tie check " << std::endl;
							}
						}

					}
					/////////////////////////////////////////////////////////////
					///////////start new round after the first round////////////
					/////////////          shuffle happens       ///////////////
					/////////////////player hand is resetted////////////////////
					if (from_client["action"] == "start_new_round")
					{
						std::cout << "New round has been started\n" << std::endl;
						///////////////////////////////////////////////////////
						//resetting the everything to start new game
						for (int i = 0; i < 52; i++)
						{
							dealer.card_Taken[i] = false;
						}
						if (player_count == 2)
						{
							while (!dealer.dealt_cards_player1.empty())
							{
								dealer.dealt_cards_player1.pop_back();
							}
							while (!dealer.dealt_cards_player2.empty())
							{
								dealer.dealt_cards_player2.pop_back();
							}
						}
						dealer.betting_round_counter = 0;
						dealer.money_pot = 0;
						//////////////////////////////////////////////////////////////////
						if (dealer.Check_num_player2() && player_count == 2)
						{
							dealer.Shuffle(dealer.Deck_cards);
							dealer.Shuffle(dealer.Deck_cards);
							dealer.Shuffle(dealer.Deck_cards);
							std::cout << "DECK has been Shuffled. Now Dealing Cards to the player\n" << std::endl;
							//deals card to two player and saves in vector for new game **currently only two player is handled
							dealer.dealt_cards_player1 = dealer.deal_card(dealer.Deck_cards, 1, player_count);
							dealer.dealt_cards_player2 = dealer.deal_card(dealer.Deck_cards, 2, player_count);

							//sending cards to the player 1
							to_player["start_game"] = "start";
							to_player["uuid"] = dealer.player_list[0].get_player_uuid();
							to_player["player_name"] = dealer.player_list[0].get_player_name();
							to_player["bet_button"] = "true";
							to_player["swap_button"] = "true";
							to_player["player_count"] = player_count;
							to_player["card"] = dealer.dealt_cards_player1;
							std::string a = to_player.dump();
							chat_message sending1;
							memcpy(sending1.body(), a.c_str(), a.size());
							sending1.body_length(a.size());
							sending1.encode_header();
							room_.deliver(sending1);

							//sending cards to the player 2
							to_player["start_game"] = "start";
							to_player["uuid"] = dealer.player_list[1].get_player_uuid();
							to_player["player_name"] = dealer.player_list[1].get_player_name();
							to_player["player_count"] = player_count;
							to_player["bet_button"] = "false";
							to_player["swap_button"] = "true";
							to_player["card"] = dealer.dealt_cards_player2;
							std::string b = to_player.dump();
							chat_message sending2;
							memcpy(sending2.body(), b.c_str(), b.size());
							sending2.body_length(b.size());
							sending2.encode_header();
							room_.deliver(sending2);

						}
					}


					/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					/////////when player hits check button signal is received from the server/////////////////////////////////////////
					//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					else if (from_client["action"] == "check")
					{
						//haven't implemented yet
					}
					//////////////////////////////////////////////////////////////////////////////////////////////////////////////////


					//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					/////////////message recived from the server that he wansts to fold///////////////////////////////////////////////
					//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					else if (from_client["action"] == "fold")
					{
						int find_player = 0;
						std::cout << from_client["player_name"] << " has folded." << std::endl;
						for (std::size_t i = 0; i < dealer.player_list.size(); i++)
						{
							if (from_client["uuid"] == dealer.player_list[i].get_player_uuid())
							{
								find_player = i;
								break;
							}
						}
						//sending player index and flag folded person to save in the winner_list
						dealer.store_fold(dealer.winner_list, find_player);

						//sending message to all player
						to_player["uuid"] = from_client["uuid"];
						to_player["player_name"] = from_client["player_name"];
						to_player["action"] = "folded";
						to_player["player_index"] = find_player;
						to_player["player_count"] = player_count;
						to_player["card"] = dealer.dealt_cards_player1;
						std::string t = to_player.dump();
						chat_message sending;
						memcpy(sending.body(), t.c_str(), t.size());
						sending.body_length(t.size());
						sending.encode_header();
						room_.deliver(sending);
					}

					//swap in done by giving player where the deck was stopped whiledealing the card. it deals in increading order
					//dealt card is traking by setting boolean value *false = not given* *true = give*
					else if (from_client["action"] == "swap")
					{
						std::cout << from_client["player_name"] << " has decided to swap cards.\n" << std::endl;
						//find the index upto which card is dealt from the deck;
						int current_index_of_dealted_card = 0;
						for (std::size_t i = 0; i < 52; i++)
						{
							if (dealer.card_Taken[i] != true)
							{
								current_index_of_dealted_card = i;
								break;
							}
						}

						//find the index of the player who wants to swap
						int find_player = 0;
						for (std::size_t i = 0; i < dealer.player_list.size(); i++)
						{
							if (from_client["uuid"] == dealer.player_list[i].get_player_uuid())
							{
								find_player = i;
								break;
							}
						}

						//first player deck of card is updated and new card is send back to the player
						if (find_player == 0)
						{
							//here dealer method is called to swap card
							dealer.Swap_Cards(dealer.dealt_cards_player1, current_index_of_dealted_card, from_client["swap_card1"],
								from_client["swap_card2"], from_client["swap_card3"], from_client["swap_card4"],
								from_client["swap_card5"]);
							//////////sending swapped card back to the player1///////////////
							to_player["uuid"] = from_client["uuid"];
							to_player["player_name"] = from_client["player_name"];
							to_player["action"] = "swapped";
							to_player["player_index"] = find_player;
							to_player["player_count"] = player_count;
							to_player["card"] = dealer.dealt_cards_player1;
							std::string t = to_player.dump();
							chat_message sending;
							memcpy(sending.body(), t.c_str(), t.size());
							sending.body_length(t.size());
							sending.encode_header();
							room_.deliver(sending);
						}
						//2nd player deck of card is updated and new card is sent back to the player
						else if (find_player == 1)
						{
							dealer.Swap_Cards(dealer.dealt_cards_player2, current_index_of_dealted_card, from_client["swap_card1"],
								from_client["swap_card2"], from_client["swap_card3"], from_client["swap_card4"],
								from_client["swap_card5"]);
							//////////sending swapped card back to the player 2///////////////
							std::cout.write(read_msg_.body(), read_msg_.body_length());
							std::cout << "\n";
							to_player["uuid"] = from_client["uuid"];
							to_player["player_name"] = from_client["player_name"];
							to_player["action"] = "swapped";
							to_player["player_index"] = find_player;
							to_player["player_count"] = player_count;
							to_player["card"] = dealer.dealt_cards_player2;
							std::string t = to_player.dump();
							chat_message sending;
							memcpy(sending.body(), t.c_str(), t.size());
							sending.body_length(t.size());
							sending.encode_header();
							room_.deliver(sending);
						}

						//3rd player deck is updates and new card is sent back to the player
						else if (find_player == 2)
						{
							dealer.Swap_Cards(dealer.dealt_cards_player3, current_index_of_dealted_card, from_client["swap_card1"],
								from_client["swap_card2"], from_client["swap_card3"], from_client["swap_card4"],
								from_client["swap_card5"]);
							//////////sending swapped card back to the player3///////////////
							std::cout.write(read_msg_.body(), read_msg_.body_length());
							std::cout << "\n";
							to_player["uuid"] = from_client["uuid"];
							to_player["player_name"] = from_client["player_name"];
							to_player["action"] = "swapped";
							to_player["player_index"] = find_player;
							to_player["player_count"] = player_count;
							to_player["card"] = dealer.dealt_cards_player3;
							std::string t = to_player.dump();
							chat_message sending;
							memcpy(sending.body(), t.c_str(), t.size());
							sending.body_length(t.size());
							sending.encode_header();
							room_.deliver(sending);
						}
					}
					///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					do_read_header();
				}
				else
				{
					room_.leave(shared_from_this());
				}
			});
	}

	void do_write()
	{
		auto self(shared_from_this());
		asio::async_write(socket_,
			asio::buffer(write_msgs_.front().data(),
				write_msgs_.front().length()),
			[this, self](std::error_code ec, std::size_t /*length*/)
			{
				if (!ec)
				{
					write_msgs_.pop_front();
					if (!write_msgs_.empty())
					{
						do_write();
					}
				}
				else
				{
					room_.leave(shared_from_this());
				}
			});
	}

	tcp::socket socket_;
	chat_room& room_;
	chat_message read_msg_;
	chat_message_queue write_msgs_;
};

//----------------------------------------------------------------------

class chat_server
{
public:
	chat_server(asio::io_context& io_context,
		const tcp::endpoint& endpoint)
		: acceptor_(io_context, endpoint)
	{
		do_accept();
	}

private:
	void do_accept()
	{
		acceptor_.async_accept(
			[this](std::error_code ec, tcp::socket socket)
			{
				if (!ec)
				{
					std::make_shared<chat_session>(std::move(socket), room_)->start();
				}

				do_accept();
			});
	}

	tcp::acceptor acceptor_;
	chat_room room_;
};

//----------------------------------------------------------------------

int main(int argc, char* argv[])
{
	try
	{
		if (argc < 2)
		{
			std::cerr << "Usage: chat_server <port> [<port> ...]\n";
			return 1;
		}

		asio::io_context io_context;

		std::list<chat_server> servers;
		for (int i = 1; i < argc; ++i)
		{
			tcp::endpoint endpoint(tcp::v4(), std::atoi(argv[i]));
			servers.emplace_back(io_context, endpoint);
		}

		io_context.run();
	}
	catch (std::exception & e)
	{
		std::cerr << "Exception: " << e.what() << "\n";
	}




	return 0;
}
