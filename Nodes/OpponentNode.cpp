#include <memory>
#include <iostream>
#include <cassert>

#include "ChoiceNode.h"
#include "OpponentNode.h"
#include "../Config.h"
#include "../Action.h"
#include "../GameUtilities/Decision.h"
#include "../GameUtilities/GameUtilities.h"

OpponentNode::OpponentNode() :
    Node::Node() { }

OpponentNode::OpponentNode(
        Stage state,
        double pot,
        std::vector<int> boardCards,
        Player botPlayer,
        Player oppPlayer,
        int playerTurn,
        Node* const parent) :
    Node::Node(state,
            pot,
            boardCards,
            botPlayer,
            oppPlayer,
            playerTurn,
            parent) { }

OpponentNode::OpponentNode(const OpponentNode& obj) :
    Node(obj) { }

OpponentNode::OpponentNode(const Node& obj) :
    Node(obj) { }

void OpponentNode::fold() {
    foldChild.reset(new OpponentNode(*this));
    foldChild->setParent(this);
    //foldChild->getGame().setPlayerTurn(!foldChild->getGame().getPlayerTurn());
    foldChild->setIsFolded(true);
    foldChild->setVisitCount(0);
}

void OpponentNode::call() {
    Player tempPlayer = game.getOppPlayer();
    tempPlayer.setChips(tempPlayer.getChips() - (currentRaise - tempPlayer.getPotInvestment()) );
    tempPlayer.setPotInvestment(currentRaise);
    bool tempAllIn = false;
		if (game.getBotPlayer().getChips() + game.getBotPlayer().getPotInvestment() <= currentRaise ||
				game.getOppPlayer().getChips() + game.getOppPlayer().getPotInvestment() <= currentRaise) {
			tempAllIn = true;
		}
        int tempTurn = !game.getPlayerTurn();
        if (!getIsFirst()) {
            tempTurn = smallBlindPosition;
        }
        if (getIsFirst() || (smallBlindPosition == 0)) {
        callChild.reset( new ChoiceNode(game.getState() + !getIsFirst(),
                    initialChips * 2 - tempPlayer.getChips() - game.getBotPlayer().getChips(),
                    game.getBoardCards(),
                    game.getBotPlayer(),
                    tempPlayer,
                    tempTurn,
                    this) );
        } else {
             callChild.reset( new OpponentNode(game.getState() + !getIsFirst(),
                    initialChips * 2 - tempPlayer.getChips() - game.getBotPlayer().getChips(),
                    game.getBoardCards(),
                    game.getBotPlayer(),
                    tempPlayer,
                    tempTurn,
                    this) );
        }
        callChild->setIsAllIn(tempAllIn);
        callChild->setCurrentRaise(firstAction * currentRaise);
        callChild->getGame().getBotPlayer().setPotInvestment( firstAction * callChild->getGame().getBotPlayer().getPotInvestment());
        callChild->getGame().getOppPlayer().setPotInvestment( firstAction * callChild->getGame().getOppPlayer().getPotInvestment());
        callChild->setIsFirst(!firstAction);
}

void OpponentNode::raise(double raiseAmount) {
    if (raiseAmount < bigBlind || raiseAmount < 2*currentRaise) {
        raiseAmount = bigBlind > (2*currentRaise) ? bigBlind : (2*currentRaise);
    } 
	// if raise all-in (or more) create AllInNode, handled by call
	if (game.getBotPlayer().getChips() + game.getBotPlayer().getPotInvestment() <= currentRaise ||
				game.getOppPlayer().getChips() + game.getOppPlayer().getPotInvestment() <= currentRaise) {
        call();
        if (callChild->getGame().getPlayerTurn() == 0) {
            raiseChild.reset(new ChoiceNode(*static_cast<ChoiceNode*>(callChild.get())));
        } else {
            raiseChild.reset(new OpponentNode(*static_cast<OpponentNode*>(callChild.get())));
        }
        return;
		}
	if (raiseAmount >= game.getBotPlayer().getChips() + game.getBotPlayer().getPotInvestment() ||
			raiseAmount >= game.getOppPlayer().getChips() + game.getOppPlayer().getPotInvestment() ) {

		// set raiseAmount to lesser of chip amounts
		raiseAmount = std::min(game.getBotPlayer().getChips() + game.getBotPlayer().getPotInvestment(),
				game.getOppPlayer().getChips() + game.getOppPlayer().getPotInvestment());
	}
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
}

Decision OpponentNode::makeDecision(std::vector<int> deck) {
    std::cout << "Enter Action opp: Call(0), Raise(1), Fold(2)" << std::endl;
    Decision decision;
    int temp;
    std::cin >> temp;
    decision.action = static_cast<Action>(temp);
    std::cout << "Opp Decision: " << static_cast<int>(decision.action) << std::endl;
    if (decision.action == Action::RAISE) {
        if (game.getBotPlayer().getChips() + game.getBotPlayer().getPotInvestment() <= currentRaise ||
                game.getOppPlayer().getChips() + game.getOppPlayer().getPotInvestment() <= currentRaise) {
            decision.raiseAmount = 1;
        } else {
            std::cout << "Enter Raise amount" << std::endl;
            double amount;
            std::cin >> amount;
            decision.raiseAmount = amount;
        }
    }
    return decision;
}
