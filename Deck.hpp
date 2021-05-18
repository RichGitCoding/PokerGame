
#pragma once

#include <vector>
#include <iostream>

static int const CARDS_IN_DECK = 52;

class Deck
{
	public:
		Deck();
		~Deck();
		void shuffle();
		int draw();
	private:
		std::vector<int> deck_of_cards;
};