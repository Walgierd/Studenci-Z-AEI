#pragma once
#include <vector>
#include <memory>
#include <map>
#include <SFML/Graphics.hpp>
#include "Player.h"
#include "Buildable.h"
#include "Knight.h"
#include "Board.h"
#include "SimpleButton.h"
#include "UIButton.h"

enum class CardType {
    FreeRoad,
    FreeSettlement,
    MoveRobber,
    VictoryPoint
};

struct Card {
    CardType type;
    Card(CardType t) : type(t) {}
    virtual ~Card() = default;
    virtual void use(
        Player& player,
        std::vector<std::unique_ptr<Buildable>>& buildables,
        Board& board,
        Knight& knight,
        std::vector<Player>& players,
        bool& freeBuildRoad,
        bool& freeBuildSettlement,
        bool& knightMoveMode
    ) = 0;
    virtual std::string getName() const = 0;
};

struct FreeRoadCard : public Card {
    FreeRoadCard() : Card(CardType::FreeRoad) {}
    void use(
        Player& player,
        std::vector<std::unique_ptr<Buildable>>& buildables,
        Board& board,
        Knight& knight,
        std::vector<Player>& players,
        bool& freeBuildRoad,
        bool& freeBuildSettlement,
        bool& knightMoveMode
    ) override;
    std::string getName() const override { return "Darmowa droga"; }
};

struct FreeSettlementCard : public Card {
    FreeSettlementCard() : Card(CardType::FreeSettlement) {}
    void use(
        Player& player,
        std::vector<std::unique_ptr<Buildable>>& buildables,
        Board& board,
        Knight& knight,
        std::vector<Player>& players,
        bool& freeBuildRoad,
        bool& freeBuildSettlement,
        bool& knightMoveMode
    ) override;
    std::string getName() const override { return "Darmowe osiedle"; }
};

struct MoveRobberCard : public Card {
    MoveRobberCard() : Card(CardType::MoveRobber) {}
    void use(
        Player& player,
        std::vector<std::unique_ptr<Buildable>>& buildables,
        Board& board,
        Knight& knight,
        std::vector<Player>& players,
        bool& freeBuildRoad,
        bool& freeBuildSettlement,
        bool& knightMoveMode
    ) override;
    std::string getName() const override { return "Przenieś złodzieja"; }
};

struct VictoryPointCard : public Card {
    VictoryPointCard() : Card(CardType::VictoryPoint) {}
    void use(
        Player& player,
        std::vector<std::unique_ptr<Buildable>>& buildables,
        Board& board,
        Knight& knight,
        std::vector<Player>& players,
        bool& freeBuildRoad,
        bool& freeBuildSettlement,
        bool& knightMoveMode
    ) override;
    std::string getName() const override { return "Punkt zwycięstwa"; }
};

class CardManager {
public:
    std::string buyCardWithMessage(Player& player); 

    void showCards(sf::RenderWindow& window, const sf::Font& font, Player& player);
    void useCard(
        size_t idx,
        Player& player,
        std::vector<std::unique_ptr<Buildable>>& buildables,
        Board& board,
        Knight& knight,
        std::vector<Player>& players,
        BuildMode& buildMode,
        std::vector<std::unique_ptr<BuildSpotButton>>& buildButtons,
        sf::RenderWindow& window,
        float hexSize,
        int currentPlayer,
        bool& freeBuildRoad,
        bool& freeBuildSettlement, 
        bool& knightMoveMode,
        std::vector<std::unique_ptr<BuildSpotButton>>& knightMoveButtons
    );

    const std::map<int, std::vector<std::unique_ptr<Card>>>& getPlayerCards() const {
        return playerCards;
    }

    std::unique_ptr<SimpleButton> buyCardButton;
    std::unique_ptr<SimpleButton> showCardsButton;

    bool cardsVisible = false;

private:
    std::map<int, std::vector<std::unique_ptr<Card>>> playerCards; 
};

