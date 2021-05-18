//
// chat_server.cpp
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2018 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <cstdlib>
#include <deque>
#include <iostream>
#include <list>
#include <memory>
#include <set>
#include <utility>
#include <thread>
#include <chrono>
#include "asio.hpp"
#include "chat_message.hpp"
#include "Player.cpp"
#include "Deck.cpp"

#include "json.hpp"

using asio::ip::tcp;

static int const MAX_STARTING_PLAYERS = 2;

Deck deck_;
std::vector<std::string> all_id;
std::vector<std::string> waitlist;
std::string game_state;
int turn;
int setting_bet;
int Money = 100;//updt
int total_pot;//updt
std::vector<std::vector<int>> final_cards;

//----------------------------------------------------------------------

typedef std::deque<chat_message> chat_message_queue;

//----------------------------------------------------------------------

class poker_participant
{
public:
  virtual ~poker_participant() {}
  virtual void deliver(const chat_message& msg) = 0;
};

typedef std::shared_ptr<poker_participant> poker_participant_ptr;

//----------------------------------------------------------------------

class chat_room
{
public:
  void join(poker_participant_ptr participant)
  {
    participants_.insert(participant);
    for (auto msg: recent_msgs_)
      participant->deliver(msg);
  }

  void leave(poker_participant_ptr participant)
  {
    participants_.erase(participant);
  }

  void deliver(const chat_message& msg)
  {
    recent_msgs_.push_back(msg);
    while (recent_msgs_.size() > max_recent_msgs)
      recent_msgs_.pop_front();

    for (auto participant: participants_)
      participant->deliver(msg);
  }

  int size()
  {
    return participants_.size();
  }

private:
  std::set<poker_participant_ptr> participants_;
  enum { max_recent_msgs = 100 };
  chat_message_queue recent_msgs_;
};

//----------------------------------------------------------------------

class poker_session : public poker_participant, public std::enable_shared_from_this<poker_session>
{
public:
  poker_session(tcp::socket socket, chat_room& room) : socket_(std::move(socket)), room_(room)
  {
    game_state = "new game";
    total_pot = 0;
  }

  void start()
  {
    room_.join(shared_from_this());

    std::cout << "Number of players: " << room_.size() << std::endl;
    do_read_header();
  }

  void deliver(const chat_message& msg)
  {
    bool write_in_progress = !write_msgs_.empty();
    write_msgs_.push_back(msg);
    if (!write_in_progress)
    {
      do_write();
    }
  }

  int find_winner(std::vector<std::vector<int>> final_cards)
  {
    return final_cards.at(0).at(0);
  }

private:
  void do_read_header()
  {
    auto self(shared_from_this());
    asio::async_read(socket_, asio::buffer(read_msg_.data(), chat_message::header_length), [this, self](std::error_code ec, std::size_t /*length*/)
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
        room_.leave(shared_from_this());
      }
    });
  }

  void do_read_body()
  {
    auto self(shared_from_this());
    asio::async_read(socket_, asio::buffer(read_msg_.body(), read_msg_.body_length()), [this, self](std::error_code ec, std::size_t /*length*/)
    {
      if (!ec)
      {
        nlohmann::json to_dealer = nlohmann::json::parse(std::string(read_msg_.body()));
        nlohmann::json to_player;
        std::string PlayerID = to_dealer["uuid"].get<std::string>();
        
        /******************************************************************
          NEW GAME

          This is the first game state where set-up takes place. Once
          enough players join, cards will be dealt and the next phase 
          will begin.
        ******************************************************************/
        if(game_state == "new game")
        {
          /*
            Upon a user pressing "JOIN", the size of the room is checked.
            If there is a spot for the player to join then a JSON object 
            is made for them with a Player ID made from their uuid.
          */
          if(room_.size() <= MAX_STARTING_PLAYERS && to_dealer["action"].get<std::string>() == "join")
          {
            to_player[PlayerID]["phase"] = "joined";
            all_id.push_back(PlayerID);
          }

          /*
            Now that enough players have joined, the deck will be shuffled
            and the game will begin to deal cards
          */
          if(all_id.size() >= MAX_STARTING_PLAYERS)
          {
            game_state = "deal cards";
            deck_.shuffle();
          }
        }
        else
        {
          if(room_.size() <= 5 && to_dealer["action"].get<std::string>() == "join")
          {
            to_player[PlayerID]["phase"] = "waitlist";
            waitlist.push_back(PlayerID);
          }
        }

        if(game_state == "new round")
        {
          if(all_id.size() >= MAX_STARTING_PLAYERS)
          {
            game_state = "deal cards";
            deck_.shuffle();
          }
        }

        /******************************************************************
          DEAL CARDS

          Once 5 players have joined the poker game will begin with the 
          5 cards being dealt to all players and recorded in the JSON
        ******************************************************************/
        if(to_dealer.find("cards") == to_dealer.end())
        {
          if(game_state == "deal cards")
          {
            std::cout << "Dealing cards" << std::endl;
            //A loop is used to go through and deal each player 5 cards
            num_players = all_id.size();
            for (int i = 0; i < num_players; i++)
            {
              to_player[all_id.at(i)]["cards"] = {deck_.draw(), deck_.draw(), deck_.draw(), deck_.draw(), deck_.draw()};
            }

            //Once all players have 5 cards, the betting phase begins and values are initialized
            game_state = "first bet";
            to_player["all"] = game_state;
            to_player["drawn"] = true;
            turn = 0;
            setting_bet = 5;
            total_pot = 0;//updt
			Money = 100;//updt
          }
        }

        /******************************************************************
          This is the first betting phase where players choose to bet,
          pass, fold, etc. After this is complete, the game will progress
          according to the player's action
        ******************************************************************/
        if(game_state == "first bet")
        {
          std::cout << "First bet" << std::endl;
          //The turn is assigned to the first player that joined and then their action will be recorded
          to_player["turn"] = all_id.at(turn);
          to_player["setting bet"] = setting_bet;
		  to_player["pot"] = total_pot;//updt
		  to_player["Money"]= Money;//updt

          if(PlayerID == all_id.at(turn))
          {
            to_player["drawn"] = false;
            //If the player chose to bet the setting bet for the next player is
            //set to match the previous player
            if(to_dealer["action"].get<std::string>() == "bet")
            {
              to_player[all_id.at(turn)]["ingame"] = true;
              setting_bet = to_dealer["bet"].get<int>();
              total_pot += to_dealer["bet"].get<int>();//updt
            }
            else if(to_dealer["action"].get<std::string>() == "fold")
            {
              to_player[all_id.at(turn)]["ingame"] = false;
              all_id.erase(all_id.begin() + turn);
              waitlist.push_back(all_id.at(turn));
            }
            else if(to_dealer["action"].get<std::string>() == "check")
            {
              to_player[all_id.at(turn)]["ingame"] = true;
              total_pot += to_dealer["bet"].get<int>();
            }

            to_player["setting bet"] = setting_bet;
            to_player["pot"] = total_pot;//updt

            turn++;
            num_players = all_id.size();
            if(turn != num_players)
            {
              to_player["turn"] = all_id.at(turn);
              to_player["all"] = game_state;
            }
            else
            {
              game_state = "exchange";
              to_player["all"] = game_state;
              turn = 0;
            }
          }
        }

        if(game_state == "exchange")
        {
          std::cout << "Exchanging" << std::endl;

          to_player["turn"] = all_id.at(turn);

          if(to_dealer["action"].get<std::string>() == "exchange")
          {
            std::vector<int> new_cards = to_dealer["cards"].get<std::vector<int>>();
            std::cout << to_dealer["cards"] << std::endl;
            if(new_cards.size() < 5)
            {
              int draws = 5 - new_cards.size();
              for (int i = 0; i < draws; i++)
              {
                new_cards.push_back(deck_.draw());
              }
            }
            to_player[PlayerID]["cards"] = new_cards;
            turn++;
            num_players = all_id.size();
            if(turn != num_players)
            {
              to_player["turn"] = all_id.at(turn);
              to_player["all"] = game_state;
            }
            else
            {
              game_state = "final bet";
              to_player["all"] = game_state;
              turn = 0;
            }
          }
		  
		  
		  
        }

        if(game_state == "final bet")
        {
          std::cout << "Final bet" << std::endl;

          //The turn is assigned to the first player that joined and then their action will be recorded
          to_player["turn"] = all_id.at(turn);
          to_player["setting bet"] = setting_bet;
		      to_player["pot"] = total_pot;//updt

          if(PlayerID == all_id.at(turn))
          {
            //If the player chose to bet the setting bet for the next player is
            //set to match the previous player
            if(to_dealer["action"].get<std::string>() == "bet")
            {
              to_player[all_id.at(turn)]["ingame"] = true;
              setting_bet = to_dealer["bet"].get<int>();
              total_pot += to_dealer["bet"].get<int>();//updt
              final_cards.push_back(to_dealer["cards"].get<std::vector<int>>());
            }
            else if(to_dealer["action"].get<std::string>() == "fold")
            {
              to_player[all_id.at(turn)]["ingame"] = false;
              all_id.erase(all_id.begin() + turn);
              waitlist.push_back(all_id.at(turn));
            }
            else if(to_dealer["action"].get<std::string>() == "check")
            {
              to_player[all_id.at(turn)]["ingame"] = true;
              total_pot += to_dealer["bet"].get<int>();
              final_cards.push_back(to_dealer["cards"].get<std::vector<int>>());
            }

            to_player["setting bet"] = setting_bet;
            to_player["pot"] = total_pot;//updt

            turn++;
            num_players = all_id.size();
            if(turn != num_players)
            {
              to_player["turn"] = all_id.at(turn);
              to_player["all"] = game_state;
            }
            else
            {
              game_state = "reveal";
              to_player["all"] = game_state;
              turn = 0;
            }
          }
        }

        if(game_state == "reveal")
        {
          std::cout << "Reveal" << std::endl;
          to_player["winner"] = find_winner(final_cards);

          game_state = "new game";
          //all_id.insert(all_id.end(), waitlist.begin(), waitlist.end());
          all_id.clear();
          final_cards.clear();
          deck_ = Deck();
        }

        std::string t = to_player.dump();
        chat_message sending;
        if (t.size() < chat_message::max_body_length)
        {
          std::cout << t << std::endl;
          memcpy( sending.body(), t.c_str(), t.size() );
          sending.body_length(t.size());
          sending.encode_header();
          room_.deliver(sending);
        }
        do_read_header();
      }
      else
      {
        room_.leave(shared_from_this());
      }
    });
  }

  void do_write()
  {
    auto self(shared_from_this());
    asio::async_write(socket_, asio::buffer(write_msgs_.front().data(), write_msgs_.front().length()), [this, self](std::error_code ec, std::size_t)
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
        room_.leave(shared_from_this());
      }
    });
  }

  tcp::socket socket_;
  chat_room& room_;
  chat_message read_msg_;
  chat_message_queue write_msgs_;

  int num_players;
};

//----------------------------------------------------------------------

class chat_server
{
public:
  chat_server(asio::io_context& io_context,
      const tcp::endpoint& endpoint)
    : acceptor_(io_context, endpoint)
  {
    do_accept();
  }

private:
  void do_accept()
  {
    acceptor_.async_accept(
        [this](std::error_code ec, tcp::socket socket)
        {
          if (!ec)
          {
            std::make_shared<poker_session>(std::move(socket), room_)->start();
          }

          do_accept();
        });
  }

  tcp::acceptor acceptor_;
  chat_room room_;
};

//----------------------------------------------------------------------

int main(int argc, char* argv[])
{
  try
  {
    if (argc < 2)
    {
      std::cerr << "Usage: chat_server <port> [<port> ...]\n";
      return 1;
    }

    asio::io_context io_context;
    std::list<chat_server> servers;

    std::cout << "Poker++ server has initialized." << std::endl;
    for (int i = 1; i < argc; ++i)
    {
      tcp::endpoint endpoint(tcp::v4(), std::atoi(argv[i]));
      servers.emplace_back(io_context, endpoint);
    }

    io_context.run();
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}
