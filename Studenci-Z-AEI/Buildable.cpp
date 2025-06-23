#include "Buildable.h"
#include <filesystem>
#include <ranges>

void Road::draw(sf::RenderWindow& window) const {
    sf::RectangleShape shape;
    shape.setSize(sf::Vector2f(40, 10));
    shape.setOrigin(20, 5);
    shape.setPosition((start + end) / 2.f);
    shape.setRotation(atan2(end.y - start.y, end.x - start.x) * 180.f / 3.14159f);
    shape.setFillColor(sf::Color(200, 50 + 40 * ownerId, 50));
    window.draw(shape);

    static sf::Font font;
    static bool loaded = false;
    static bool checked = false;
    if (!checked) {
        if (std::filesystem::exists("Fonts/arial.ttf")) {
            loaded = font.loadFromFile("Fonts/arial.ttf");
        }
        checked = true;
    }
    if (loaded) {
        sf::Text text(std::to_string(ownerId + 1), font, 12);
        text.setFillColor(sf::Color::White);
        text.setPosition((start + end) / 2.f);
        window.draw(text);
    }
}
sf::Vector2f Road::getPosition() const { return (start + end) / 2.f; }

void Settlement::draw(sf::RenderWindow& window) const {
    static sf::Font font;
    static bool loaded = false;
    static bool checked = false;
    if (!checked) {
        if (std::filesystem::exists("Fonts/arial.ttf")) {
            loaded = font.loadFromFile("Fonts/arial.ttf");
        }
        checked = true;
    }
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
sf::Vector2f Settlement::getPosition() const { return pos; }

void BuildSpotButton::draw(sf::RenderWindow& window) const {
    if (!visible) return;
    sf::CircleShape circ(radius);
    circ.setOrigin(radius, radius);
    circ.setPosition(pos);
    circ.setFillColor(highlighted ? sf::Color(200, 255, 200) : sf::Color(200, 200, 200, 180));
    circ.setOutlineColor(sf::Color::Black);
    circ.setOutlineThickness(2.f);
    window.draw(circ);
}
bool BuildSpotButton::isClicked(const sf::Vector2f& mouse) const {
    return visible && std::hypot(mouse.x - pos.x, mouse.y - pos.y) <= radius;
}

SettlementSpotButton::SettlementSpotButton(const sf::Vector2f& p, std::function<void(const sf::Vector2f&)> cb) {
    pos = p;
    buildCallback = cb;
}
void SettlementSpotButton::onClick() {
    if (buildCallback) buildCallback(pos);
    visible = false;
}

RoadSpotButton::RoadSpotButton(const sf::Vector2f& p1, const sf::Vector2f& p2, std::function<void(const sf::Vector2f&, const sf::Vector2f&)> cb) {
    pos = (p1 + p2) / 2.f;
    endPos = p2;
    buildCallback = cb;
}
void RoadSpotButton::onClick() {
    if (buildCallback) buildCallback(pos, endPos);
    visible = false;
}

std::vector<sf::Vector2f> getUniqueHexVertices(const std::vector<sf::Vector2f>& centers, float size, float epsilon) {
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

std::vector<std::pair<sf::Vector2f, sf::Vector2f>> getUniqueHexEdges(const std::vector<sf::Vector2f>& centers, float size, float epsilon) {
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

bool isSettlementFarEnough(const std::vector<std::unique_ptr<Buildable>>& buildables, const sf::Vector2f& pos, float minDist) {
    auto settlements = buildables
        | std::views::transform([](const auto& b) { return dynamic_cast<Settlement*>(b.get()); })
        | std::views::filter([](auto* s) { return s != nullptr; });
    for (auto* s : settlements) {
        if (std::hypot(s->pos.x - pos.x, s->pos.y - pos.y) < minDist)
            return false;
    }
    return true;
}

bool isRoadConnected(const std::vector<std::unique_ptr<Buildable>>& buildables, const sf::Vector2f& start, const sf::Vector2f& end, int playerId, float tolerance) {
    auto settlements = buildables
        | std::views::transform([](const auto& b) { return dynamic_cast<Settlement*>(b.get()); })
        | std::views::filter([](auto* s) { return s != nullptr; });
    for (auto* s : settlements) {
        if (s->ownerId == playerId &&
            (std::hypot(s->pos.x - start.x, s->pos.y - start.y) < tolerance ||
             std::hypot(s->pos.x - end.x, s->pos.y - end.y) < tolerance))
            return true;
    }
    auto roads = buildables
        | std::views::transform([](const auto& b) { return dynamic_cast<Road*>(b.get()); })
        | std::views::filter([](auto* r) { return r != nullptr; });
    for (auto* r : roads) {
        if (r->ownerId == playerId &&
            (std::hypot(r->start.x - start.x, r->start.y - start.y) < tolerance ||
             std::hypot(r->end.x - end.x, r->end.y - end.y) < tolerance ||
             std::hypot(r->start.x - end.x, r->start.y - end.y) < tolerance ||
             std::hypot(r->end.x - start.x, r->end.y - start.y) < tolerance))
            return true;
    }
    return false;
}

void initializeBuildButtons(
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
    
            bool occupied = false;
            for (const auto& b : buildables) {
                if (auto* s = dynamic_cast<Settlement*>(b.get())) {
                    if (std::hypot(s->pos.x - pos.x, s->pos.y - pos.y) < 1.0f) {
                        occupied = true;
                        break;
                    }
                }
            }
            if (!occupied) {
                buildButtons.push_back(std::make_unique<SettlementSpotButton>(pos, [&](const sf::Vector2f& p) {
              
                }));
            }
        }
    }

    if (buildMode == BuildMode::Road) {
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
            if (!occupied) {
                buildButtons.push_back(std::make_unique<RoadSpotButton>(edge.first, edge.second, [&](const sf::Vector2f& a, const sf::Vector2f& b) {
          
                }));
            }
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
    bool setupPhase,
    Logs* logs
) {

    if (!isSettlementFarEnough(buildables, pos, minDist)) {
        if (logs) logs->add("Za blisko innego akademika!");
        return false;
    }


    int playerSettlements = 0;
    std::vector<sf::Vector2f> playerSettlementPositions;
    for (const auto& b : buildables) {
        if (auto* s = dynamic_cast<Settlement*>(b.get())) {
            if (s->ownerId == players[currentPlayer].getId()) {
                playerSettlements++;
                playerSettlementPositions.push_back(s->pos);
            }
        }
    }

    if (playerSettlements >= 2) {
        bool connected = false;
        for (const auto& prevPos : playerSettlementPositions) {
            if (areSettlementsConnected(buildables, players[currentPlayer].getId(), prevPos, pos)) {
                connected = true;
                break;
            }
        }
        if (!connected) {
            if (logs) logs->add("Nowy akademik musi byc polaczony z jednym z Twoich wczesniejszych akademikow!");
   
            return false;
        }
    }

    if (setupPhase ||
        (players[currentPlayer].getResourceCount(ResourceType::Pizza) >= 1 &&
            players[currentPlayer].getResourceCount(ResourceType::Piwo) >= 1 &&
            players[currentPlayer].getResourceCount(ResourceType::Notatki) >= 1) ||
        freeBuildSettlement) {
        if (!setupPhase && !freeBuildSettlement) {
            players[currentPlayer].removeResource(ResourceType::Pizza, 1);
            players[currentPlayer].removeResource(ResourceType::Piwo, 1);
            players[currentPlayer].removeResource(ResourceType::Notatki, 1);
        }
        auto settlement = std::make_unique<Settlement>();
        settlement->ownerId = players[currentPlayer].getId();
        settlement->pos = pos;
        buildables.push_back(std::move(settlement));
        freeBuildSettlement = false;

   
        for (const auto& port : HexTile::ports) {
            if (std::hypot(port.pos.x - pos.x, port.pos.y - pos.y) < 20.f) { 
                players[currentPlayer].addPort(port.type);
            }
        }

        if (logs) logs->add("Gracz " + std::to_string(players[currentPlayer].getId() + 1) + " buduje akademik");
        return true;
    }
    else {
        if (logs) logs->add("Brak zasobow do budowy akademika!");
    }
    return false;
}

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
) {

    for (const auto& b : buildables) {
        if (auto* r = dynamic_cast<Road*>(b.get())) {
            bool same = (std::hypot(r->start.x - start.x, r->start.y - start.y) < 1.0f &&
                         std::hypot(r->end.x - end.x, r->end.y - end.y) < 1.0f);
            bool reverse = (std::hypot(r->start.x - end.x, r->start.y - end.y) < 1.0f &&
                            std::hypot(r->end.x - start.x, r->end.y - start.y) < 1.0f);
            if (same || reverse) {
                if (logs) logs->add("Droga już istnieje!");
                return false;
            }
        }
    }

    bool valid = false;
    if (setupPhase) {
        float distStart = std::hypot(start.x - lastSettlementPos.x, start.y - lastSettlementPos.y);
        float distEnd = std::hypot(end.x - lastSettlementPos.x, end.y - lastSettlementPos.y);
        if (distStart < 1e-2 || distEnd < 1e-2) valid = true;
    }
    else {
        valid = isRoadConnected(buildables, start, end, players[currentPlayer].getId());
    }

    if (valid || (freeBuildRoad && isRoadConnected(buildables, start, end, players[currentPlayer].getId()))) {
        if (setupPhase ||
            (players[currentPlayer].getResourceCount(ResourceType::Kawa) >= 1 &&
                players[currentPlayer].getResourceCount(ResourceType::Kabel) >= 1) ||
            freeBuildRoad) {
            if (!setupPhase && !freeBuildRoad) {
                players[currentPlayer].removeResource(ResourceType::Kawa, 1);
                players[currentPlayer].removeResource(ResourceType::Kabel, 1);
            }
            auto road = std::make_unique<Road>();
            road->ownerId = players[currentPlayer].getId();
            road->start = start;
            road->end = end;
            buildables.push_back(std::move(road));
            freeBuildRoad = false;
            return true;
        }
        else {
            if (logs) logs->add("Brak zasobow do budowy korytarza!");
        }
    }
    else {
        if (logs) logs->add("Korytarz musi byc polaczony z Twoja infrastruktura!");
    }
    return false;
}

bool tryBuildCity(
    std::vector<std::unique_ptr<Buildable>>& buildables,
    std::vector<Player>& players,
    int currentPlayer,
    const sf::Vector2f& pos
) {
    auto settlements = buildables
        | std::views::transform([](const auto& b) { return dynamic_cast<Settlement*>(b.get()); })
        | std::views::filter([](auto* s) { return s != nullptr; });
    for (auto* s : settlements) {
        if (!s->isCity && s->ownerId == players[currentPlayer].getId() &&
            std::hypot(s->pos.x - pos.x, s->pos.y - pos.y) < 40.f) {
            if (players[currentPlayer].getResourceCount(ResourceType::Kawa) >= 2 &&
                players[currentPlayer].getResourceCount(ResourceType::Piwo) >= 2 &&
                players[currentPlayer].getResourceCount(ResourceType::Pizza) >= 2) {
                players[currentPlayer].removeResource(ResourceType::Kawa, 2);
                players[currentPlayer].removeResource(ResourceType::Piwo, 2);
                players[currentPlayer].removeResource(ResourceType::Pizza, 2);
                s->isCity = true;
                return true;
            }
        }
    }
    return false;
}

bool areSettlementsConnected(
    const std::vector<std::unique_ptr<Buildable>>& buildables,
    int playerId,
    const sf::Vector2f& from,
    const sf::Vector2f& to,
    float epsilon
) {
    std::set<sf::Vector2f, bool(*)(const sf::Vector2f&, const sf::Vector2f&)> nodes(
        [](const sf::Vector2f& a, const sf::Vector2f& b) {
            if (std::abs(a.x - b.x) > 1.0f) return a.x < b.x;
            return a.y < b.y;
        }
    );
    std::multimap<sf::Vector2f, sf::Vector2f, decltype(nodes.key_comp())> edges(nodes.key_comp());

    for (const auto& b : buildables) {
        if (auto* r = dynamic_cast<Road*>(b.get())) {
            if (r->ownerId == playerId) {
                nodes.insert(r->start);
                nodes.insert(r->end);
                edges.insert({ r->start, r->end });
                edges.insert({ r->end, r->start });
            }
        }
    }

    for (const auto& b : buildables) {
        if (auto* s = dynamic_cast<Settlement*>(b.get())) {
            if (s->ownerId == playerId) {
                nodes.insert(s->pos);
            }
        }
    }

    std::queue<sf::Vector2f> q;
    std::set<sf::Vector2f, decltype(nodes.key_comp())> visited(nodes.key_comp());
    q.push(from);
    visited.insert(from);

    while (!q.empty()) {
        sf::Vector2f curr = q.front(); q.pop();
        if (std::hypot(curr.x - to.x, curr.y - to.y) < epsilon)
            return true;
        auto range = edges.equal_range(curr);
        for (auto it = range.first; it != range.second; ++it) {
            if (visited.count(it->second) == 0) {
                visited.insert(it->second);
                q.push(it->second);
            }
        }
    }
    return false;
}