#ifndef __CLIENT_WINDOW_H
#define __CLIENT_WINDOW_H

#include <gtkmm.h>
#include <vector>
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
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <mutex>

const std::string APP_TITLE{ "Poker ++" };
const std::string APP_NAME{ "edu.uta.cse3310.poker.v0_1_0" };


class Mainwin : public Gtk::Window {
public:
	Mainwin();
	virtual ~Mainwin();
	void on_next_round_click();
	void on_play_click();
	void on_bet_click();
	void on_check_click();
	void on_fold_click();
	void on_swap_click();
	void on_leave_click();
	void check_player_mode_entry(std::string);
	void show_message_dialog1();
	void show_message_dialog2();
	void show_message_dialog3();
	void show_message_dialog4();
	void show_message_dialog5();
	void show_message_dialog6();
	void show_message_dialog7();
	void show_message_dialog8();
	void show_message_dialog9();
	void show_message_dialog10();
	void set_card1(std::string);
	void set_card2(std::string);
	void set_card3(std::string);
	void set_card4(std::string);
	void set_card5(std::string);
	void set_total_amount_after_bet(std::string);
	void set_total_amount_after_win(int);
	std::string generate_uuid();

	//private:
	Gtk::Button* playButton;
	Gtk::Button* betButton;
	Gtk::Button* checkButton;
	Gtk::Button* foldButton;
	Gtk::Button* swapButton;
	Gtk::Button* leaveButton;
	Gtk::Button* start_next_round_button;
	Gtk::Label* nameEntry;
	Gtk::Image* card1;
	Gtk::Image* card2;
	Gtk::Image* card3;
	Gtk::Image* card4;
	Gtk::Image* card5;
	Gtk::Label* betAmount;
	Gtk::Label* totalAmount;
	Gtk::Label* p1_status;
	Gtk::Label* p2_status;
	Gtk::Label* p3_status;
	Gtk::Label* p4_status;
	Gtk::Label* p5_status;
	bool play_button_mode = true;
	bool button_state = false;
	int total_amount = 100;
	int bet_amount = 0;
	std::string player_name;
	std::string uuid;
	int count_swap = 0;
	std::mutex m;
};
#endif 
