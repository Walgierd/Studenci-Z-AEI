#include "Cards.h"
#include <random>
#include <SFML/Graphics.hpp>
#include "Menu.h"

bool freeBuildRoad = false;
bool freeBuildSettlement = false;
bool freeKnightMove = false;

void CardManager::buyCard(Player& player) {
    // Koszt: 1 kawa, 1 piwo, 1 notatki
    if (player.getResourceCount(ResourceType::Kabel) < 1 ||
        player.getResourceCount(ResourceType::Piwo) < 1 ||
        player.getResourceCount(ResourceType::Notatki) < 1)
        return;

    player.removeResource(ResourceType::Kabel, 1);
    player.removeResource(ResourceType::Piwo, 1);
    player.removeResource(ResourceType::Notatki, 1);


    std::vector<CardType> types = { CardType::FreeRoad, CardType::FreeSettlement, CardType::MoveRobber, CardType::VictoryPoint };
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, static_cast<int>(types.size()) - 1);
    CardType drawn = types[dist(gen)];

    std::unique_ptr<Card> card;
    switch (drawn) {
    case CardType::FreeRoad: card = std::make_unique<FreeRoadCard>(); break;
    case CardType::FreeSettlement: card = std::make_unique<FreeSettlementCard>(); break;
    case CardType::MoveRobber: card = std::make_unique<MoveRobberCard>(); break;
    case CardType::VictoryPoint: card = std::make_unique<VictoryPointCard>(); break;
    }
    playerCards[player.getId()].push_back(std::move(card));
}

void CardManager::showCards(sf::RenderWindow& window, const sf::Font& font, Player& player) {
    if (!cardsVisible) return;
    const auto& cards = playerCards[player.getId()];
    float y = 200.f;
    for (size_t i = 0; i < cards.size(); ++i) {
        sf::Text text;
        text.setFont(font);
        text.setString(std::to_string(i + 1) + ". " + cards[i]->getName());
        text.setCharacterSize(28);
        text.setFillColor(sf::Color::Yellow);
        text.setPosition(400.f, y);
        window.draw(text);
        y += 40.f;
    }

}

void CardManager::useCard(
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
) {
    auto& cards = playerCards[player.getId()];
    if (idx >= cards.size()) return;
    CardType type = cards[idx]->type;

    if (type == CardType::FreeSettlement) {
        buildMode = BuildMode::Settlement;
        buildButtons.clear();
        freeBuildSettlement = true; 
        initializeBuildButtons(buildButtons, buildables, board, hexSize, buildMode, players, currentPlayer, window);
    }
    else if (type == CardType::FreeRoad) {
        buildMode = BuildMode::Road;
        buildButtons.clear();
        freeBuildRoad = true;
        initializeBuildButtons(buildButtons, buildables, board, hexSize, buildMode, players, currentPlayer, window);
    }
    else if (type == CardType::MoveRobber) {
        knightMoveMode = true;
        knightMoveButtons.clear();
        const auto& tiles = board.getTiles();
        for (size_t i = 0; i < tiles.size(); ++i) {
            if (static_cast<int>(i) == knight.tileIndex) continue;
            knightMoveButtons.push_back(std::make_unique<KnightMoveButton>(
                tiles[i].getPosition(), static_cast<int>(i),
                [&](int idx) {
                    knight.setPosition(idx);
                    knightMoveMode = false;
                    knightMoveButtons.clear();
                }
            ));
        }
    }

    cards.erase(cards.begin() + idx);
}


void FreeRoadCard::use(Player&, std::vector<std::unique_ptr<Buildable>>&, Board&, Knight&, std::vector<Player>&) {
    extern bool freeBuildRoad;
    freeBuildRoad = true;
}

void FreeSettlementCard::use(Player&, std::vector<std::unique_ptr<Buildable>>&, Board&, Knight&, std::vector<Player>&) {
    extern bool freeBuildSettlement;
    freeBuildSettlement = true;
}

void MoveRobberCard::use(Player&, std::vector<std::unique_ptr<Buildable>>&, Board&, Knight&, std::vector<Player>&) {
    extern bool freeKnightMove;
    freeKnightMove = true;
}

void VictoryPointCard::use(Player& player, std::vector<std::unique_ptr<Buildable>>&, Board&, Knight&, std::vector<Player>&) {

}