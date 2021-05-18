#include "GTK.h"

Mainwin::Mainwin(chat_client *c) 
{
  chat = c;

  /*nlohmann::json to_dealer;
  to_dealer["name"] = player.get_name();
  to_dealer["uuid"] = player.get_UUID();
  to_dealer["money"] = player.get_money();
  to_dealer["action"] = "standby";
  update_server(to_dealer);*/
  player.set_action("standby");
  can_join = false;
  can_bet = false;
  can_exchange = false;
  discard1 = false;
  discard2 = false;
  discard3 = false;
  discard4 = false;
  discard5 = false;
  // /////////////////
  // G U I   S E T U P
  // /////////////////

  set_default_size(600, 300);
  set_title("Poker++");

  // Put a vertical box container as the Window contents
  vbox = Gtk::manage(new Gtk::VBox);
  add(*vbox);

  // ///////
  // M E N U
  // Add a menu bar as the top item in the vertical box
  Gtk::MenuBar *menubar = Gtk::manage(new Gtk::MenuBar);
  vbox->pack_start(*menubar, Gtk::PACK_SHRINK, 0);

  //     F I L E
  // Create a File menu and add to the menu bar
  Gtk::MenuItem *menuitem_file = Gtk::manage(new Gtk::MenuItem("_File", true));
  menubar->append(*menuitem_file);
  Gtk::Menu *filemenu = Gtk::manage(new Gtk::Menu());
  menuitem_file->set_submenu(*filemenu);

  //         Q U I T
  // Append Quit to the File menu
  Gtk::MenuItem *menuitem_quit = Gtk::manage(new Gtk::MenuItem("_Quit", true));
  menuitem_quit->signal_activate().connect([this] {this->on_quit_click();});
  filemenu->append(*menuitem_quit);

  //     H E L P
  // Create a Help menu and add to the menu bar
  Gtk::MenuItem *menuitem_help = Gtk::manage(new Gtk::MenuItem("_Help", true));
  menubar->append(*menuitem_help);
  Gtk::Menu *helpmenu = Gtk::manage(new Gtk::Menu());
  menuitem_help->set_submenu(*helpmenu);

  //           A B O U T
  // Append About to the Help menu
  Gtk::MenuItem *menuitem_about = Gtk::manage(new Gtk::MenuItem("About", true));
  menuitem_about->signal_activate().connect([this] {this->on_about_click();});
  helpmenu->append(*menuitem_about);

  //              I D
  // Displays the user's name and uuid
  UserID = Gtk::manage(new Gtk::Label(player.get_name() + "\t" + player.get_UUID()));
  vbox->pack_start(*UserID);

  // /////////////
  // T O O L B A R
  // Add a toolbar to the vertical box below the menu
  Gtk::Toolbar *toolbar = Gtk::manage(new Gtk::Toolbar);
  vbox->pack_end(*toolbar);

  //     H A N D B U T T O N
  // Add Display Hand buttons
  hbox = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_HORIZONTAL));
  vbox->pack_start(*hbox);

  card1_button_image = Gtk::manage(new Gtk::Image{"back.png"});
  card1_button = Gtk::manage(new Gtk::ToolButton(*card1_button_image));
  card1_button->signal_clicked().connect([this] {this->on_card1_click();});
  hbox->pack_start(*card1_button);

  card2_button_image = Gtk::manage(new Gtk::Image{"back.png"});
  card2_button = Gtk::manage(new Gtk::ToolButton(*card2_button_image));
  card2_button->signal_clicked().connect([this] {this->on_card2_click();});
  hbox->pack_start(*card2_button);

  card3_button_image = Gtk::manage(new Gtk::Image{"back.png"});
  card3_button = Gtk::manage(new Gtk::ToolButton(*card3_button_image));
  card3_button->signal_clicked().connect([this] {this->on_card3_click();});
  hbox->pack_start(*card3_button);


  card4_button_image = Gtk::manage(new Gtk::Image{"back.png"});
  card4_button = Gtk::manage(new Gtk::ToolButton(*card4_button_image));
  card4_button->signal_clicked().connect([this] {this->on_card4_click();});
  hbox->pack_start(*card4_button);

  card5_button_image = Gtk::manage(new Gtk::Image{"back.png"});
  card5_button = Gtk::manage(new Gtk::ToolButton(*card5_button_image));
  card5_button->signal_clicked().connect([this] {this->on_card5_click();});
  hbox->pack_start(*card5_button);

  //     E X C H A N G E   B U T T O N
  // Add bet/raise button
  exchange_button_image = Gtk::manage(new Gtk::Image{"cards.png"});
  exchange_button = Gtk::manage(new Gtk::ToolButton(*exchange_button_image));
  exchange_button->set_tooltip_markup("Exchange");
  exchange_button->signal_clicked().connect([this] {this->on_exchange_click();});
  toolbar->append(*exchange_button);

  //     R A I S E  B U T T O N
  // Add bet/raise button
  raise_button_image = Gtk::manage(new Gtk::Image{"money.png"});
  raise_button = Gtk::manage(new Gtk::ToolButton(*raise_button_image));
  raise_button->set_tooltip_markup("Bet/Raise");
  raise_button->signal_clicked().connect([this] {this->on_raise_click();});
  toolbar->append(*raise_button);

  //     C H E C K  B U T T O N
  // Add check button
  check_button_image = Gtk::manage(new Gtk::Image{"pass.png"});
  check_button = Gtk::manage(new Gtk::ToolButton(*check_button_image));
  check_button->set_tooltip_markup("check");
  check_button->signal_clicked().connect([this] {this->on_check_click();});
  toolbar->append(*check_button);

  //     F O L D B U T T O N
  // Add fold button
  fold_button_image = Gtk::manage(new Gtk::Image{"fold.png"});
  fold_button = Gtk::manage(new Gtk::ToolButton(*fold_button_image));
  fold_button->set_tooltip_markup("Fold");
  fold_button->signal_clicked().connect([this] {this->on_fold_click();});
  toolbar->append(*fold_button);

  //     J O I N  B U T T O N
  // Add join button
  join_button = Gtk::manage(new Gtk::ToolButton("JOIN"));
  join_button->set_tooltip_markup("join");
  join_button->signal_clicked().connect([this] {this->on_join_click();});
  toolbar->append(*join_button);

  // ///////////////////////
  // D A T A   D I S P L A Y
  // Provide a text entry box to show the remaining pot value
  server_message = Gtk::manage(new Gtk::Label("Status: Starting"));
  vbox->pack_start(*server_message);

  player_money = Gtk::manage(new Gtk::Label("Money: $"+ std::to_string(player.get_money())));
  vbox->pack_start(*player_money);

  player_bet = Gtk::manage(new Gtk::Label("Setting bet: "));
  vbox->pack_start(*player_bet);

  pot_value = Gtk::manage(new Gtk::Label("Current pot value: "));
  vbox->pack_start(*pot_value);
  
  reset_sensitivity();

  // Make the box and everything in it visible
  vbox->show_all();
}

Mainwin::~Mainwin()
{}

void Mainwin::set_up(std::string name, std::string uuid)
{
  player.set_player(name, uuid);
}

void Mainwin::update_server(nlohmann::json to_dealer)
{
  chat_message msg;
  std::string t = to_dealer.dump();

  //std::cout << t << std::endl;

  msg.body_length(t.size());
  std::memcpy(msg.body(), t.c_str() , msg.body_length());
  msg.encode_header();
  chat->write(msg);
}

std::string Mainwin::find_pic(int card)
{
  int value = card % 100;
  int suit = card / 100;
  std::string pic;

  if(value == 1)
    pic = "A";
  else if(value == 11)
    pic = "J";
  else if(value == 12)
    pic = "Q";
  else if(value == 13)
    pic = "K";
  else
    pic = std::to_string(value);

  if(suit == 0)
    pic += "H";
  else if(suit == 1)
    pic += "D";
  else if(suit == 2)
    pic += "C";
  else if(suit == 3)
    pic += "S";
  else
    pic = "back";

  pic += ".png";
  return pic;
}


void Mainwin::update_cards(int card1, int card2, int card3, int card4, int card5)
{
  std::vector<int> cards = {card1, card2, card3, card4, card5};
  std::sort(cards.begin(), cards.end(), [](int a, int b){return a % 100 < b % 100;});
  card1 = cards.at(0);
  card2 = cards.at(1);
  card3 = cards.at(2);
  card4 = cards.at(3);
  card5 = cards.at(4);
  player.player_discard(1);
  card1_button_image->set(Gdk::Pixbuf::create_from_file(find_pic(card1)));
  card2_button_image->set(Gdk::Pixbuf::create_from_file(find_pic(card2)));
  card3_button_image->set(Gdk::Pixbuf::create_from_file(find_pic(card3)));
  card4_button_image->set(Gdk::Pixbuf::create_from_file(find_pic(card4)));
  card5_button_image->set(Gdk::Pixbuf::create_from_file(find_pic(card5)));

  player.player_draw(card1);
  player.player_draw(card2);
  player.player_draw(card3);
  player.player_draw(card4);
  player.player_draw(card5);
}

void Mainwin::update_status(std::string status)
{
  server_message->set_text("Status: " + status);
}

void Mainwin::update_sensitivity(std::string sensitivity, bool set)
{
  if(sensitivity == "join")
    can_join = set;
  else if(sensitivity == "bet")
    can_bet = set;
  else if(sensitivity == "exchange")
    can_exchange = set;
  reset_sensitivity();
}

void Mainwin::update_setting_bet(int bet)
{
  player_bet->set_text("Setting bet: $" + std::to_string(bet));
  setting_bet = bet;
}

void Mainwin::update_pot(int bet)//updt
{
	pot_value-> set_text("Current pot value: $" + std::to_string(bet));
	pot = bet;
}
void Mainwin::update_money(int bet)//updt
{
	Money -= bet;
	player_money-> set_text("Money: $" + std::to_string(Money));
	
}

std::string Mainwin::client_uuid()
{
  return player.get_UUID();
}

void Mainwin::win_lose(int first)
{
  if(first == player.get_player_card(0))
  {
    player.set_money(player.get_money() + pot);
    total_bet = 0;
    //update_pot(0);
    update_status("You won");
  }
  else
  {
    player.set_money(player.get_money() - total_bet);
    total_bet = 0;
    //update_pot(0);
    update_status("You lose");
  }
  //std::this_thread::sleep_for (std::chrono::seconds(5));

  /*nlohmann::json to_dealer;
  
  to_dealer["name"] = player.get_name();
  to_dealer["uuid"] = player.get_UUID();
  to_dealer["money"] = player.get_money();
  to_dealer["action"] = "standby";
  player.set_action("standby");*/

  update_sensitivity("join", false);
  update_sensitivity("bet", false);
  update_sensitivity("exchange", false);

  //update_server(to_dealer);

}

void Mainwin::on_card1_click()
{
  discard1 = !discard1;
}

void Mainwin::on_card2_click()
{
  discard2 = !discard2;
}

void Mainwin::on_card3_click()
{
  discard3 = !discard3;
}

void Mainwin::on_card4_click()
{
  discard4 = !discard4;
}

void Mainwin::on_card5_click()
{
  discard5 = !discard5;
}

void Mainwin::on_quit_click()
{
  close();
}

void Mainwin::on_exchange_click()
{
  nlohmann::json to_dealer;

  if(!discard1)
    to_dealer["cards"].push_back(player.get_player_card(0));
  if(!discard2)
    to_dealer["cards"].push_back(player.get_player_card(1));
  if(!discard3)
    to_dealer["cards"].push_back(player.get_player_card(2));
  if(!discard4)
    to_dealer["cards"].push_back(player.get_player_card(3));
  if(!discard5)
    to_dealer["cards"].push_back(player.get_player_card(4));

  to_dealer["name"] = player.get_name();
  to_dealer["uuid"] = player.get_UUID();
  to_dealer["money"] = player.get_money();
  to_dealer["action"] = "exchange";
  to_dealer["bet"] = total_bet;
  //to_dealer["cards"] = player.get_hand();
  player.set_action("exchange");

  update_server(to_dealer);
  update_sensitivity("exchange", false);
}

void Mainwin::on_raise_click()
{
  chat_message msg;
  nlohmann::json to_dealer;

  Gtk::Dialog adialog{"Raise", *this};
  Gtk::Grid grid;
  Gtk::Label l_bet{"Raise amount: "};
  Gtk::Entry e_bet;
  grid.attach(l_bet, 0, 0, 1, 1);
  grid.attach(e_bet, 1, 0, 2, 1);
  adialog.get_content_area()->pack_start(grid, Gtk::PACK_SHRINK, 0);
  adialog.add_button("Raise", 1);
  adialog.add_button("Cancel", 0);
  adialog.show_all();
  if(adialog.run() == 0)
  {
    return;
  }

  total_bet = setting_bet + std::stoi(e_bet.get_text());
 total_pot = pot +  std::stoi(e_bet.get_text());//updt
 Money -= std::stoi(e_bet.get_text());//updt

  to_dealer["name"] = player.get_name();
  to_dealer["uuid"] = player.get_UUID();
  to_dealer["money"] = player.get_money();
  to_dealer["action"] = "bet";
  to_dealer["bet"] = total_bet;
  to_dealer["pot"] = total_pot;//updt
  to_dealer["Money"] = Money;//updt
  to_dealer["cards"] = {player.get_player_card(0), player.get_player_card(1), player.get_player_card(2), player.get_player_card(3), player.get_player_card(4)};
  player.set_action("bet");

  update_server(to_dealer);
  update_sensitivity("bet", false);

}

void Mainwin::on_fold_click()
{
  chat_message msg;
  nlohmann::json to_dealer;

  to_dealer["name"] = player.get_name();
  to_dealer["uuid"] = player.get_UUID();
  to_dealer["money"] = player.get_money();
  to_dealer["action"] = "fold";
  to_dealer["cards"] = {player.get_player_card(0), player.get_player_card(1), player.get_player_card(2), player.get_player_card(3), player.get_player_card(4)};
  player.set_action("fold");
  total_bet = 0;

  update_server(to_dealer);

  update_cards(999, 999, 999, 999, 999);
  update_sensitivity("bet", false);
}

void Mainwin::on_check_click()
{
  chat_message msg;
  nlohmann::json to_dealer;

  total_bet = setting_bet;

  to_dealer["name"] = player.get_name();
  to_dealer["uuid"] = player.get_UUID();
  to_dealer["money"] = player.get_money();
  to_dealer["action"] = "check";
  to_dealer["bet"] = total_bet;
  to_dealer["cards"] = {player.get_player_card(0), player.get_player_card(1), player.get_player_card(2), player.get_player_card(3), player.get_player_card(4)};
  player.set_action("check");

  update_server(to_dealer);
  update_sensitivity("bet", false);
}

void Mainwin::on_join_click()
{
  if(player.get_action() == "standby")
  {
    nlohmann::json to_dealer;

    to_dealer["name"] = player.get_name();
    to_dealer["uuid"] = player.get_UUID();
    to_dealer["money"] = player.get_money();
    to_dealer["action"] = "join";
    player.set_action("joined");

    update_server(to_dealer);
  }
}

void Mainwin::on_about_click()
{
  Glib::ustring s = R"(
<span size='24000' weight='bold'>Poker ++</span>

Hand values are as follows: 
1. Royal Flush: The highest hand will be a royal flush having 5 consecutive  cards of the same suit from 10 => ace.
2. Straight Flush: The next hand is a straight flush having 5 consecutive cards of the same suit.
3. 4 of a kind: The next hand in order is a hand containing 4 equal valued cards with one extra card.
4. Full House: It has a combination of 3 of a kind and a pair.
5. Flush: It contains 5 cards of the same suit.
6. Straight: It contains 5 consecutive cards of different suits.
7. 3 of a kind: It contains 3 equal valued cards.
8. Two pairs: It contains 2 pairs of cards and one extra card.
9. One pair: It contains one pair of cards.
10. Lone high card: This hand is based on the highest card it has.

Swapping Card Rules:
1. To swap the cards, player has to enter the place of card on hand to be exchanged.
2. Player can exchange upto three cards.

)";
  Gtk::MessageDialog dlg(*this, s, true, Gtk::MESSAGE_INFO, Gtk::BUTTONS_OK, true);
  dlg.run();
}

void Mainwin::reset_sensitivity()
{
  card1_button->set_sensitive(can_exchange);
  card2_button->set_sensitive(can_exchange);
  card3_button->set_sensitive(can_exchange);
  card4_button->set_sensitive(can_exchange);
  card5_button->set_sensitive(can_exchange);

  exchange_button->set_sensitive(can_exchange);
  raise_button->set_sensitive(can_bet);
  fold_button->set_sensitive(can_bet);
  check_button->set_sensitive(can_bet);
  join_button->set_sensitive(!can_join);
}
