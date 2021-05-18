
#include <cstdlib>
#include <deque>
#include <iostream>
#include <thread>
#include "asio.hpp"
#include "chat_message.hpp"
#include "json.hpp"

#include "Poker_client.h"
#include "GTK.cpp"

Mainwin *win = nullptr;

std::string PlayerID = boost::uuids::to_string(boost::uuids::random_generator()());

//////////////////////////////////////////////////////////////////////////////////////////

chat_client::chat_client(asio::io_context& io_context, const tcp::resolver::results_type& endpoints) : io_context_(io_context), socket_(io_context)
{
  client_status = "starting";
  do_connect(endpoints);
}

void chat_client::write(const chat_message& msg)
{
  asio::post(io_context_, [this, msg]()
  {
    bool write_in_progress = !write_msgs_.empty();
    write_msgs_.push_back(msg);
    if (!write_in_progress)
    {
      do_write();
    }
  });
}

void chat_client::close()
{
  asio::post(io_context_, [this]() { socket_.close(); });
}


void chat_client::do_connect(const tcp::resolver::results_type& endpoints)
{
  asio::async_connect(socket_, endpoints, [this](std::error_code ec, tcp::endpoint)
  {
    if (!ec)
    {
      do_read_header();
    }
  });
}

void chat_client::do_read_header()
{
  asio::async_read(socket_, asio::buffer(read_msg_.data(), chat_message::header_length), [this](std::error_code ec, std::size_t)
  {
    if (!ec && read_msg_.decode_header())
    {
      // clear out the old buffer from the last read
      // a '\0' is a good value to make sure a string
      // is terminated
      for (unsigned int i=0;i<chat_message::max_body_length;i++)
      {
        read_msg_.body() [i] = '\0';
      }
      do_read_body();
    }
    else
    {
      socket_.close();
    }
  });
}

void chat_client::do_read_body()
{
  asio::async_read(socket_, asio::buffer(read_msg_.body(), read_msg_.body_length()), [this](std::error_code ec, std::size_t)
  {
    if (!ec)
    {
		gdk_threads_enter();//updt
		
      //Reads the JSON from the server which is then parsed
      nlohmann::json to_player = nlohmann::json::parse(std::string(read_msg_.body()));

      /*****************************************************************
        This section of code is the response to the NEW GAME section 
        from the server side
      ******************************************************************/
      if(to_player.find(PlayerID) != to_player.end())
      {
        if(to_player[PlayerID].find("phase") !=  to_player[PlayerID].end())
        {
          if(to_player[PlayerID]["phase"].get<std::string>() == "joined")
          {
            //This will update the player's status which will be "joined"
            win->update_status(to_player[PlayerID]["phase"].get<std::string>());
            //This will change the sensitivity on the "join" button so it will be able to be used 
            win->update_sensitivity("join", true);
          }
        }
      }

      /******************************************************************
        This section code is the response to the DEAL CARDS section from
        the server where the client receives and interpret the cards
      ******************************************************************/
      if(to_player.find("all") != to_player.end())
      {
        //The vector of cards dealed to the client is saved into a vector
        //and is then uploaded to the client
        if(to_player["all"].get<std::string>() == "first bet")
        {
          if(to_player["drawn"].get<bool>())
          {
            std::vector<int> player_cards = to_player[PlayerID]["cards"].get<std::vector<int>>();
            win->update_cards(player_cards.at(0), player_cards.at(1), player_cards.at(2), player_cards.at(3), player_cards.at(4));
          }

          win->update_setting_bet(to_player["setting bet"].get<int>());
		   win->update_pot(to_player["pot"].get<int>());//updt
		   win->update_money(to_player["Money"].get<int>());//updt
		  
          if(to_player["turn"].get<std::string>() == PlayerID)
          {
            win->update_status("Your turn to CHECK, RAISE, or FOLD");
            win->update_sensitivity("bet", true);
          }
          else
          {
            win->update_status("Waiting for other player decision");
            win->update_sensitivity("bet", false);
          }
        }

        else if(to_player["all"].get<std::string>() == "exchange")
        { 
          if(to_player["turn"].get<std::string>() == PlayerID)
          {
            win->update_status("Choose the desires cards to be discard, then press exchange");
            win->update_sensitivity("exchange", true);
          }
          else
          {
            win->update_status("Waiting for other player to exchange cards");
            win->update_sensitivity("exchange", false);
          }
        }

        else if(to_player["all"].get<std::string>() == "final bet")
        {
          win->update_setting_bet(to_player["setting bet"].get<int>());
          if(to_player["turn"].get<std::string>() == PlayerID)
          {
            win->update_status("Your turn to CHECK, RAISE, or FOLD");
            win->update_sensitivity("bet", true);
          }
          else
          {
            win->update_status("Waiting for other player decision");
            win->update_sensitivity("bet", false);
          }
        }

        else if(to_player["all"].get<std::string>() == "reveal")
        {
          win->win_lose(to_player["winner"].get<int>());
        }

        if(to_player.find(PlayerID) != to_player.end())
        {
          if(to_player[PlayerID].find("cards") != to_player[PlayerID].end())
          {
            std::vector<int> player_cards = to_player[PlayerID]["cards"].get<std::vector<int>>();
            win->update_cards(player_cards.at(0), player_cards.at(1), player_cards.at(2), player_cards.at(3), player_cards.at(4));
          }         
        }
      }
	  gdk_threads_leave();//updt

      do_read_header();
    }
    else
    {
      socket_.close();
    }
  });
}

void chat_client::do_write()
{
  asio::async_write(socket_, asio::buffer(write_msgs_.front().data(), write_msgs_.front().length()), [this](std::error_code ec, std::size_t)
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

//////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[])
{
	gdk_threads_init();//updt
  try
  {
    if (argc != 3)
    {
      std::cerr << "Usage: chat_client <host> <port>\n";
      return 1;
    }

    asio::io_context io_context;

    tcp::resolver resolver(io_context);
    auto endpoints = resolver.resolve(argv[1], argv[2]);
    chat_client c(io_context, endpoints);

    std::thread t([&io_context](){ io_context.run(); });

    argc = 1;
    auto app = Gtk::Application::create(argc,argv, "");
    win = new Mainwin(&c);
    win->set_up("Player X", PlayerID);
    app->run(*win);

    c.close();
    t.join();
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}
