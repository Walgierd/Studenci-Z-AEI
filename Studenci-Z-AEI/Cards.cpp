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
    int visibleIdx = 1;
    for (size_t i = 0; i < cards.size(); ++i) {
        if (cards[i]->type == CardType::VictoryPoint) continue; 
        sf::Text text;
        text.setFont(font);
        text.setString(std::to_string(visibleIdx++) + ". " + cards[i]->getName());
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


    if (card->type == CardType::FreeRoad) {
        std::vector<sf::Vector2f> hexCenters;
        for (const auto& tile : board.getTiles())
            hexCenters.push_back(tile.getPosition());
        auto roadSpots = getUniqueHexEdges(hexCenters, hexSize);
        bool canBuild = false;
        for (const auto& edge : roadSpots) {
            bool occupied = false;
            for (const auto& b : buildables) {
                if (auto* r = dynamic_cast<Road*>(b.get())) {
                    bool same = (std::hypot(r->start.x - edge.first.x, r->start.y - edge.first.y) < 1.0f &&
                                 std::hypot(r->end.x - edge.second.x, r->end.y - edge.second.y) < 1.0f);
                    bool reverse = (std::hypot(r->start.x - edge.second.x, r->start.y - edge.second.y) < 1.0f &&
                                    std::hypot(r->end.x - edge.first.x, r->end.y - edge.first.y) < 1.0f);
                    if (same || reverse) {
                        occupied = true;
                        break;
                    }
                }
            }
            if (!occupied && isRoadConnected(buildables, edge.first, edge.second, player.getId())) {
                canBuild = true;
                break;
            }
        }
        if (!canBuild) {
            playerCards[player.getId()].push_back(std::make_unique<FreeRoadCard>());
            Logs logs(sf::Font(), 10);
            logs.add("Nie możesz zbudować żadnej drogi. Karta wraca do talii.");
            it->second.erase(it->second.begin() + idx);
            return;
        }
    }
    if (card->type == CardType::FreeSettlement) {
        std::vector<sf::Vector2f> hexCenters;
        for (const auto& tile : board.getTiles())
            hexCenters.push_back(tile.getPosition());
        auto settlementSpots = getUniqueHexVertices(hexCenters, hexSize);
        bool canBuild = false;
        for (const auto& pos : settlementSpots) {
            bool occupied = false;
            for (const auto& b : buildables) {
                if (auto* s = dynamic_cast<Settlement*>(b.get())) {
                    if (std::hypot(s->pos.x - pos.x, s->pos.y - pos.y) < 1.0f) {
                        occupied = true;
                        break;
                    }
                }
            }
            if (!occupied && isSettlementFarEnough(buildables, pos, hexSize * std::sqrt(3.f) - 5)) {
                int playerSettlements = 0;
                for (const auto& b : buildables) {
                    if (auto* s = dynamic_cast<Settlement*>(b.get())) {
                        if (s->ownerId == player.getId()) playerSettlements++;
                    }
                }
                bool connected = true;
                if (playerSettlements >= 2) {
                    std::vector<sf::Vector2f> playerSettlementPositions;
                    for (const auto& b : buildables) {
                        if (auto* s = dynamic_cast<Settlement*>(b.get())) {
                            if (s->ownerId == player.getId()) playerSettlementPositions.push_back(s->pos);
                        }
                    }
                    connected = false;
                    for (const auto& prevPos : playerSettlementPositions) {
                        if (areSettlementsConnected(buildables, player.getId(), prevPos, pos)) {
                            connected = true;
                            break;
                        }
                    }
                }
                if (connected) {
                    canBuild = true;
                    break;
                }
            }
        }
        if (!canBuild) {
            // Zwróć kartę do decku
            playerCards[player.getId()].push_back(std::make_unique<FreeSettlementCard>());
            Logs logs(sf::Font(), 10);
            logs.add("Nie możesz zbudować osiedla. Karta wraca do talii.");
            it->second.erase(it->second.begin() + idx);
            return;
        }
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
    if (card->type != CardType::VictoryPoint) {
        it->second.erase(it->second.begin() + idx);
    }
}

int CardManager::getVictoryPointCardCount(int playerId) const {
    int count = 0;
    auto it = playerCards.find(playerId);
    if (it != playerCards.end()) {
        for (const auto& card : it->second) {
            if (card->type == CardType::VictoryPoint)
                ++count;
        }
    }
    return count;
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

void VictoryPointCard::use(Player&, std::vector<std::unique_ptr<Buildable>>&, Board&, Knight&, std::vector<Player>&, bool&, bool&, bool&) {
    // Nie usuwaj tej karty z playerCards!
    // Punkty są liczone na podstawie liczby kart VictoryPoint w playerCards
}