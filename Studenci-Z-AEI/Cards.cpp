#include "Cards.h"
#include <random>
#include <SFML/Graphics.hpp>
#include "Menu.h"
#include "Buildable.h"
#include "Logs.h"

bool freeBuildRoad = false;
bool freeBuildSettlement = false;
bool freeKnightMove = false;

std::string CardManager::buyCardWithMessage(Player& player) {
    if (player.getResourceCount(ResourceType::Kabel) < 1 ||
        player.getResourceCount(ResourceType::Piwo) < 1 ||
        player.getResourceCount(ResourceType::Notatki) < 1)
        return "";

    player.removeResource(ResourceType::Kabel, 1);
    player.removeResource(ResourceType::Piwo, 1);
    player.removeResource(ResourceType::Notatki, 1);

    std::vector<CardType> types = { CardType::FreeRoad, CardType::FreeSettlement, CardType::MoveRobber, CardType::VictoryPoint };
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, static_cast<int>(types.size()) - 1);
    CardType drawn = types[dist(gen)];

    std::unique_ptr<Card> card;
    switch (static_cast<int>(drawn)) {
    case static_cast<int>(CardType::FreeRoad): 
        card = std::make_unique<FreeRoadCard>(); 
        break;
    case static_cast<int>(CardType::FreeSettlement): 
        card = std::make_unique<FreeSettlementCard>(); 
        break;
    case static_cast<int>(CardType::MoveRobber): 
        card = std::make_unique<MoveRobberCard>(); 
        break;
    case static_cast<int>(CardType::VictoryPoint): 
        card = std::make_unique<VictoryPointCard>(); 
        break;
    }
    std::string cardName = card->getName();
    playerCards[player.getId()].push_back(std::move(card));
    return cardName;
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
    auto it = playerCards.find(player.getId());
    if (it == playerCards.end() || idx >= it->second.size()) {
        Logs logs(sf::Font(), 10);
        logs.add("Niepoprawny index karty.");
        return;
    }

    auto& card = it->second[idx];
    if (player.hasUsedCardThisTurn()) {
        Logs logs(sf::Font(), 10); 
        logs.add("Gracz użył już karty w tej turze.");
        return;
    }

    // Przekazuj flagi do use!
    card->use(player, buildables, board, knight, players, freeBuildRoad, freeBuildSettlement, knightMoveMode);

    // Jeśli knightMoveMode zostało ustawione, przygotuj przyciski
    if (knightMoveMode) {
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

    player.setUsedCardThisTurn(true);
    it->second.erase(it->second.begin() + idx);
}

void FreeRoadCard::use(
    Player&,
    std::vector<std::unique_ptr<Buildable>>&,
    Board&,
    Knight&,
    std::vector<Player>&,
    bool& freeBuildRoad,
    bool&,
    bool&
) {
    freeBuildRoad = true;
}

void FreeSettlementCard::use(
    Player&,
    std::vector<std::unique_ptr<Buildable>>&,
    Board&,
    Knight&,
    std::vector<Player>&,
    bool&,
    bool& freeBuildSettlement,
    bool&
) {
    freeBuildSettlement = true;
}

void MoveRobberCard::use(
    Player&,
    std::vector<std::unique_ptr<Buildable>>&,
    Board&,
    Knight&,
    std::vector<Player>&,
    bool&,
    bool&,
    bool& knightMoveMode
) {
    knightMoveMode = true;
}

void VictoryPointCard::use(Player& player, std::vector<std::unique_ptr<Buildable>>&, Board&, Knight&, std::vector<Player>&, bool&, bool&, bool&) {
    
}