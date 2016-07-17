#ifndef PLAYER_H
#define PLAYER_H

#include <vector>

class Player {
	private:
		std::vector<int> holeCards;
		double chips;
		double potInvestment;


	public:
		// member accessibility fcns
		Player();
		Player(std::vector<int> hcards, double c, double pi);
		std::vector<int> getHoleCards();
		double getChips();
		double getPotInvestment();
		void setChips();
		void setPotInvestment();

		Player(){
			chips = 0;
			potInvestment = 0;
		}

		Player(std::vector<int> hcards, double c, double pi){
			holeCards = hcards;
			chips = c;
			potInvestment = pi;
		}

		std::vector<int> getHoleCards() {
			return holeCards;
		}

		double getChips() {
			return chips;
		}

		double getPotInvestment() {
			return potInvestment;
		}

		void setChips (double chip){
			chips = chip;
		}

		void setPotInvestment(double pi){
			potInvestment = pi;
		}
};

#endif