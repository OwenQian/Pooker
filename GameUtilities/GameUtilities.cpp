#include <vector>
#include <string>
#include <algorithm> 	//for deck shuffling
#include <random>		//for deck shuffling
#include <chrono>		//for deck shuffling seed

#include "GameUtilities.h"
#include "helper.h"

std::vector<int> deal(	std::vector<int>	&previousDeck,
						int					state) {
	enum states {HOLECARDS, PREFLOP, FLOP, TURN, RIVER};
	unsigned seed =	std::chrono::system_clock::now().time_since_epoch().count();
	std::vector<int> dealtCards;
	std::shuffle(previousDeck.begin(), previousDeck.end(), 
			std::default_random_engine(seed));
	if (state == HOLECARDS){
		for (int i = 0; i < 2; ++i){
			dealtCards.push_back(previousDeck.back());
			previousDeck.pop_back();
		}
	} else if (state == PREFLOP) {
		for (int i = 0; i < 3; ++i) {
			dealtCards.push_back(previousDeck.back());
			previousDeck.pop_back();
		}
	} else if (state != RIVER) {
		dealtCards.push_back(previousDeck.back());
		previousDeck.pop_back();
	}
	return dealtCards;
}

void init_deck(std::vector<int>& deck) {
	deck.reserve(52);
	const char ranks[] = {'2', '3', '4', '5', '6', '7', '8', '9', 'T', 'J', 'Q', 'K', 'A'};
	const char suits[] = {'s', 'h', 'd', 'c'};
	for (char i: ranks) {
		for (char j: suits) {
			std::string s {i, j};
			deck.push_back(cardToHex(s));
		}
	}
}

void playGame(){
	double bigBlind = 50.0;
	double smallBlind = 25.0;
	double initialChips = 1000.0;
	int smallBlindPosition = 0;
	
	Player botPlayer(0, 0, initialChips, 0); //0, 0 represetn dummy cards
	Player oppPlayer(0, 0, initialChips, 0);
	
	while (botPlayer.getChips() && oppPlayer.getChips()){ // while both players have chips
		std::vector<Player> updatePlayers = playRound (botPlayer, oppPlayer);
		botPlayer = updatePlayers[0];
		oppPlayer = updatePlayers[1];
	}
}