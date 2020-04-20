#ifndef __CLIENT_H
#define __CLIENT_H

#include <gtkmm.h>

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
};
#endif 
