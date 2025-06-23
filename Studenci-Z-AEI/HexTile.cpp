#include "HexTile.h"
#include "Buildable.h"
#include <cmath>
#include <string>
#include <map>
#include <algorithm>
#include <numeric>
#include <ranges>


struct Vector2fPairLess {
    bool operator()(const std::pair<sf::Vector2f, sf::Vector2f>& lhs, const std::pair<sf::Vector2f, sf::Vector2f>& rhs) const {
        if (lhs.first.x != rhs.first.x) return lhs.first.x < rhs.first.x;
        if (lhs.first.y != rhs.first.y) return lhs.first.y < rhs.first.y;
        if (lhs.second.x != rhs.second.x) return lhs.second.x < rhs.second.x;
        return lhs.second.y < rhs.second.y;
    }
};

std::vector<Port> HexTile::ports;

// dodałem ranges na laby
static std::vector<sf::Vector2f> getOuterVertices(const std::vector<sf::Vector2f>& hexCenters, float hexSize, float epsilon = 1.0f) {
    auto vertices = getUniqueHexVertices(hexCenters, hexSize, epsilon);
    auto edges = getUniqueHexEdges(hexCenters, hexSize, epsilon);

    std::vector<int> edgeCount(vertices.size(), 0);
    for (const auto& edge : edges) {
        for (size_t i = 0; i < vertices.size(); ++i) {
            if (std::hypot(vertices[i].x - edge.first.x, vertices[i].y - edge.first.y) < epsilon ||
                std::hypot(vertices[i].x - edge.second.x, vertices[i].y - edge.second.y) < epsilon) {
                edgeCount[i]++;
            }
        }
    }

    
    std::vector<sf::Vector2f> outerVertices;
    for (size_t i = 0; i < vertices.size(); ++i) {
        if (edgeCount[i] == 2)
            outerVertices.push_back(vertices[i]);
    }
    return outerVertices;
}

HexTile::HexTile(float x, float y, float size, ResourceType resource, int number)
    : resourceType(resource), position(x, y), hexSize(size), number(number)
{
    setupHexShape(size);
    hexShape.setPosition(position);
    switch (resourceType) {
    case ResourceType::Kawa:    hexShape.setFillColor(sf::Color(139, 69, 19)); break;
    case ResourceType::Piwo:    hexShape.setFillColor(sf::Color(255, 215, 0)); break; // piwopiwopiwo
    case ResourceType::Notatki: hexShape.setFillColor(sf::Color::White); break;
    case ResourceType::Pizza:   hexShape.setFillColor(sf::Color(255, 99, 71)); break;
    case ResourceType::Kabel:   hexShape.setFillColor(sf::Color(128, 128, 128)); break;
    default:                    hexShape.setFillColor(sf::Color(100, 100, 100)); break;
    }
}

void HexTile::setupHexShape(float size) {
    hexShape.setPointCount(6);
    for (int i = 0; i < 6; ++i) {
        float angle = 3.14159f / 3.f * i - 3.14159f / 6.f;
        hexShape.setPoint(i, { size * std::cos(angle), size * std::sin(angle) });
    }
}

void HexTile::setupPorts(const std::vector<sf::Vector2f>& hexCenters, float hexSize) {
    ports.clear();

    auto futureOuterVertices = std::async(std::launch::async, [&]() {
        return getOuterVertices(hexCenters, hexSize);
    });

    auto outerVertices = futureOuterVertices.get();

    const int portCount = 9;
    std::vector<PortType> portTypes = {
        PortType::Kawa, PortType::Generic, PortType::Piwo, PortType::Generic, PortType::Notatki,
        PortType::Generic, PortType::Pizza, PortType::Generic, PortType::Kabel
    };

    // srodek z ranges
    sf::Vector2f center = std::accumulate(
        hexCenters.begin(), hexCenters.end(), sf::Vector2f(0, 0),
        [](const sf::Vector2f& acc, const sf::Vector2f& c) { return acc + c; }
    );
    center.x /= hexCenters.size();
    center.y /= hexCenters.size();

    // sortowanie z ranges
    std::ranges::sort(outerVertices, [center](const sf::Vector2f& v1, const sf::Vector2f& v2) {
        float a1 = std::atan2(v1.y - center.y, v1.x - center.x);
        float a2 = std::atan2(v2.y - center.y, v2.x - center.x);
        return a1 < a2;
    });

    for (int i = 0; i < portCount && i < static_cast<int>(outerVertices.size()); ++i) {
        int idx = static_cast<int>(i * outerVertices.size() / float(portCount));
        sf::Vector2f v = outerVertices[idx];
        ports.emplace_back(v, v, portTypes[i]);
    }
}

void HexTile::draw(sf::RenderWindow& window) const {
    window.draw(hexShape);

    if (resourceType != ResourceType::None) {
        float circleRadius = hexSize / 2.4f;
        sf::CircleShape circle(circleRadius);
        circle.setFillColor(sf::Color::Black);
        circle.setOrigin(circleRadius, circleRadius);
        circle.setPosition(position);

        window.draw(circle);

        static sf::Font font;
        static bool fontLoaded = false;
        if (!fontLoaded) {
            if (std::filesystem::exists("Fonts/arial.ttf")) {
                fontLoaded = font.loadFromFile("Fonts/arial.ttf");
            } else {
                // nie chce mi się no wiadomo że czcionka będzie w plikach
                return;
            }
        }
        std::string numberStr = std::to_string(number);
        sf::Text text;
        text.setFont(font);
        text.setString(numberStr);
        text.setCharacterSize(static_cast<unsigned int>(circleRadius * 0.8f));
        text.setFillColor(sf::Color::White);
        text.setStyle(sf::Text::Bold);

        sf::FloatRect bounds = text.getLocalBounds();
        text.setOrigin(bounds.left + bounds.width / 2.f, bounds.top + bounds.height / 2.f);
        text.setPosition(position);

        window.draw(text);
    }
}

void HexTile::drawPorts(sf::RenderWindow& window) {
    static sf::Font font;
    static bool fontLoaded = false;
    if (!fontLoaded) {
        if (std::filesystem::exists("Fonts/pixel.ttf")) {
            fontLoaded = font.loadFromFile("Fonts/pixel.ttf");
        } else {
            // ditto
            return;
        }
    }
    for (const auto& port : ports) {
        // miała być elka ale wyszedł ala mostek, imo może być
        float mainLen = 40.f, mainThick = 12.f;
        float legLen = 28.f, legThick = 12.f;


        sf::RectangleShape base(sf::Vector2f(mainLen, mainThick));
        base.setFillColor(sf::Color(80, 80, 80));
        base.setOrigin(mainLen / 2.f, mainThick / 2.f);
        base.setPosition(port.pos);
        base.setRotation(port.angle);

        sf::RectangleShape leg(sf::Vector2f(legThick, legLen));
        leg.setFillColor(sf::Color(80, 80, 80));
        float rad = port.angle * 3.14159265f / 180.f;
        float dx = std::cos(rad) * (mainLen / 2.f - legThick / 2.f);
        float dy = std::sin(rad) * (mainLen / 2.f - legThick / 2.f);
        leg.setOrigin(legThick / 2.f, 0.f);
        leg.setPosition(port.pos.x + dx, port.pos.y + dy);
        leg.setRotation(port.angle + 90.f);

        window.draw(base);
        window.draw(leg);

   
        sf::Text text;
        text.setFont(font);
        text.setString(port.label);
        text.setCharacterSize(18);
		text.setFillColor(sf::Color(204, 85, 0)); // kolor portu
        text.setStyle(sf::Text::Bold);
        sf::FloatRect bounds = text.getLocalBounds();
        text.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
        text.setPosition(port.pos.x, port.pos.y - 30);
        window.draw(text);
    }
}

ResourceType HexTile::getResourceType() const {
    return resourceType;
}

sf::Vector2f HexTile::getPosition() const {
    return position;
}
