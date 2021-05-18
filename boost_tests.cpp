#include <boost/test/unit_test.hpp>
#define BOOST_TEST_MODULE pokertests
#include <vector>
#include <algorithm>

#include "Deck.hpp"
#include "GTK.h"


BOOST_AUTO_TEST_SUITE(game_tests)

BOOST_AUTO_TEST_CASE(shuffle_test)
{
	Deck preshuf;
	std::vector<int> postshuf = preshuf.shuffle();
	BOOST_CHECK(preshuf != postshuf);
}

BOOST_AUTO_TEST_CASE(draw_test)
{
	Deck deck;
	deck.draw();
	BOOST_CHECK(deck.size < 52);
}

BOOST_AUTO_TEST_CASE(find_test)
{
	int card = 312; //Queen of spades
	std::string found2 = "QS.png";
	std::string found = find_pic(card);
	BOOST_CHECK_EQUAL(found, found2);
}

BOOST_AUTO_TEST_CASE(deck_test)
{
	int expect = 52;
	Deck deck;
	BOOST_CHECK_EQUAL(deck.deck_of_cards.size(), expect);
}

BOOST_AUTO_TEST_CASE(dupe_test)
{
	Deck deck;
	sort(deck.deck_of_cards.begin(), deck.deck_of_cards.end());
	auto it = std::unique(deck.deck_of_cards.begin(), deck.deck_of_cards.end());
	BOOST_CHECK_EQUAL(it, deck.deck_of_cards.end());
}

BOOST_AUTO_TEST_SUITE_END()