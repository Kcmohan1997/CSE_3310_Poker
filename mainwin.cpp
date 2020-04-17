#include "mainwin.h"
#include <sstream>
#include <fstream>
#include <iostream>

using namespace std;

Mainwin::Mainwin()
  : betButton{Gtk::manage(new Gtk::Button{"Bet"})},
    checkButton{Gtk::manage(new Gtk::Button{"Check"})},
    foldButton{Gtk::manage(new Gtk::Button{"Fold"})},
    swapButton{Gtk::manage(new Gtk::Button{"Swap"})},
    spectateButton{Gtk::manage(new Gtk::ToggleButton{" SPECTATE "})},
    nameEntry{Gtk::manage(new Gtk::Label{"\t\t"})},
    card1{Gtk::manage(new Gtk::Label{"Card 1"})},
    card2{Gtk::manage(new Gtk::Label{"Card 2"})},
    card3{Gtk::manage(new Gtk::Label{"Card 3"})},
    card4{Gtk::manage(new Gtk::Label{"Card 4"})},
    card5{Gtk::manage(new Gtk::Label{"Card 5"})},
    betAmount{Gtk::manage(new Gtk::Label{"$ 0"})},
    totalAmount{Gtk::manage(new Gtk::Label{"$ 0"})}
{
    // Gui Setup

    set_default_size(800, 600);
    set_title(APP_TITLE);

    // Put a vertical box container as the Window contents
    Gtk::Box *vbox = Gtk::manage(new Gtk::VBox);
    add(*vbox);
    //Header Hbox
    Gtk::Box *hbox0 = Gtk::manage(new Gtk::HBox);
    vbox->pack_start(*hbox0, Gtk::PACK_SHRINK, 0);
    hbox0->pack_start(*spectateButton, Gtk::PACK_SHRINK, 0);
    spectateButton->signal_clicked().connect([this] {this->on_spectate_click();});
    ///*if we want an ante button
    playButton = Gtk::manage(new Gtk::Button{" PLAY "});
    hbox0->pack_start(*playButton, Gtk::PACK_SHRINK, 0);
    playButton->signal_clicked().connect([this] {this->on_play_click();});
    //*/
    Gtk::Label *time = Gtk::manage(new Gtk::Label{"Time"});
    hbox0->pack_start(*time, Gtk::PACK_SHRINK, 0);
    time->set_hexpand(true);
    Gtk::Label *playerName = Gtk::manage(new Gtk::Label{"Player Name: "});
   
    hbox0->pack_start(*playerName, Gtk::PACK_SHRINK, 0);
    hbox0->pack_start(*nameEntry, Gtk::PACK_SHRINK, 0);

    // Card Display
    // Hbox of 5 labels, labels will be changable after integratio with dealer.
    Gtk::Box *hbox = Gtk::manage(new Gtk::HBox);
    vbox->pack_start(*hbox);
    
    hbox->pack_start(*card1);
    hbox->pack_start(*card2);
    hbox->pack_start(*card3);
    hbox->pack_start(*card4);
    hbox->pack_start(*card5);
    //Interactive zone of buttons and labels held in another hbox
    Gtk::Box *hbox2 = Gtk::manage(new Gtk::HBox);
    vbox->pack_start(*hbox2);
    hbox2->pack_start(*betButton);
    betButton->signal_clicked().connect([this] {this->on_bet_click();});
    hbox2->pack_start(*checkButton);
    checkButton->signal_clicked().connect([this] {this->on_check_click();});
    hbox2->pack_start(*foldButton);
    foldButton->signal_clicked().connect([this] {this->on_fold_click();});
    hbox2->pack_start(*swapButton);
    swapButton->signal_clicked().connect([this] {this->on_swap_click();});
    Gtk::Box *vbox2 = Gtk::manage(new Gtk::VBox);
    Gtk::Label *chatBox = Gtk::manage(new Gtk::Label{"Chat Area"});
    Gtk::Label *chatField = Gtk::manage(new Gtk::Label{"Chat field"});
    hbox2->pack_start(*vbox2);
    vbox2->pack_start(*chatBox, Gtk::PACK_SHRINK, 0);
    chatBox->override_background_color(Gdk::RGBA{"gray"});
    chatField->override_background_color(Gdk::RGBA{"white"});
    vbox2->pack_start(*chatField);
    chatField->set_hexpand(true);
    chatField->set_vexpand(true);
    Gtk::Grid *grid = Gtk::manage (new Gtk::Grid);
    Gtk::Label *space = Gtk::manage(new Gtk::Label{});
    space->set_vexpand(true);
    grid->attach(*space, 0, 1, 1, 1);
    Gtk::Label *total = Gtk::manage(new Gtk::Label{"Total"});
    grid->attach(*total, 0, 1, 1, 1);
    grid->attach(*totalAmount, 2, 1, 2, 1);
    Gtk::Label *bet = Gtk::manage(new Gtk::Label{"\tBet Amount"});
    grid->attach(*bet, 0, 2, 1, 1);
    grid->attach(*betAmount, 2, 2, 2, 1);
    hbox2->pack_start(*grid);
    Gtk::Button *leaveButton = Gtk::manage(new Gtk::Button{"Leave"});
    hbox2->pack_start(*leaveButton);
    leaveButton->signal_clicked().connect([this] {this->on_leave_click();});

    vbox->show_all();
}

Mainwin::~Mainwin() { }


/// Callback to methods
    void Mainwin::on_bet_click() {
    Gtk::Dialog dialog{"Betting window", *this};

    Gtk::Grid grid;

    Gtk::Label l_name{"How much will you bet"};
    Gtk::Entry e_name;
    grid.attach(l_name, 0, 0, 1, 1);
    grid.attach(e_name, 1, 0, 2, 1);

    dialog.get_content_area()->add(grid);

    dialog.add_button("Place Bet", 1);
    dialog.add_button("Cancel Bet", 0);

    dialog.show_all();
    dialog.run();
    betAmount->set_text("$" + e_name.get_text());
    }


    void Mainwin::on_check_click() {
    betAmount->set_text("$0");
    }   

     
    void Mainwin::on_fold_click() {
    betButton->set_sensitive(false);
    swapButton->set_sensitive(false);
    checkButton->set_sensitive(false); 
    foldButton->set_sensitive(false);
    }     
                
    void Mainwin::on_swap_click() {
    Gtk::Dialog dialog{"Swap Decision", *this};

    Gtk::Grid grid;

    Gtk::Label l_name{"Which Cards do you wish to swap"};
    Gtk::Label directions{"Please type numbers corresponding to cards with spaces in between, or type 'all'"};
    Gtk::Entry e_name;
    grid.attach(l_name, 0, 0, 1, 1);
    grid.attach(directions, 0, 1, 2, 1);
    grid.attach(e_name, 0, 2, 2, 1);

    dialog.get_content_area()->add(grid);

    dialog.add_button("Swap", 1);
    dialog.add_button("Cancel Swap", 0);

    dialog.show_all();
    dialog.run();
    }


    void Mainwin::on_spectate_click() {
    //spectate->set_active(false);	
    bool state;
    if (spectateButton->get_active() == true)
    { 
	state = false;
    	play_button_mode = true;
	playButton->set_sensitive(play_button_mode);
    }
    else
    {
    	state = true;
    }
    betButton->set_sensitive(state); 
    swapButton->set_sensitive(state); 
    checkButton->set_sensitive(state); 
    foldButton->set_sensitive(state); 	
    }

    void Mainwin::on_play_click(){

        Gtk::Dialog dialog{"Player Info",*this};
	Gtk::Grid grid;
	Gtk::Label message1{"Enter Name"};
        Gtk::Entry name_Entry;
        Gtk::Label message2{"Enter Ante $"};
        Gtk::Entry ante_Entry;
        grid.attach(message1, 0, 0, 1, 1);
        grid.attach(name_Entry, 1, 0, 2, 1);
	grid.attach(message2, 0, 1, 1, 1);
        grid.attach(ante_Entry, 1, 1, 2, 1);
        dialog.get_content_area()->add(grid);
	dialog.add_button("Enter",0);

        dialog.show_all();
	dialog.run();
        
	nameEntry->set_text(name_Entry.get_text()+"\t");
        //xxxx->set_text(ante_Entry.get_text()+" "); //will change later
        play_button_mode = false;
        //spectateButton->set_sensitive(play_button_mode);
        playButton->set_sensitive(play_button_mode);
	betButton->set_sensitive(true);
	swapButton->set_sensitive(true);
	checkButton->set_sensitive(true);
	foldButton->set_sensitive(true);	
    }
   
    void Mainwin::on_leave_click() {
    close();
    }
