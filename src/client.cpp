#include "client.h"
#include <sstream>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <deque>
#include <thread>
#include <assert.h>
#include "asio.hpp"
#include "chat_message.hpp"
//#include "jason.hpp"

GtkWidget *fromView  = NULL;  // text from the chat server

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
chat_client *c;
//////////////////////////////////////////////////////////////////////////////////////

Mainwin::Mainwin()
  :betButton{Gtk::manage(new Gtk::Button{"Bet"})},
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
    std:: string text = "this is a test";
    //char line[chat_message::max_body_length + 1];
	
///////////////////////////////////////////

////////////////Server Message test////////

      chat_message msg;
      char x[text.size()+1];
      std::strcpy(x,text.c_str());
      msg.body_length(std::strlen(x));
      std::memcpy(msg.body(), x, msg.body_length());
      msg.encode_header();
      assert ( c );  // this is a global class
      c->write(msg);
   

///////////////////////////////////////////
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


int main (int argc, char *argv[]) {
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
   std::thread t([&io_context](){ io_context.run(); });
   
   

	Gtk::Main app(argc,argv);
  	Mainwin client_window;
  	Gtk::Main::run(client_window); 
    
   gtk_main();
   
   c->close();
   t.join();


	return 0;
}





