
#include <algorithm>
#include <random>

#include "Deck.hpp"

/*
	A vector of 52 ints are made where the hundreds place
	represents its suit and the remainder represents its
	value. Ints between 1-13 represent Hearts, from 
	101-113 represents Diamonds, from 201-213 represents
	Clubs, and from 301-313 represents Spades. A value of
	1 represents an ace, from 2-10 represents their respective
	value, from 11-13 represent Jack, Queen, and King.
*/

Deck::Deck()
{
	deck_of_cards.clear();
	
	for (int i = 0; i < 4; i++)
	{
		for(int j = 1; j < 14; j++)
		{
			deck_of_cards.push_back(i * 100 + j);
		}
	}
}

Deck::~Deck()
{}

void Deck::shuffle()
{
	//auto rng = std::default_random_engine {};
	std::random_shuffle(std::begin(deck_of_cards), std::end(deck_of_cards));
}

int Deck::draw()
{
	int card_drawn = deck_of_cards.at(0);
	deck_of_cards.erase(deck_of_cards.begin());
	return card_drawn;
}
