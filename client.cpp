//#include <iostream>
//#include "hand.h"
//#include "dealer.h"
//#include "player.h"
//#include "ranking.h"
#include "mainwin.h"



int main (int argc, char *argv[]) {
    auto app = Gtk::Application::create(argc, argv, APP_NAME);
    Mainwin win;
    return app->run(win);
}
