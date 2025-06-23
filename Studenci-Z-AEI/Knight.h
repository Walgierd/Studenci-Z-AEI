#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <functional>
#include "Buildable.h"
#include "HexTile.h"



class Knight {
public:
    
    int tileIndex = -1;

    
    void setPosition(int idx);

  
    sf::Vector2f getPosition(const std::vector<HexTile>& tiles) const;

  
    void draw(sf::RenderWindow& window, const std::vector<HexTile>& tiles, float hexSize) const;

 
    bool blocksTile(int idx) const;

  
    void startMoveMode(
        std::vector<std::unique_ptr<BuildSpotButton>>& knightMoveButtons,
        const std::vector<HexTile>& tiles,
        int currentTileIndex,
        std::function<void(int)> onMove
    );

    void handleMoveClick(
        const sf::Vector2f& mousePos,
        std::vector<std::unique_ptr<BuildSpotButton>>& knightMoveButtons,
        bool& knightMoveMode
    );
};


struct KnightMoveButton : public BuildSpotButton {//przycisk dla rycerza na środek hexa
    int tileIdx;
    std::function<void(int)> moveCallback;
    KnightMoveButton(const sf::Vector2f& p, int idx, std::function<void(int)> cb);
    void onClick() override;
};

