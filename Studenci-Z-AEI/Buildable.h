#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include <cmath>
#include <functional>
#include <vector>
#include <set>

#include "Player.h"
#include "Board.h"

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
    void draw(sf::RenderWindow& window) const override {
        sf::RectangleShape shape;
        shape.setSize(sf::Vector2f(40, 10));
        shape.setOrigin(20, 5);
        shape.setPosition((start + end) / 2.f);
        shape.setRotation(atan2(end.y - start.y, end.x - start.x) * 180.f / 3.14159f);
        shape.setFillColor(sf::Color(200, 50 + 40 * ownerId, 50));
        window.draw(shape);

        static sf::Font font;
        static bool loaded = font.loadFromFile("Fonts/arial.ttf");
        if (loaded) {
            sf::Text text(std::to_string(ownerId + 1), font, 12);
            text.setFillColor(sf::Color::White);
            text.setPosition((start + end) / 2.f);
            window.draw(text);
        }
    }
    sf::Vector2f getPosition() const override { return (start + end) / 2.f; }
};

struct Settlement : public Buildable {
    sf::Vector2f pos;
    bool isCity = false;
    void draw(sf::RenderWindow& window) const override {
        static sf::Font font;
        static bool loaded = font.loadFromFile("Fonts/arial.ttf");
        if (isCity) {
            sf::RectangleShape rect(sf::Vector2f(32, 32));
            rect.setOrigin(16, 16);
            rect.setPosition(pos);
            rect.setFillColor(sf::Color(200, 200, 0));
            rect.setOutlineColor(sf::Color::Black);
            rect.setOutlineThickness(3.f);
            window.draw(rect);
            if (loaded) {
                sf::Text text(std::to_string(ownerId + 1), font, 18);
                text.setFillColor(sf::Color::Black);
                text.setPosition(pos.x - 8, pos.y - 12);
                window.draw(text);
            }
        }
        else {
            sf::CircleShape circ(16.f);
            circ.setOrigin(16, 16);
            circ.setPosition(pos);
            circ.setFillColor(sf::Color(100, 100, 255));
            circ.setOutlineColor(sf::Color::Black);
            circ.setOutlineThickness(3.f);
            window.draw(circ);
            if (loaded) {
                sf::Text text(std::to_string(ownerId + 1), font, 18);
                text.setFillColor(sf::Color::Black);
                text.setPosition(pos.x - 8, pos.y - 12);
                window.draw(text);
            }
        }
    }
    sf::Vector2f getPosition() const override { return pos; }
};

struct BuildSpotButton {
    sf::Vector2f pos;
    float radius = 10.f;
    bool highlighted = false;
    bool visible = true;
    virtual ~BuildSpotButton() = default;
    virtual void draw(sf::RenderWindow& window) const {
        if (!visible) return;
        sf::CircleShape circ(radius);
        circ.setOrigin(radius, radius);
        circ.setPosition(pos);
        circ.setFillColor(highlighted ? sf::Color(200, 255, 200) : sf::Color(200, 200, 200, 180));
        circ.setOutlineColor(sf::Color::Black);
        circ.setOutlineThickness(2.f);
        window.draw(circ);
    }
    virtual bool isClicked(const sf::Vector2f& mouse) const {
        return visible && std::hypot(mouse.x - pos.x, mouse.y - pos.y) <= radius;
    }
    virtual void onClick() = 0;
};

struct SettlementSpotButton : public BuildSpotButton {
    std::function<void(const sf::Vector2f&)> buildCallback;
    SettlementSpotButton(const sf::Vector2f& p, std::function<void(const sf::Vector2f&)> cb) {
        pos = p;
        buildCallback = cb;
    }
    void onClick() override {
        if (buildCallback) buildCallback(pos);
        visible = false;
    }
};

struct RoadSpotButton : public BuildSpotButton {
    sf::Vector2f endPos;
    std::function<void(const sf::Vector2f&, const sf::Vector2f&)> buildCallback;
    RoadSpotButton(const sf::Vector2f& p1, const sf::Vector2f& p2, std::function<void(const sf::Vector2f&, const sf::Vector2f&)> cb) {
        pos = (p1 + p2) / 2.f;
        endPos = p2;
        buildCallback = cb;
    }
    void onClick() override {
        if (buildCallback) buildCallback(pos, endPos);
        visible = false;
    }
};

inline std::vector<sf::Vector2f> getUniqueHexVertices(const std::vector<sf::Vector2f>& centers, float size, float epsilon = 1.0f) {
    std::vector<sf::Vector2f> allVertices;
    for (const auto& center : centers) {
        for (int i = 0; i < 6; ++i) {
            float angle = 3.14159265f / 3.f * i - 3.14159265f / 6.f;
            sf::Vector2f v(center.x + size * std::cos(angle), center.y + size * std::sin(angle));
            bool found = false;
            for (const auto& existing : allVertices) {
                if (std::hypot(existing.x - v.x, existing.y - v.y) < epsilon) {
                    found = true;
                    break;
                }
            }
            if (!found) allVertices.push_back(v);
        }
    }
    return allVertices;
}

inline std::vector<std::pair<sf::Vector2f, sf::Vector2f>> getUniqueHexEdges(const std::vector<sf::Vector2f>& centers, float size, float epsilon = 1.0f) {
    std::vector<std::pair<sf::Vector2f, sf::Vector2f>> allEdges;
    for (const auto& center : centers) {
        std::vector<sf::Vector2f> v;
        for (int i = 0; i < 6; ++i) {
            float angle = 3.14159265f / 3.f * i - 3.14159265f / 6.f;
            v.emplace_back(center.x + size * std::cos(angle), center.y + size * std::sin(angle));
        }
        for (int i = 0; i < 6; ++i) {
            sf::Vector2f a = v[i];
            sf::Vector2f b = v[(i + 1) % 6];
            bool found = false;
            for (const auto& existing : allEdges) {
                bool same = (std::hypot(existing.first.x - a.x, existing.first.y - a.y) < epsilon &&
                    std::hypot(existing.second.x - b.x, existing.second.y - b.y) < epsilon);
                bool reverse = (std::hypot(existing.first.x - b.x, existing.first.y - b.y) < epsilon &&
                    std::hypot(existing.second.x - a.x, existing.second.y - a.y) < epsilon);
                if (same || reverse) {
                    found = true;
                    break;
                }
            }
            if (!found) allEdges.emplace_back(a, b);
        }
    }
    return allEdges;
}

inline bool isSettlementFarEnough(const std::vector<std::unique_ptr<Buildable>>& buildables, const sf::Vector2f& pos, float minDist) {
    for (const auto& b : buildables) {
        if (auto* s = dynamic_cast<Settlement*>(b.get())) {
            if (std::hypot(s->pos.x - pos.x, s->pos.y - pos.y) < minDist)
                return false;
        }
    }
    return true;
}

inline bool isRoadConnected(const std::vector<std::unique_ptr<Buildable>>& buildables, const sf::Vector2f& start, const sf::Vector2f& end, int playerId, float tolerance = 20.f) {
    for (const auto& b : buildables) {
        if (auto* s = dynamic_cast<Settlement*>(b.get())) {
            if (s->ownerId == playerId &&
                (std::hypot(s->pos.x - start.x, s->pos.y - start.y) < tolerance ||
                    std::hypot(s->pos.x - end.x, s->pos.y - end.y) < tolerance))
                return true;
        }
        if (auto* r = dynamic_cast<Road*>(b.get())) {
            if (r->ownerId == playerId &&
                (std::hypot(r->start.x - start.x, r->start.y - start.y) < tolerance ||
                    std::hypot(r->end.x - end.x, r->end.y - end.y) < tolerance ||
                    std::hypot(r->start.x - end.x, r->start.y - end.y) < tolerance ||
                    std::hypot(r->end.x - start.x, r->end.y - start.y) < tolerance))
                return true;
        }
    }
    return false;
}

inline void initializeBuildButtons(
    std::vector<std::unique_ptr<BuildSpotButton>>& buildButtons,
    std::vector<std::unique_ptr<Buildable>>& buildables,
    Board& board,
    float hexSize,
    BuildMode buildMode,
    std::vector<Player>& players,
    int currentPlayer,
    sf::RenderWindow& window
) {
    buildButtons.clear();
    std::vector<sf::Vector2f> hexCenters;
    for (const auto& tile : board.getTiles())
        hexCenters.push_back(tile.getPosition());

    auto settlementSpots = getUniqueHexVertices(hexCenters, hexSize);
    auto roadSpots = getUniqueHexEdges(hexCenters, hexSize);

    if (buildMode == BuildMode::Settlement) {
        for (const auto& pos : settlementSpots) {
            buildButtons.push_back(std::make_unique<SettlementSpotButton>(pos, [&](const sf::Vector2f& p) {
                }));
        }
    }

    if (buildMode == BuildMode::Road) {
        for (const auto& edge : roadSpots) {
            buildButtons.push_back(std::make_unique<RoadSpotButton>(edge.first, edge.second, [&](const sf::Vector2f& a, const sf::Vector2f& b) {
                }));
        }
    }
}

bool tryBuildSettlement(
    std::vector<std::unique_ptr<Buildable>>& buildables,
    std::vector<Player>& players,
    int currentPlayer,
    const sf::Vector2f& pos,
    float minDist,
    bool& freeBuildSettlement,
    bool setupPhase // nowy parametr
);

bool tryBuildRoad(
    std::vector<std::unique_ptr<Buildable>>& buildables,
    std::vector<Player>& players,
    int currentPlayer,
    const sf::Vector2f& start,
    const sf::Vector2f& end,
    bool& freeBuildRoad,
    bool setupPhase, // nowy parametr
    const sf::Vector2f& lastSettlementPos // nowy parametr
);

bool tryBuildCity(
    std::vector<std::unique_ptr<Buildable>>& buildables,
    std::vector<Player>& players,
    int currentPlayer,
    const sf::Vector2f& pos
);