#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include <cmath>
#include <functional>
#include <vector>
#include <set>
#include <queue>
#include <map>

#include "Player.h"
#include "Board.h"
#include "Logs.h"   

class Board;

enum class BuildMode { None, Road, Settlement, City };

struct Buildable {
    int ownerId = -1;
    virtual ~Buildable() = default;
    virtual void draw(sf::RenderWindow& window) const = 0;
    virtual sf::Vector2f getPosition() const = 0;
};

struct Road : public Buildable {
    sf::Vector2f start, end;
    void draw(sf::RenderWindow& window) const override;
    sf::Vector2f getPosition() const override;
};

struct Settlement : public Buildable {
    sf::Vector2f pos;
    bool isCity = false;
    void draw(sf::RenderWindow& window) const override;
    sf::Vector2f getPosition() const override;
};

struct BuildSpotButton {
    sf::Vector2f pos;
    float radius = 10.f;
    bool highlighted = false;
    bool visible = true;
    virtual ~BuildSpotButton() = default;
    virtual void draw(sf::RenderWindow& window) const;
    virtual bool isClicked(const sf::Vector2f& mouse) const;
    virtual void onClick() = 0;
};

struct SettlementSpotButton : public BuildSpotButton {
    std::function<void(const sf::Vector2f&)> buildCallback;
    SettlementSpotButton(const sf::Vector2f& p, std::function<void(const sf::Vector2f&)> cb);
    void onClick() override;
};

struct RoadSpotButton : public BuildSpotButton {
    sf::Vector2f endPos;
    std::function<void(const sf::Vector2f&, const sf::Vector2f&)> buildCallback;
    RoadSpotButton(const sf::Vector2f& p1, const sf::Vector2f& p2, std::function<void(const sf::Vector2f&, const sf::Vector2f&)> cb);
    void onClick() override;
};

std::vector<sf::Vector2f> getUniqueHexVertices(const std::vector<sf::Vector2f>& centers, float size, float epsilon = 1.0f);
std::vector<std::pair<sf::Vector2f, sf::Vector2f>> getUniqueHexEdges(const std::vector<sf::Vector2f>& centers, float size, float epsilon = 1.0f);
bool isSettlementFarEnough(const std::vector<std::unique_ptr<Buildable>>& buildables, const sf::Vector2f& pos, float minDist);
bool isRoadConnected(const std::vector<std::unique_ptr<Buildable>>& buildables, const sf::Vector2f& start, const sf::Vector2f& end, int playerId, float tolerance = 20.f);
void initializeBuildButtons(
    std::vector<std::unique_ptr<BuildSpotButton>>& buildButtons,
    std::vector<std::unique_ptr<Buildable>>& buildables,
    Board& board,
    float hexSize,
    BuildMode buildMode,
    std::vector<Player>& players,
    int currentPlayer,
    sf::RenderWindow& window
);

bool tryBuildSettlement(
    std::vector<std::unique_ptr<Buildable>>& buildables,
    std::vector<Player>& players,
    int currentPlayer,
    const sf::Vector2f& pos,
    float minDist,
    bool& freeBuildSettlement,
    bool setupPhase,
    Logs* logs
);

bool tryBuildRoad(
    std::vector<std::unique_ptr<Buildable>>& buildables,
    std::vector<Player>& players,
    int currentPlayer,
    const sf::Vector2f& start,
    const sf::Vector2f& end,
    bool& freeBuildRoad,
    bool setupPhase,
    const sf::Vector2f& lastSettlementPos,
    Logs* logs
);

bool tryBuildCity(
    std::vector<std::unique_ptr<Buildable>>& buildables,
    std::vector<Player>& players,
    int currentPlayer,
    const sf::Vector2f& pos
);

bool areSettlementsConnected(
    const std::vector<std::unique_ptr<Buildable>>& buildables,
    int playerId,
    const sf::Vector2f& from,
    const sf::Vector2f& to,
    float epsilon = 1.0f
);