#include <memory>
#include <iostream>
#include <cassert>

#include "ChoiceNode.h"
#include "OpponentNode.h"
#include "../Stage.h"
#include "../Config.h"
#include "../handEval/helper.h"
#include "../GameUtilities/GameUtilities.h"

OpponentNode::OpponentNode() :
    Node() { }

OpponentNode::OpponentNode(int state,
        double pot,
        std::vector<int> boardCards,
        Player botPlayer,
        Player oppPlayer,
        int playerTurn,
        Node* const parent) :
    Node(state,
            pot,
            boardCards,
            botPlayer,
            oppPlayer,
            playerTurn,
            parent) { }

OpponentNode::OpponentNode(const OpponentNode& obj) :
    Node(obj) { }

std::unique_ptr<Node>& OpponentNode::call() {
    Player tempPlayer = game.getOppPlayer();
    tempPlayer.setChips(tempPlayer.getChips() - (currentRaise - tempPlayer.getPotInvestment()) );
    tempPlayer.setPotInvestment(currentRaise);
    bool tempAllIn = false;
		if (game.getBotPlayer().getChips() + game.getBotPlayer().getPotInvestment() <= currentRaise ||
				game.getOppPlayer().getChips() + game.getOppPlayer().getPotInvestment() <= currentRaise) {
			tempAllIn = true;
		}
        if (getIsFirst() || smallBlindPosition == 1) {
        callChild.reset( new OpponentNode(game.getState() + !getIsFirst(),
                    initialChips * 2 - tempPlayer.getChips() - game.getBotPlayer().getChips(),
                    game.getBoardCards(),
                    game.getBotPlayer(),
                    tempPlayer,
                    !(game.getPlayerTurn()),
                    this) );
        } else {
             callChild.reset( new ChoiceNode(game.getState() + !getIsFirst(),
                    initialChips * 2 - tempPlayer.getChips() - game.getBotPlayer().getChips(),
                    game.getBoardCards(),
                    game.getBotPlayer(),
                    tempPlayer,
                    !(game.getPlayerTurn()),
                    this) );
        }
        callChild->setIsAllIn(tempAllIn);
        callChild->setCurrentRaise(firstAction * currentRaise);
        callChild->getGame().getBotPlayer().setPotInvestment( firstAction * callChild->getGame().getBotPlayer().getPotInvestment());
        callChild->getGame().getOppPlayer().setPotInvestment( firstAction * callChild->getGame().getOppPlayer().getPotInvestment());
        callChild->setIsFirst(false);
        return callChild;
}

std::unique_ptr<Node>& OpponentNode::fold(){
	return Node::fold();
}

std::unique_ptr<Node>& OpponentNode::raise(double raiseAmount) {
    if (raiseAmount < bigBlind || raiseAmount < 2*currentRaise) {
        raiseAmount = bigBlind > (2*currentRaise) ? bigBlind : (2*currentRaise);
    } 
	// if raise all-in (or more) create AllInNode, handled by call
	if (game.getBotPlayer().getChips() <= currentRaise ||
				game.getOppPlayer().getChips() <= currentRaise) {
			return call();
		}
	if (raiseAmount >= game.getBotPlayer().getChips() + game.getBotPlayer().getPotInvestment() ||
			raiseAmount >= game.getOppPlayer().getChips() + game.getOppPlayer().getPotInvestment() ) {
		std::cout << "Raising All-In" << std::endl;

		// set raiseAmount to lesser of chip amounts
		raiseAmount = std::min(game.getBotPlayer().getChips() + game.getBotPlayer().getPotInvestment(),
				game.getOppPlayer().getChips() + game.getOppPlayer().getPotInvestment());
	}
	setIsFirst(false);
		Player tempPlayer = game.getOppPlayer();
		tempPlayer.setChips(tempPlayer.getChips() - (raiseAmount - tempPlayer.getPotInvestment()) );
		tempPlayer.setPotInvestment(raiseAmount);
		raiseChild.reset(new ChoiceNode( game.getState(),
			initialChips * 2 - tempPlayer.getChips() - game.getBotPlayer().getChips(),
			game.getBoardCards(),
			game.getBotPlayer(),
			tempPlayer,
			!(game.getPlayerTurn()),
			this ));
		raiseChild->setCurrentRaise(raiseAmount);
	
	return raiseChild;
}

