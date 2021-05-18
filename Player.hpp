
#pragma once

#include <iostream>
#include <vector>

static int const MAX_HAND_SIZE = 5;

class Player
{
	public:
		Player();
		~Player();
		void set_player(std::string name, std::string UUID);
		std::string get_name();
		std::string get_UUID();
		int get_money();
		void set_money(int new_amount);
		void set_action(std::string action);
		std::string get_action();
		void player_draw(int card);
		void player_discard(int cardnum);
		int get_player_card(int position);
		std::vector<int> get_hand();
		int get_hand_size();
	private:
		std::string _name;
		std::string _UUID;
		int _money = 100;
		std::string _action;
		std::vector<int> _hand;
};
