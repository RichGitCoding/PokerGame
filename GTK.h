//GROUP 003-8
#ifndef _GTK_H
#define _GTK_H

#include <gtkmm.h>
#include <chrono>
#include <thread>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/lexical_cast.hpp>
#include "json.hpp"
#include "Player.cpp"
#include "Poker_client.h"

class Mainwin : public Gtk::Window
{
  public:   
    Mainwin(chat_client *c);
    virtual ~Mainwin();
    void set_up(std::string name, std::string uuid);
    void update_server(nlohmann::json to_dealer);
    std::string find_pic(int card);
    void update_sensitivity(std::string sensitivity, bool set);
    void update_cards(int card1, int card2, int card3, int card4, int card5);
    void update_status(std::string status);
    void update_setting_bet(int bet);
	
	void update_pot(int bet);//updt
	void update_money(int bet);//updt
	
    void reset_sensitivity(); 
    std::string client_uuid();
    void win_lose(int first);

  protected:
    void on_quit_click();
    void on_exchange_click();
    void on_raise_click();
    void on_fold_click();
    void on_check_click();
    void on_join_click();
    void on_about_click();
    void on_card1_click();
    void on_card2_click();
    void on_card3_click();
    void on_card4_click();
    void on_card5_click();

  private:
    Gtk::Label *UserID;

    Gtk::Box *vbox;
    Gtk::Box *hbox;

    Gtk::Image *card1_button_image;
    Gtk::Image *card2_button_image;
    Gtk::Image *card3_button_image;
    Gtk::Image *card4_button_image;
    Gtk::Image *card5_button_image;
    Gtk::ToolButton *card1_button;
    Gtk::ToolButton *card2_button;
    Gtk::ToolButton *card3_button;
    Gtk::ToolButton *card4_button;
    Gtk::ToolButton *card5_button;
    bool discard1;
    bool discard2;
    bool discard3;
    bool discard4;
    bool discard5;

    Gtk::ToolButton *exchange_button;
    Gtk::Image *exchange_button_image;
    Gtk::ToolButton *raise_button;
    Gtk::Image *raise_button_image;
    Gtk::ToolButton *fold_button;
    Gtk::Image *fold_button_image;
    Gtk::ToolButton *check_button;
    Gtk::Image *check_button_image;
    Gtk::ToolButton *join_button;

    Gtk::Label *server_message;
    Gtk::Label *player_money;
    Gtk::Label *player_bet;
    Gtk::Label *pot_value;

    Player player;
    int total_bet;
    int pot;
	int total_pot;//updt
    int setting_bet;
    bool can_join;
    bool can_bet;
	int Money = 100;//updt
    bool can_exchange;
    chat_client *chat;
  
};

#endif
