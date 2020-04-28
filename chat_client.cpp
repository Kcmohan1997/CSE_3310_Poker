#include "client_window.h"


using asio::ip::tcp;

typedef std::deque<chat_message> chat_message_queue;

////////////////////////////////////////////////
//////////////////gui window object////////////
//////////////////////////////////////////////
class Mainwin;
Mainwin* update_window;
/////////////////////////////////////////////


///added to update game status to other player
void update_game_status(std::string, int, int);

class chat_client
{
public:
	chat_client(asio::io_context& io_context,
		const tcp::resolver::results_type& endpoints)
		: io_context_(io_context),
		socket_(io_context)
	{
		do_connect(endpoints);
	}

	void write(const chat_message& msg)
	{
		asio::post(io_context_,
			[this, msg]()
			{
				bool write_in_progress = !write_msgs_.empty();
				write_msgs_.push_back(msg);
				if (!write_in_progress)
				{
					do_write();
				}
			});
	}

	void close()
	{
		asio::post(io_context_, [this]() { socket_.close(); });
	}

private:
	void do_connect(const tcp::resolver::results_type& endpoints)
	{
		asio::async_connect(socket_, endpoints,
			[this](std::error_code ec, tcp::endpoint)
			{
				if (!ec)
				{
					do_read_header();
				}
			});
	}

	void do_read_header()
	{
		asio::async_read(socket_,
			asio::buffer(read_msg_.data(), chat_message::header_length),
			[this](std::error_code ec, std::size_t /*length*/)
			{
				if (!ec && read_msg_.decode_header())
				{
					// clear out the old buffer from the last read
							// a '\0' is a good value to make sure a string
						// is terminated
					for (unsigned int i = 0; i < chat_message::max_body_length; i++)
					{
						read_msg_.body()[i] = '\0';
					}
					do_read_body();
				}
				else
				{
					socket_.close();
				}
			});
	}


	void do_read_body()
	{
		asio::async_read(socket_,
			asio::buffer(read_msg_.body(), read_msg_.body_length()),
			[this](std::error_code ec, std::size_t /*length*/)
			{
				if (!ec)
				{
					//////////////////////////////////////////////////////////////////////////////////////////
					/////this is the part where we get message fromt the server and we need to update gui/////
					//////////////////////////////////////////////////////////////////////////////////////////
					std::string dealer_msg(read_msg_.body());
					nlohmann::json from_dealer = nlohmann::json::parse(dealer_msg);

					///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					//if player joins the game GAME STATUS gets updated...////////////////////////////////////////////////////////////
					if (from_dealer["status"] == "joined")
					{
						int player_number = from_dealer["player_count"];
						if (player_number == 1)
						{
							update_window->m.lock();
							update_window->p1_status->set_text("Joined");
							update_window->m.unlock();
						}
						else if (player_number == 2)
						{
							update_window->m.lock();
							update_window->p2_status->set_text("Joined");
							update_window->m.unlock();
						}
						else if (player_number == 3)
						{
							update_window->m.lock();
							update_window->p3_status->set_text("Joined");
							update_window->m.unlock();
						}
						else if (player_number == 4)
						{
							update_window->m.lock();
							update_window->p4_status->set_text("Joined");
							update_window->m.unlock();
						}
						else if (player_number == 5)
						{
							update_window->m.lock();
							update_window->p5_status->set_text("Joined");
							update_window->m.unlock();
						}
					}

					//here the player gets the card from the dealer
					if (from_dealer["start_game"] == "start")
					{
						if (from_dealer["uuid"] == update_window->uuid)
						{
							int no_player = from_dealer["player_count"];
							for (int i = 0; i < no_player; i++)
							{
								for (int i = 0; i < 5; i++)
								{
									if (i == 0)
									{
										update_window->m.lock();
										std::string a = from_dealer["card"][0];
										update_window->card1->set("./src/Card_Images/" + a + ".png");
										update_window->m.unlock();
									}
									else if (i == 1)
									{
										update_window->m.lock();
										std::string a = from_dealer["card"][1];
										update_window->card2->set("./src/Card_Images/" + a + ".png");
										update_window->m.unlock();
									}
									else if (i == 2)
									{
										update_window->m.lock();
										std::string a = from_dealer["card"][2];
										update_window->card3->set("./src/Card_Images/" + a + ".png");
										update_window->m.unlock();
									}
									else if (i == 3)
									{
										update_window->m.lock();
										std::string a = from_dealer["card"][3];
										update_window->card4->set("./src/Card_Images/" + a + ".png");
										update_window->m.unlock();
									}
									else if (i == 4)
									{
										update_window->m.lock();
										std::string a = from_dealer["card"][4];
										update_window->card5->set("./src/Card_Images/" + a + ".png");
										update_window->m.unlock();
									}

								}
							}
							//setting to match the betting turn
							if (from_dealer["bet_button"] == "true")
							{
								//does nothing because he is the first player in the game
							}
							else if (from_dealer["bet_button"] == "false")
							{
								update_window->m.lock();
								update_window->betButton->set_sensitive(false);
								update_window->m.unlock();
							}
						}
						//when new round starts swapping is allowed to both players
						if (from_dealer["swap_button"] == true)
						{
							update_window->swapButton->set_sensitive(true);
							update_window->start_next_round_button->set_sensitive(false);
						}

					}
					//////////////////////////////////////////////////////////////////////////////////////////
					/////signal received from the server that player betted/////////////////////////////////
					//////////////////////////////////////////////////////////////////////////////////////
					if (from_dealer["action"] == "betted")
					{
						int player_index = from_dealer["player_index"];
						player_index += 1;
						int player_count = from_dealer["player_count"];
						std::string a = from_dealer["action"];
						int b = from_dealer["bet_amount"];
						std::stringstream c;
						c << b;
						std::string message = a + " $" + c.str();
						update_game_status(message, player_count, player_index);
						int no_player = from_dealer["player_count"];

						//betting button shifts as the number of player
						if (no_player == 2)
						{
							if (from_dealer["non_better_uuid"] == update_window->uuid)
							{
								update_window->m.lock();
								update_window->betButton->set_sensitive(true);
								update_window->m.unlock();
							}

						}
					}


					///////////////////////////////////////////////////////////////////////////////////////////////////////////
					///////////////////// message received from the server with swapped card///////////////////////////////////
					//////////////////////////////////////////////////////////////////////////////////////////////////////////
					if (from_dealer["action"] == "swapped")
					{
						if (from_dealer["uuid"] == update_window->uuid)
						{
							for (int i = 0; i < 5; i++)
							{
								if (i == 0)
								{
									// setting up the poker card image
									update_window->m.lock();
									std::string a = from_dealer["card"][0];
									update_window->card1->set("./src/Card_Images/" + a + ".png");
									update_window->m.unlock();
								}
								else if (i == 1)
								{
									update_window->m.lock();
									std::string a = from_dealer["card"][1];
									update_window->card2->set("./src/Card_Images/" + a + ".png");
									update_window->m.unlock();
								}
								else if (i == 2)
								{
									update_window->m.lock();
									std::string a = from_dealer["card"][2];
									update_window->card3->set("./src/Card_Images/" + a + ".png");
									update_window->m.unlock();
								}
								else if (i == 3)
								{
									update_window->m.lock();
									std::string a = from_dealer["card"][3];
									update_window->card4->set("./src/Card_Images/" + a + ".png");
									update_window->m.unlock();
								}
								else if (i == 4)
								{
									update_window->m.lock();
									std::string a = from_dealer["card"][4];
									update_window->card5->set("./src/Card_Images/" + a + ".png");
									update_window->m.unlock();
								}

							}
						}

						int player_index = from_dealer["player_index"];
						//	std::cout<<"testing crash"<<std::endl;
						player_index += 1;
						int player_count = from_dealer["player_count"];
						std::string message = from_dealer["action"];
						update_game_status(message, player_count, player_index);
					}

					///////////////////////////////////////////////////////////////////////
					//////////////updates gui accordingly if the action is folded/////////
					///////////////////////////////////////////////////////////////////////
					if (from_dealer["action"] == "folded")
					{
						int player_index = from_dealer["player_index"];
						player_index += 1;
						int player_count = from_dealer["player_count"];
						std::string message = from_dealer["action"];
						update_game_status(message, player_count, player_index);
					}

					//////////////////////////////////////////////////////////////////////////////////////////////////////////




					/////////////////////////////////////////////////////////////////////////////////////////////////////////
					//////////////////////////winner is received from the server////////////////////////////////////////////
					////////////////////////////////////////////////////////////////////////////////////////////////////////
					if (from_dealer["game_status"] == "winner_decided")
					{
						update_window->start_next_round_button->set_sensitive(true);
						int winner_index = 0;
						int player_count = 0;
						int looser_index = 0;
						if (from_dealer["winner_uuid"] == update_window->uuid)
						{
							//std::cout<<"this is a check1"<<std::endl;
							int win_amount = from_dealer["pot_money"];
							update_window->set_total_amount_after_win(win_amount);
							winner_index = from_dealer["winner_index"];
							player_count = from_dealer["player_count"];
							looser_index = from_dealer["looser_index"];
						}
						update_game_status("**WINNER**", player_count, winner_index + 1);
						update_game_status("**LOOSER**", player_count, looser_index + 1);
					}

					if (from_dealer["game_status"] == "tie")
					{
						update_window->start_next_round_button->set_sensitive(true);
						//std::cout<<"this is a check3"<<std::endl;
						int player_count = from_dealer["player_count"];
						std::string message = "**TIE**";
						if (player_count == 2)
						{
							update_game_status(message, player_count, 1);
							update_game_status(message, player_count, 2);
						}

					}

					///////////////////////////////////////////////////////////////////////////////////////////////////////
					do_read_header();
				}
				else
				{
					socket_.close();
				}
			});
	}

	void do_write()
	{
		asio::async_write(socket_,
			asio::buffer(write_msgs_.front().data(),
				write_msgs_.front().length()),
			[this](std::error_code ec, std::size_t /*length*/)
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
					socket_.close();
				}
			});
	}

private:
	asio::io_context& io_context_;
	tcp::socket socket_;
	chat_message read_msg_;
	chat_message_queue write_msgs_;
};
//global class
chat_client* c;

///////// method to bet///////////////
void Mainwin::on_bet_click()
{
	Gtk::Dialog dialog{ "Betting window", *this };
	Gtk::Grid grid;
	Gtk::Label l_name{ "How much would you bet $1 to $5" };
	Gtk::Entry e_name;
	grid.attach(l_name, 0, 0, 1, 1);
	grid.attach(e_name, 1, 0, 2, 1);
	dialog.get_content_area()->add(grid);
	dialog.add_button("Place Bet", 1);
	dialog.add_button("Cancel Bet", 0);
	dialog.show_all();
	dialog.run();
	if (!e_name.get_text().empty())
	{
		if (std::stoi(e_name.get_text()) == 0)
		{
			show_message_dialog4();
			on_bet_click();
		}

		else if (std::stoi(e_name.get_text()) > 5)
		{
			show_message_dialog8();
			on_bet_click();
		}
		else
		{
			set_total_amount_after_bet(e_name.get_text());
			betAmount->set_text(" $ " + e_name.get_text());
			bet_amount = std::stoi(e_name.get_text());
			///////////////////////////////////////////////////////////////////////////////////////////
			/////message is sent to the server that player wants to bet and the bet amount ////////////
			///////////////////////////////////////////////////////////////////////////////////////////
			betButton->set_sensitive(false);
			std::string text;
			chat_message msg;
			nlohmann::json to_dealer;
			to_dealer["uuid"] = uuid;
			to_dealer["player_name"] = player_name;
			to_dealer["action"] = "bet";
			to_dealer["bet_amount"] = bet_amount;
			text = to_dealer.dump();
			msg.body_length(text.size());
			std::memcpy(msg.body(), text.c_str(), msg.body_length());
			msg.encode_header();
			assert(c);  // this is a global class
			c->write(msg);
		}
	}
}

//fucntion when player clicks check button
void Mainwin::on_check_click() {
	betAmount->set_text("0");

	///////////////////////////////////////////////////////////////////////////////////////////
	/////message is sent to the server that player wants to check ////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////
	std::string text;
	//char line[chat_message::max_body_length + 1];
	chat_message msg;
	nlohmann::json to_dealer;
	to_dealer["uuid"] = uuid;
	to_dealer["player_name"] = player_name;
	to_dealer["action"] = "check";
	text = to_dealer.dump();
	msg.body_length(text.size());
	std::memcpy(msg.body(), text.c_str(), msg.body_length());
	msg.encode_header();
	assert(c);  // this is a global class
	c->write(msg);
}

//fucntion when player clicks fold button
void Mainwin::on_fold_click() {
	betButton->set_sensitive(false);
	swapButton->set_sensitive(false);
	checkButton->set_sensitive(false);
	foldButton->set_sensitive(false);

	///////////////////////////////////////////////////////////////////////////////////////////
	/////message is sent to the server that player wants to fold //////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////
	std::string text;
	chat_message msg;
	nlohmann::json to_dealer;
	to_dealer["uuid"] = uuid;
	to_dealer["player_name"] = player_name;
	to_dealer["action"] = "fold";
	text = to_dealer.dump();
	msg.body_length(text.size());
	std::memcpy(msg.body(), text.c_str(), msg.body_length());
	msg.encode_header();
	assert(c);  // this is a global class
	c->write(msg);
}

//function when player enters swap button
void Mainwin::on_swap_click() {

	/////check button added///////
	Gtk::Dialog dialog{ "Swap Card Decision upto three card", *this };
	dialog.set_default_size(300, 100);
	Gtk::Grid grid;
	Gtk::CheckButton swap1{ "card 1" };
	Gtk::CheckButton swap2{ "card 2" };
	Gtk::CheckButton swap3{ "card 3" };
	Gtk::CheckButton swap4{ "card 4" };
	Gtk::CheckButton swap5{ "card 5" };
	grid.attach(swap1, 0, 0, 1, 1);
	grid.attach(swap2, 1, 0, 1, 1);
	grid.attach(swap3, 2, 0, 1, 1);
	grid.attach(swap4, 3, 0, 1, 1);
	grid.attach(swap5, 4, 0, 1, 1);
	dialog.get_content_area()->add(grid);

	dialog.add_button("Swap", 1);
	dialog.add_button("Cancel Swap", 0);

	dialog.show_all();
	dialog.run();

	//assign to track the index of swapping card - value if no need to swap
	int card1 = -1, card2 = -1, card3 = -1, card4 = -1, card5 = -1;
	if (swap1.get_active())
	{
		card1 = 0;
		count_swap++;
	}
	if (swap2.get_active())
	{
		card2 = 1;
		count_swap++;
	}
	if (swap3.get_active())
	{
		card3 = 2;
		count_swap++;
	}
	if (swap4.get_active())
	{
		card4 = 3;
		count_swap++;
	}
	if (swap5.get_active())
	{
		card5 = 4;
		count_swap++;
	}

	///////////////////////////////////////////////////////////////////////////////////////////
	/////message is sent to the server that player wants to swap the card /////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////
	if ((swap1.get_active() || swap2.get_active() || swap3.get_active() || swap4.get_active() || swap5.get_active()) && count_swap < 4)
	{
		std::string text;
		chat_message msg;
		nlohmann::json to_dealer;
		to_dealer["uuid"] = uuid;
		to_dealer["player_name"] = player_name;
		to_dealer["action"] = "swap";
		to_dealer["swap_card1"] = card1;
		to_dealer["swap_card2"] = card2;
		to_dealer["swap_card3"] = card3;
		to_dealer["swap_card4"] = card4;
		to_dealer["swap_card5"] = card5;
		text = to_dealer.dump();
		msg.body_length(text.size());
		std::memcpy(msg.body(), text.c_str(), msg.body_length());
		msg.encode_header();
		assert(c);  // this is a global class
		c->write(msg);
		swapButton->set_sensitive(false);
		show_message_dialog9();
	}
	else if (count_swap >= 4)
	{
		show_message_dialog5();
		count_swap = 0;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////
///// message is sent to the server that player wants to start the new round     //////////
///////////////////////////////////////////////////////////////////////////////////////////
void Mainwin::on_next_round_click()
{
	std::string text;
	chat_message msg;
	nlohmann::json to_dealer;
	to_dealer["action"] = "start_new_round";
	text = to_dealer.dump();
	msg.body_length(text.size());
	std::memcpy(msg.body(), text.c_str(), msg.body_length());
	msg.encode_header();
	assert(c);  // this is a global class
	c->write(msg);
	update_window->start_next_round_button->set_sensitive(false);
}

//fucntion to check if the player clicks play button
void Mainwin::on_play_click()
{
	Gtk::Dialog dialog1{ "Player Info",*this };
	Gtk::Grid grid;
	Gtk::Label message1{ "Enter Name" };
	Gtk::Entry name_Entry;
	grid.attach(message1, 0, 0, 1, 1);
	grid.attach(name_Entry, 1, 0, 2, 1);
	dialog1.get_content_area()->add(grid);
	dialog1.add_button("Enter", 0);

	dialog1.show_all();
	dialog1.run();

	//calls function to check if player enter all the information
	check_player_mode_entry(name_Entry.get_text());
}

//function to check if player enter the ante and name
void Mainwin::check_player_mode_entry(std::string name_Entry)
{
	if (!(name_Entry.empty()))
	{
		show_message_dialog1();
		nameEntry->set_text(name_Entry + "\t");
		player_name = name_Entry;
		uuid = generate_uuid();
		play_button_mode = false;
		totalAmount->set_text("$  100");
		playButton->set_sensitive(play_button_mode);
		betButton->set_sensitive(true);
		swapButton->set_sensitive(true);
		checkButton->set_sensitive(true);
		foldButton->set_sensitive(true);
		///////////////////////////////////////////////////////////////////////////////////////////
		/////message is sent to the server that player joined the game ////////////////////////////
		///////////////////////////////////////////////////////////////////////////////////////////
		std::string text;
		chat_message msg;
		nlohmann::json to_dealer;
		to_dealer["uuid"] = uuid;
		to_dealer["player_name"] = player_name;
		to_dealer["mode"] = "player";
		to_dealer["ante"] = 2; //setting the deafault ante for all player
		text = to_dealer.dump();
		msg.body_length(text.size());
		std::memcpy(msg.body(), text.c_str(), msg.body_length());
		msg.encode_header();
		assert(c);  // this is a global class
		c->write(msg);
	}
	else
	{
		show_message_dialog2();
		on_play_click();
	}
}


//this funciton continuously gets called to give other player updates about the other player action
void update_game_status(std::string message, int player_size, int player_index)
{
	for (int i = 0; i < player_size; i++)
	{
		//player 1 action gets updated
		if (player_index == 1)
		{
			update_window->m.lock();
			update_window->p1_status->set_text(message);
			update_window->m.unlock();
		}
		//player 2 action gets updated
		else if (player_index == 2)
		{
			update_window->m.lock();
			update_window->p2_status->set_text(message);
			update_window->m.unlock();
		}

		else if (player_index == 3)
		{
			update_window->m.lock();
			update_window->p3_status->set_text(message);
			update_window->m.unlock();
		}
		else if (player_index == 4)
		{
			update_window->m.lock();
			update_window->p4_status->set_text(message);
			update_window->m.unlock();
		}
		else if (player_index == 5)
		{
			update_window->m.lock();
			update_window->p5_status->set_text(message);
			update_window->m.unlock();
		}
	}
}


int main(int argc, char* argv[]) {
	if (argc != 3)
	{
		std::cerr << "Usage: chat_client <host> <port>\n";
		return 1;
	}

	asio::io_context io_context;
	tcp::resolver resolver(io_context);
	auto endpoints = resolver.resolve(argv[1], argv[2]);
	c = new chat_client(io_context, endpoints);
	assert(c);
	std::thread t([&io_context]() { io_context.run(); });
	Gtk::Main app(argc, argv);
	Mainwin client_window;

	// setting the address of gui class Mainwin
	update_window = &client_window;
	Gtk::Main::run(client_window);
	gtk_main();

	c->close();
	t.join();

	return 0;
}
