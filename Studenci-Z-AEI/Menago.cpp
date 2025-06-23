#include "Menago.h"
#include <ranges>

//tury usunięte i przenisione do turnmanager w game bo łatwiej się pisało


std::map<int, std::map<ResourceType, int>> handleDiceRollWithLog(
    std::vector<Player>& players,
    int currentPlayer,
    Board& board,
    std::vector<std::unique_ptr<Buildable>>& buildables,
    Knight& knight,
    float hexSize
) {
    std::map<int, std::map<ResourceType, int>> received;
    if (players.empty() || players[currentPlayer].hasRolled())
        return received;

    players[currentPlayer].rollDice();
    int diceSum = players[currentPlayer].getDice1() + players[currentPlayer].getDice2();

    const auto& tiles = board.getTiles();

    int idx = 0;
    for (const auto& tile : tiles) {
        if (tile.getNumber() == diceSum && !knight.blocksTile(idx)) {//filtrowanie z ranges
            auto settlements = buildables
                | std::views::filter([](const auto& b) { return dynamic_cast<Settlement*>(b.get()) != nullptr; });

            for (const auto& b : settlements) {
                auto* s = static_cast<Settlement*>(b.get());
                if (std::hypot(s->pos.x - tile.getPosition().x, s->pos.y - tile.getPosition().y) < hexSize + 2) {
                    int amount = s->isCity ? 2 : 1;
                    players[s->ownerId].addResource(tile.getResourceType(), amount);
                    received[s->ownerId][tile.getResourceType()] += amount;
                }
            }
        }
        ++idx;
    }
    return received;
}