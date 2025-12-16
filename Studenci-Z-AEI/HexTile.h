#pragma once

#include "Resource.h"
#include "Drawable.h"
#include <vector>
#include <string>
#include <SFML/Graphics.hpp>
#include <filesystem>
#include <future>
#include <map>

enum class PortType {
    Generic,    
    Kawa,      
    Piwo,
    Notatki,
    Pizza,
    Kabel
};

struct Port {
    sf::Vector2f edgeStart; 
    sf::Vector2f edgeEnd;   
    sf::Vector2f pos;       
    PortType type;
    std::string label;
    float angle; // no prawie elka

    Port(const sf::Vector2f& a, const sf::Vector2f& b, PortType t)
        : edgeStart(a), edgeEnd(b), pos((a + b) / 2.f), type(t)
    {
        float dx = b.x - a.x, dy = b.y - a.y;
        angle = std::atan2(dy, dx) * 180.f / 3.14159265f;
        switch (type) {
        case PortType::Kawa: label = "Kawa 2:1"; break;
        case PortType::Piwo: label = "Piwo 2:1"; break;
        case PortType::Notatki: label = "Notatki 2:1"; break;
        case PortType::Pizza: label = "Pizza 2:1"; break;
        case PortType::Kabel: label = "Kabel 2:1"; break;
        default: label = "3:1"; break;
        }
    }
};

class HexTile : public Tile, public Drawable {
public:
    HexTile(float x, float y, float size, ResourceType resource, int number = 0);
    void draw(sf::RenderWindow& window) const override;
    ResourceType getResourceType() const override;
    sf::Vector2f getPosition() const override;
    int getNumber() const { return number; }

    static std::vector<Port> ports;
    static void setupPorts(const std::vector<sf::Vector2f>& hexCenters, float hexSize);

    static void drawPorts(sf::RenderWindow& window);

private:
    void setupHexShape(float size);
    ResourceType resourceType;
    sf::Vector2f position;
    sf::ConvexShape hexShape;
    float hexSize;
    int number;

    sf::Texture texture; 
    std::string textureFileName; 

    static std::map<ResourceType, sf::Texture> textures;
    static void loadTextures();
};

















