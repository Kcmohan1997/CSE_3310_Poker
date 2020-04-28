#include "client_window.h"

//using namespace std;
////////////////////////////
Mainwin::Mainwin()
	:betButton{ Gtk::manage(new Gtk::Button{"Bet"}) },
	checkButton{ Gtk::manage(new Gtk::Button{"Check"}) },
	foldButton{ Gtk::manage(new Gtk::Button{"Fold"}) },
	swapButton{ Gtk::manage(new Gtk::Button{"Swap"}) },
	nameEntry{ Gtk::manage(new Gtk::Label{""}) },
	betAmount{ Gtk::manage(new Gtk::Label{"$ 0"}) },
	totalAmount{ Gtk::manage(new Gtk::Label{"$ 0"}) }
{
	//sets window size
	set_default_size(620, 410);
	set_title(APP_TITLE);
	//override_background_color(Gdk::RGBA("green"));

	// Put a vertical box container as the Window contents
	Gtk::Box* vbox = Gtk::manage(new Gtk::VBox);
	add(*vbox);

	//Header Hbox
	Gtk::Box* hbox0 = Gtk::manage(new Gtk::HBox);
	vbox->pack_start(*hbox0, Gtk::PACK_SHRINK, 0);
	playButton = Gtk::manage(new Gtk::Button{ " PLAY " });
	hbox0->pack_start(*playButton, Gtk::PACK_SHRINK, 0);
	start_next_round_button = Gtk::manage(new Gtk::Button{ "START NEXT ROUND" });
	hbox0->pack_start(*start_next_round_button, Gtk::PACK_SHRINK, 0);
	start_next_round_button->signal_clicked().connect([this] {this->on_next_round_click(); });
	playButton->signal_clicked().connect([this] {this->on_play_click(); });

	///Gtk::Grid* grid = Gtk::manage(new Gtk::Grid);
	Gtk::Label* space1 = Gtk::manage(new Gtk::Label{ "\t" });
	Gtk::Label* total = Gtk::manage(new Gtk::Label{ "Total  :" });
	Gtk::Label* space2 = Gtk::manage(new Gtk::Label{ "\t" });
	Gtk::Label* bet = Gtk::manage(new Gtk::Label{ "Bet  :" });
	Gtk::Label* playerName = Gtk::manage(new Gtk::Label{ "Player Name: " });
	Gtk::Label* space3 = Gtk::manage(new Gtk::Label{ "\t\t" });

	hbox0->pack_start(*space1, Gtk::PACK_SHRINK, 0);
	hbox0->pack_start(*total, Gtk::PACK_SHRINK, 0);
	hbox0->pack_start(*totalAmount, Gtk::PACK_SHRINK, 0);
	hbox0->pack_start(*space2, Gtk::PACK_SHRINK, 0);
	hbox0->pack_start(*bet, Gtk::PACK_SHRINK, 0);
	hbox0->pack_start(*betAmount, Gtk::PACK_SHRINK, 0);
	hbox0->pack_start(*space3, Gtk::PACK_SHRINK, 0);
	hbox0->pack_start(*playerName, Gtk::PACK_SHRINK, 0);
	hbox0->pack_start(*nameEntry, Gtk::PACK_SHRINK, 0);

	// Card Display
	// Hbox of 5 card labels will be changable after integratio with dealer.
	Gtk::Box* hbox = Gtk::manage(new Gtk::HBox);
	card1 = Gtk::manage(new Gtk::Image{ "./src/Card_Images/playing_card.png" });
	card2 = Gtk::manage(new Gtk::Image{ "./src/Card_Images/playing_card.png" });
	card3 = Gtk::manage(new Gtk::Image{ "./src/Card_Images/playing_card.png" });
	card4 = Gtk::manage(new Gtk::Image{ "./src/Card_Images/playing_card.png" });
	card5 = Gtk::manage(new Gtk::Image{ "./src/Card_Images/playing_card.png" });
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

	//status bar where game updates will be given
	Gtk::Label* statusBox = Gtk::manage(new Gtk::Label{ "GAME STATUS" });
	Gtk::Label* player1 = Gtk::manage(new Gtk::Label{ "Player1: " });
	Gtk::Label* player2 = Gtk::manage(new Gtk::Label{ "Player2: " });
	Gtk::Label* player3 = Gtk::manage(new Gtk::Label{ "Player3: " });
	Gtk::Label* player4 = Gtk::manage(new Gtk::Label{ "Player4: " });
	Gtk::Label* player5 = Gtk::manage(new Gtk::Label{ "Player5: " });
	p1_status = Gtk::manage(new Gtk::Label("Waiting.."));
	p2_status = Gtk::manage(new Gtk::Label("Waiting.."));
	p3_status = Gtk::manage(new Gtk::Label("Waiting.."));
	p4_status = Gtk::manage(new Gtk::Label("Waiting.."));
	p5_status = Gtk::manage(new Gtk::Label("Waiting.."));
	hbox2->pack_start(*vbox2);
	vbox2->pack_start(*statusBox, Gtk::PACK_SHRINK, 0);
	statusBox->override_background_color(Gdk::RGBA{ "gray" });
	//player1->override_background_color(Gdk::RGBA{ "white" });

	Gtk::Grid* grid = Gtk::manage(new Gtk::Grid);
	grid->set_row_homogeneous(true);
	grid->attach(*player1, 0, 1, 1, 1);
	grid->attach(*p1_status, 1, 1, 1, 1);
	grid->attach(*player2, 0, 2, 1, 1);
	grid->attach(*p2_status, 1, 2, 1, 1);
	grid->attach(*player3, 0, 3, 1, 1);
	grid->attach(*p3_status, 1, 3, 1, 1);
	grid->attach(*player4, 0, 4, 1, 1);
	grid->attach(*p4_status, 1, 4, 1, 1);
	grid->attach(*player5, 0, 5, 1, 1);
	grid->attach(*p5_status, 1, 5, 1, 1);
	vbox2->pack_start(*grid);
	//chatField->set_hexpand(true);
	//chatField->set_vexpand(true);
	Gtk::Button* leaveButton = Gtk::manage(new Gtk::Button{ "Leave" });
	hbox2->pack_start(*leaveButton);
	leaveButton->signal_clicked().connect([this] {this->on_leave_click(); });
	vbox->show_all();

	//disabling the player mode buttons
	betButton->set_sensitive(button_state);
	swapButton->set_sensitive(button_state);
	checkButton->set_sensitive(button_state);
	foldButton->set_sensitive(button_state);
	start_next_round_button->set_sensitive(button_state);
}

//distructor
Mainwin::~Mainwin() { }

//closes gui when leave is clicked
void Mainwin::on_leave_click()
{
	close();
}

//sets bet amount after the bet and updates new bet amount in gui
void Mainwin::set_total_amount_after_bet(std::string amount)
{
	total_amount -= std::stoi(amount);
	std::stringstream s;
	s << total_amount;
	std::string new_amount = s.str();
	totalAmount->set_text("$" + new_amount);
}

void Mainwin::set_total_amount_after_win(int amount)
{
	total_amount += amount;
	std::stringstream s;
	s << total_amount;
	std::string new_amount = s.str();
	totalAmount->set_text("$" + new_amount);
}

///function to create message dialog box when player joins
void Mainwin::show_message_dialog1()
{
	Gtk::MessageDialog dialog1(*this, "Welcome to POKER++\nYou're starting with $500 credits.\n Default Ante: $2", false, Gtk::MESSAGE_INFO);
	dialog1.run();
}

//function to create message dialog box when entry is left blank
void Mainwin::show_message_dialog2()
{
	Gtk::MessageDialog dialog1(*this, "Please Enter all Information in the Entry Box", false, Gtk::MESSAGE_INFO);
	dialog1.run();
}

//function to create message dialog box when spectator joins the game
void Mainwin::show_message_dialog3()
{
	Gtk::MessageDialog dialog1(*this, "Welcome to POKER++\n Enjoy watching the game", false, Gtk::MESSAGE_INFO);
	dialog1.run();
}

//function to create message dialog box when bet is zero
void Mainwin::show_message_dialog4()
{
	Gtk::MessageDialog dialog1(*this, "Bet Value can't be $0", false, Gtk::MESSAGE_INFO);
	dialog1.run();
}

//method gets called when swaping card is more than three
void Mainwin::show_message_dialog5()
{
	Gtk::MessageDialog dialog(*this, "! You can only swap upto three card.", false, Gtk::MESSAGE_INFO);
	dialog.run();
}

//method is called when invalid bet amount is entered
void Mainwin::show_message_dialog8()
{
	Gtk::MessageDialog dialog(*this, "! Please Enter $1 to $5 ", false, Gtk::MESSAGE_INFO);
	dialog.run();
}

//displays swapping status
void Mainwin::show_message_dialog9()
{
	Gtk::MessageDialog dialog(*this, "! Swapping is done for this round ", false, Gtk::MESSAGE_INFO);
	dialog.run();
}

//function to generate uuid to the player joining the game
std::string Mainwin::generate_uuid()
{
	boost::uuids::random_generator generator;
	boost::uuids::uuid u = generator();
	std::string uuid;
	std::stringstream s;
	s << u;
	uuid = s.str();

	return uuid;
}
