
#include "Player.hpp"

Player::Player()
{}

Player::~Player()
{}

void Player::set_player(std::string name, std::string UUID)
{
	_name = name;
	_UUID = UUID;
	_money = 100;
}

std::string Player::get_name()
{
	return _name;
}

std::string Player::get_UUID()
{
	return _UUID;
}

int Player::get_money()
{
	return _money;
}

void Player::set_money(int new_amount)
{
	_money = new_amount;
}

void Player::set_action(std::string action)
{
	_action = action;
}

std::string Player::get_action()
{
	return _action;
}

void Player::player_draw(int card)
{
	_hand.push_back(card);
}

void Player::player_discard(int cardnum)
{
	_hand.clear();
}

int Player::get_player_card(int position)
{
	return _hand.at(position);
}

std::vector<int> Player::get_hand()
{
	return _hand;
}

int Player::get_hand_size()
{
	return _hand.size();
}
