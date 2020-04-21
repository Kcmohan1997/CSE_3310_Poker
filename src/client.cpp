#include "client.h"
#include <sstream>
#include <string>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <deque>
#include <thread>
#include <assert.h>
#include "asio.hpp"
#include "chat_message.hpp"
#include "json.hpp"

GtkWidget* fromView = NULL;  // text from the chat server

using asio::ip::tcp;

typedef std::deque<chat_message> chat_message_queue;

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
					/* char outline[read_msg_.body_length() + 2];
												// '\n' + '\0' is 2 more chars
					 outline[0] = '\n';
					 outline[read_msg_.body_length() + 1] = '\0';
					 std::memcpy ( &outline[1], read_msg_.body(), read_msg_.body_length() );
					 gtk_label_set_text (GTK_LABEL(fromView), outline);*/
					std::cout.write(read_msg_.body(), read_msg_.body_length());
					std::cout << "\n";
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

// global symbols
chat_client* c;
//////////////////////////////////////////////////////////////////////////////

///////////////// Gui Setup///////////////////////////////////////////////////
Mainwin::Mainwin()
	:betButton{ Gtk::manage(new Gtk::Button{"Bet"}) },
	checkButton{ Gtk::manage(new Gtk::Button{"Check"}) },
	foldButton{ Gtk::manage(new Gtk::Button{"Fold"}) },
	swapButton{ Gtk::manage(new Gtk::Button{"Swap"}) },
	spectateButton{ Gtk::manage(new Gtk::ToggleButton{" SPECTATE "}) },
	nameEntry{ Gtk::manage(new Gtk::Label{"\t\t"}) },
	card1{ Gtk::manage(new Gtk::Label{"Card 1"}) },
	card2{ Gtk::manage(new Gtk::Label{"Card 2"}) },
	card3{ Gtk::manage(new Gtk::Label{"Card 3"}) },
	card4{ Gtk::manage(new Gtk::Label{"Card 4"}) },
	card5{ Gtk::manage(new Gtk::Label{"Card 5"}) },
	betAmount{ Gtk::manage(new Gtk::Label{"$ 0"}) },
	totalAmount{ Gtk::manage(new Gtk::Label{"$ 0"}) }
{
	set_default_size(800, 600);
	set_title(APP_TITLE);
	//override_background_color(Gdk::RGBA("green"));
	// Put a vertical box container as the Window contents
	Gtk::Box* vbox = Gtk::manage(new Gtk::VBox);
	add(*vbox);
	//Header Hbox
	Gtk::Box* hbox0 = Gtk::manage(new Gtk::HBox);
	vbox->pack_start(*hbox0, Gtk::PACK_SHRINK, 0);
	hbox0->pack_start(*spectateButton, Gtk::PACK_SHRINK, 0);
	spectateButton->signal_clicked().connect([this] {this->on_spectate_click(); });
	///*if we want an ante button
	playButton = Gtk::manage(new Gtk::Button{ " PLAY " });
	hbox0->pack_start(*playButton, Gtk::PACK_SHRINK, 0);
	playButton->signal_clicked().connect([this] {this->on_play_click(); });
	
	Gtk::Label* time = Gtk::manage(new Gtk::Label{ " " });
	hbox0->pack_start(*time, Gtk::PACK_SHRINK, 0);
	time->set_hexpand(true);
	Gtk::Label* playerName = Gtk::manage(new Gtk::Label{ "Player Name: " });

	hbox0->pack_start(*playerName, Gtk::PACK_SHRINK, 0);
	hbox0->pack_start(*nameEntry, Gtk::PACK_SHRINK, 0);

	// Card Display
	// Hbox of 5 labels, labels will be changable after integratio with dealer.
	Gtk::Box* hbox = Gtk::manage(new Gtk::HBox);
	vbox->pack_start(*hbox);
	hbox->pack_start(*card1);
	hbox->pack_start(*card2);
	hbox->pack_start(*card3);
	hbox->pack_start(*card4);
	hbox->pack_start(*card5);
	//Interactive zone of buttons and labels held in another hbox
	Gtk::Box* hbox2 = Gtk::manage(new Gtk::HBox);
	vbox->pack_start(*hbox2);
	hbox2->pack_start(*betButton);
	betButton->signal_clicked().connect([this] {this->on_bet_click(); });
	hbox2->pack_start(*checkButton);
	checkButton->signal_clicked().connect([this] {this->on_check_click(); });
	hbox2->pack_start(*foldButton);
	foldButton->signal_clicked().connect([this] {this->on_fold_click(); });
	hbox2->pack_start(*swapButton);
	swapButton->signal_clicked().connect([this] {this->on_swap_click(); });
	Gtk::Box* vbox2 = Gtk::manage(new Gtk::VBox);
	Gtk::Label* chatBox = Gtk::manage(new Gtk::Label{ "Chat Area" });
	Gtk::Label* chatField = Gtk::manage(new Gtk::Label{ "Chat field" });
	hbox2->pack_start(*vbox2);
	vbox2->pack_start(*chatBox, Gtk::PACK_SHRINK, 0);
	chatBox->override_background_color(Gdk::RGBA{ "gray" });
	chatField->override_background_color(Gdk::RGBA{ "white" });
	vbox2->pack_start(*chatField);
	chatField->set_hexpand(true);
	chatField->set_vexpand(true);
	Gtk::Grid* grid = Gtk::manage(new Gtk::Grid);
	Gtk::Label* space = Gtk::manage(new Gtk::Label{});
	space->set_vexpand(true);
	grid->attach(*space, 0, 1, 1, 1);
	Gtk::Label* total = Gtk::manage(new Gtk::Label{ "Total" });
	grid->attach(*total, 0, 1, 1, 1);
	grid->attach(*totalAmount, 2, 1, 2, 1);
	Gtk::Label* bet = Gtk::manage(new Gtk::Label{ "\tBet Amount" });
	grid->attach(*bet, 0, 2, 1, 1);
	grid->attach(*betAmount, 2, 2, 2, 1);
	hbox2->pack_start(*grid);
	Gtk::Button* leaveButton = Gtk::manage(new Gtk::Button{ "Leave" });
	hbox2->pack_start(*leaveButton);
	leaveButton->signal_clicked().connect([this] {this->on_leave_click(); });

	vbox->show_all();
	
	betButton->set_sensitive(button_state);
	swapButton->set_sensitive(button_state);
	checkButton->set_sensitive(button_state);
	foldButton->set_sensitive(button_state);
}

Mainwin::~Mainwin() { }


///////// method to bet///////////////
void Mainwin::on_bet_click() {
	Gtk::Dialog dialog{ "Betting window", *this };

	Gtk::Grid grid;

	Gtk::Label l_name{ "How much will you bet $" };
	Gtk::Entry e_name;
	grid.attach(l_name, 0, 0, 1, 1);
	grid.attach(e_name, 1, 0, 2, 1);

	dialog.get_content_area()->add(grid);

	dialog.add_button("Place Bet", 1);
	dialog.add_button("Cancel Bet", 0);

	dialog.show_all();
	dialog.run();
	if(!e_name.get_text().empty())
	{		
		if (std::stoi(e_name.get_text())==0)
		{
			show_message_dialog4();
			on_bet_click();
		}
		else
		{
			betAmount->set_text("$" + e_name.get_text());
			//message to the server
			std::string text;
			//char line[chat_message::max_body_length + 1];
			chat_message msg;
			nlohmann::json j;
			j["action"] = "bet";
			j["amount"] = std::stoi(e_name.get_text());
			text = j.dump();
			char x[text.size() + 1];
			std::strcpy(x, text.c_str());
			msg.body_length(std::strlen(x));
			std::memcpy(msg.body(), x, msg.body_length());
			msg.encode_header();
			assert(c);  // this is a global class
			c->write(msg);	
		}
	}
}

void Mainwin::on_check_click() {
	betAmount->set_text("$0");

	//message to the server
	std::string text;
	//char line[chat_message::max_body_length + 1];
	chat_message msg;
	nlohmann::json j;
        j["action"] = "check";
	text = j.dump();
	char x[text.size() + 1];
	std::strcpy(x, text.c_str());
	msg.body_length(std::strlen(x));
	std::memcpy(msg.body(), x, msg.body_length());
	msg.encode_header();
	assert(c);  // this is a global class
	c->write(msg);
}


void Mainwin::on_fold_click() {
	betButton->set_sensitive(false);
	swapButton->set_sensitive(false);
	checkButton->set_sensitive(false);
	foldButton->set_sensitive(false);

	//message to the server
	std::string text;
	//char line[chat_message::max_body_length + 1];
	chat_message msg;
	nlohmann::json j;
        j["action"] = "fold";
	text = j.dump();
	char x[text.size() + 1];
	std::strcpy(x, text.c_str());
	msg.body_length(std::strlen(x));
	std::memcpy(msg.body(), x, msg.body_length());
	msg.encode_header();
	assert(c);  // this is a global class
	c->write(msg);
}

void Mainwin::on_swap_click() {
	Gtk::Dialog dialog{ "Swap Decision", *this };

	/*////check button added///////
	    Gtk::Dialog dialog{"Swap Decision", *this};
	    dialog.set_default_size(300, 100);

	    Gtk::Grid grid;
	    Gtk::CheckButton swap1{"1"};
	    Gtk::CheckButton swap2{"2"};
	    Gtk::CheckButton swap3{"3"};
	    Gtk::CheckButton swap4{"4"};
	    Gtk::CheckButton swap5{"5"};
	    grid.attach(swap1, 0, 0, 1, 1);
	    grid.attach(swap2, 1, 0, 1, 1);
	    grid.attach(swap3, 2, 0, 1, 1);
	    grid.attach(swap4, 3, 0, 1, 1);
	    grid.attach(swap5, 4, 0, 1, 1);
	
	    if (swap1.get_active())
	    {
		card1->set_text(swap1.get_label());
	    }

	    dialog.get_content_area()->add(grid);

	    dialog.add_button("Swap", 1);
	    dialog.add_button("Cancel Swap", 0);

	    dialog.show_all();
	    dialog.run();
	    if (swap1.get_active())
	    {
		card1->set_text(swap1.get_label());
	    }
	    if (swap2.get_active())
	    {
		card2->set_text(swap2.get_label());
	    }
	    if (swap3.get_active())
	    {
		card3->set_text(swap3.get_label());
	    }
	    if (swap4.get_active())
	    {
		card4->set_text(swap4.get_label());
	    }
	    if (swap5.get_active())
	    {
		card5->set_text(swap5.get_label());
	    }
	//////////////////////////////////////////////////*/

	Gtk::Grid grid;

	Gtk::Label l_name{ "Which Cards do you wish to swap" };
	Gtk::Label directions{ "Please type numbers corresponding to cards with spaces in between, or type 'all'" };
	Gtk::Entry e_name;
	grid.attach(l_name, 0, 0, 1, 1);
	grid.attach(directions, 0, 1, 2, 1);
	grid.attach(e_name, 0, 2, 2, 1);

	dialog.get_content_area()->add(grid);

	dialog.add_button("Swap", 1);
	dialog.add_button("Cancel Swap", 0);

	dialog.show_all();
	dialog.run();

	//message to the server
	std::string text;
	//char line[chat_message::max_body_length + 1];
	chat_message msg;
	nlohmann::json j;
        j["action"] = "swap";
	text = j.dump();
	char x[text.size() + 1];
	std::strcpy(x, text.c_str());
	msg.body_length(std::strlen(x));
	std::memcpy(msg.body(), x, msg.body_length());
	msg.encode_header();
	assert(c);  // this is a global class
	c->write(msg);
}


void Mainwin::on_spectate_click() {
	//spectate->set_active(false);	
	if (spectateButton->get_active() == true)
	{
		show_message_dialog3();
		play_button_mode = true;
		playButton->set_sensitive(play_button_mode);

		betButton->set_sensitive(button_state);
		swapButton->set_sensitive(button_state);
		checkButton->set_sensitive(button_state);
		foldButton->set_sensitive(button_state);

		//message to the server
		std::string text;
		//char line[chat_message::max_body_length + 1];
		chat_message msg;
		nlohmann::json j;
		j["mode"] = "spectator";
		text = j.dump();
		char x[text.size() + 1];
		std::strcpy(x, text.c_str());
		msg.body_length(std::strlen(x));
		std::memcpy(msg.body(), x, msg.body_length());
		msg.encode_header();
		assert(c);  // this is a global class
		c->write(msg);
	}
}

void Mainwin::on_play_click() {

	Gtk::Dialog dialog1{ "Player Info",*this };
	Gtk::Grid grid;
	Gtk::Label message1{ "Enter Name" };
	Gtk::Entry name_Entry;
	Gtk::Label message2{ "Enter Ante $" };
	Gtk::Entry ante_Entry;
	grid.attach(message1, 0, 0, 1, 1);
	grid.attach(name_Entry, 1, 0, 2, 1);
	grid.attach(message2, 0, 1, 1, 1);
	grid.attach(ante_Entry, 1, 1, 2, 1);
	dialog1.get_content_area()->add(grid);
	dialog1.add_button("Enter", 0);

	dialog1.show_all();
	dialog1.run();
	check_player_mode_entry(name_Entry.get_text(),ante_Entry.get_text());
}
	
void Mainwin::check_player_mode_entry(std::string name_Entry,std::string ante_Entry)
{
 	if(!(name_Entry.empty())&&!(ante_Entry.empty()))
    	{		
		show_message_dialog1();
		nameEntry->set_text(name_Entry+ "\t");
		//xxxx->set_text(ante_Entry.get_text()+" "); //will change later
		play_button_mode = false;
		//spectateButton->set_sensitive(play_button_mode);
		totalAmount->set_text("$ 500");
		playButton->set_sensitive(play_button_mode);
		betButton->set_sensitive(true);
		swapButton->set_sensitive(true);
		checkButton->set_sensitive(true);
		foldButton->set_sensitive(true);

		//message to the server
		std::string text;
		//char line[chat_message::max_body_length + 1];
		chat_message msg;
		nlohmann::json j;
		j["player_name"] = name_Entry;
		j["mode"] = "play";
		j["ante"] = std::stoi(ante_Entry);
		text = j.dump();
		char x[text.size() + 1];
		std::strcpy(x, text.c_str());
		msg.body_length(std::strlen(x));
		std::memcpy(msg.body(), x, msg.body_length());
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

void Mainwin::on_leave_click() {
	close();
}

void Mainwin::show_message_dialog1()
{
	Gtk::MessageDialog dialog1(*this,"Welcome to POKER++\nYou're starting with $500 credits.",false,Gtk::MESSAGE_INFO);
	dialog1.run();
}

void Mainwin::show_message_dialog2()
{
	Gtk::MessageDialog dialog1(*this,"Please Enter all Information in the Entry Box",false,Gtk::MESSAGE_INFO);
	dialog1.run();
}

void Mainwin::show_message_dialog3()
{
	Gtk::MessageDialog dialog1(*this,"Welcome to POKER++\n Enjoy watching the game",false,Gtk::MESSAGE_INFO);
	dialog1.run();
}

void Mainwin::show_message_dialog4()
{
	Gtk::MessageDialog dialog1(*this,"Bet Value can't be $0",false,Gtk::MESSAGE_INFO);
	dialog1.run();
}

int main(int argc, char* argv[]) {
	/*auto app = Gtk::Application::create(argc, argv, APP_NAME);
	Mainwin win;
	return app->run(win);*/

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
	Gtk::Main::run(client_window);

	gtk_main();

	c->close();
	t.join();


	return 0;
}

