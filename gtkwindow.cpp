#include <cstdlib>
#include <csignal>
#include <iostream>
#include <gtk/gtk.h>
#include <gtk/gtkx.h>
#include <math>
#include <sys/types>
#include <ctype>


GtkWidget *Client_window;
GtkWidget *grid_for_client
GtkWidget *Bet
GtkWidget *Check
GtkWidget *Fold
GtkWidget *Swap_cards
GtkBuilder *builder;

int main(int argc, char *argv[])
{
	builder = gtk_builder_new_from_file("Client gui.glade");
	Client_window = GTK_WIDGET(gtk_builder_get_object(builder,"Client_window"));
	g_signal_connect(Client_Window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
	gtk_builder_connect_signal(builder,NULL);

	grid_for_client = GTK_WIDGET(gtk_builder_get_object(builder,"grid_for_client"));
	Bet = GTK_WIDGET(gtk_builder_get_object(builder,"Bet"));
	Check = GTK_WIDGET(gtk_builder_get_object(builder,"Check"));
	Fold = GTK_WIDGET(gtk_builder_get_object(builder,"Fold"));
	Swap_cards = GTK_WIDGET(gtk_builder_get_object(builder,"Swap_cards"));

	gtk_widget_show(Client_window);

	gtk_main();

	return EXIT_SUCCESS;
}

