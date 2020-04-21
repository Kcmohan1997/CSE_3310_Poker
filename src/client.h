#ifndef __CLIENT_H
#define __CLIENT_H

#include <gtkmm.h>
#include <vector>

const std::string APP_TITLE{"Poker ++"};
const std::string APP_NAME{"edu.uta.cse3310.poker.v0_1_0"};

class Mainwin : public Gtk::Window {
    public:
        Mainwin();
        virtual ~Mainwin();
    protected:
	void on_spectate_click(); 
 	void on_play_click();
        void on_bet_click();   
        void on_check_click();        
        void on_fold_click();                      
        void on_swap_click();
        void on_leave_click();
	void check_player_mode_entry(std::string,std::string);
	void show_message_dialog1();
	void show_message_dialog2();
	void show_message_dialog3();
	void show_message_dialog4();
	       
    private:
	Gtk::Button* playButton;
    	Gtk::Button* betButton;
    	Gtk::Button* checkButton;
    	Gtk::Button* foldButton;
        Gtk::Button* swapButton;
	Gtk::ToggleButton* spectateButton;
        Gtk::Label* nameEntry;
	Gtk::Label* card1;
	Gtk::Label* card2;
	Gtk::Label* card3;
	Gtk::Label* card4;
	Gtk::Label* card5;
        Gtk::Label* betAmount;
        Gtk::Label* totalAmount;
        bool play_button_mode = true;
	bool button_state = false;
	int total_amount = 500;
	int bet_amount = 0;
	std::vector <std::string> poker_card;
};
#endif 
