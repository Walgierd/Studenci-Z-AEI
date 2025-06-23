#include "Game.h"
#include <thread>
#include <mutex>
#include <SFML/Window.hpp> 
#include <ranges> 

Game::Game()
    : window(sf::VideoMode(1920, 1080), "Studenci z AEI", sf::Style::Default),
    currentStyle(sf::Style::Default),
    menu(1920, 1080),
    score(), 
    playerUI(font, &score),
    inMenu(true),
    hexSize(80.f),
    board(hexSize, sf::Vector2f(1920.f / 2.f, 1080.f / 2.f)),
    buildMode(BuildMode::None),
    knight(),
    knightMoveMode(false),
    freeBuildRoad(false),
    freeBuildSettlement(false),
    setupPhase(true),
    setupTurn(0),
    setupStep(0),
    setupPlayerIndex(0),
    logs(font),
    bank(-1)
{
    if (!std::filesystem::exists("Fonts/arial.ttf") || !font.loadFromFile("Fonts/arial.ttf")) {
        throw std::runtime_error("Brak czcionki ");
    }
    knight.setPosition(static_cast<int>(board.getTiles().size() / 2));
    setupPlayerButtons();
    lastSettlementPos.resize(4, sf::Vector2f(-1000, -1000));

 
	players.push_back(bank);//bank dodawany do graczy z id-1 ZOSTWIĆ W SPOKOJU bo wywala wektor

    
    std::vector<sf::Vector2f> hexCenters;//porty tu
    for (const auto& tile : board.getTiles())
        hexCenters.push_back(tile.getPosition());
    HexTile::setupPorts(hexCenters, hexSize);
}

void Game::run() {
    while (window.isOpen()) {
        handleEvents();
        update();
        render();
        if (!turnManager.getPlayers().empty() && buildMode == BuildMode::None) {
            static int lastPlayer = -1;
            if (turnManager.getCurrentPlayerIndex() != lastPlayer) {
                turnManager.getCurrentPlayer().resetDice();
                lastPlayer = turnManager.getCurrentPlayerIndex();
            }
        }
    }
}

void Game::handleEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            window.close();

        if (inMenu)
            handleMenuEvents(event);
        else
            handleGameEvents(event);

        if (event.type == sf::Event::TextEntered) {
            menu.handleTextEntered(event.text.unicode);
        }
    }
}

void Game::handleMenuEvents(const sf::Event& event) {
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2f mousePos = window.mapPixelToCoords({ event.mouseButton.x, event.mouseButton.y });
        if (menu.isStartClicked(mousePos)) {
            int playerCount = menu.getSelectedPlayerCount();
            turnManager.initialize(playerCount);
            lastSettlementPos.assign(menu.getSelectedPlayerCount(), sf::Vector2f(-1000, -1000));
            const auto& nicks = menu.getPlayerNicknames();
            auto& players = turnManager.getPlayers();
            for (size_t i = 0; i < players.size() && i < nicks.size(); ++i) {
                std::string nick = nicks[i];//tu tylko faktyczna część nicku
                size_t colon = nick.find(':');
                if (colon != std::string::npos) {
                    nick = nick.substr(colon + 2);
                }
                players[i].setNickname(nick);
            }
            for (auto& player : players) {
                player.addResource(ResourceType::Pizza, 10);
                player.addResource(ResourceType::Piwo, 10);
                player.addResource(ResourceType::Notatki, 10);
                player.addResource(ResourceType::Kawa, 10);
                player.addResource(ResourceType::Kabel, 10);
            }
            inMenu = false;
        }
        if (menu.isFullscreenClicked(mousePos)) {
            menu.setFullscreenToggleRequested(true);
        }
    }
}

void Game::handleGameEvents(const sf::Event& event) {
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2f mousePos = window.mapPixelToCoords({ event.mouseButton.x, event.mouseButton.y });

        auto& players = turnManager.getPlayers();
        int currentPlayer = turnManager.getCurrentPlayerIndex();

  
        sf::RectangleShape diceButton(sf::Vector2f(120, 50));//rectangle shape jest do kostki
        diceButton.setPosition(static_cast<float>(window.getSize().x) - 180.f, 300.f);
        bool diceClicked = diceButton.getGlobalBounds().contains(mousePos);

      
        if (diceClicked) {// jeden roll na ture
            if (players[currentPlayer].hasRolled()) {
                std::string logMsg = players[currentPlayer].getNickname() + " juz rzucil kostka w tej turze!";
                std::thread([this, logMsg]() {
                    std::lock_guard<std::mutex> lock(logsMutex);
                    logs.add(logMsg);
                }).detach();
                return; 
            }

            auto received = handleDiceRollWithLog(players, currentPlayer, board, buildables, knight, hexSize);
            if (!players[currentPlayer].hasRolled()) return;

            int d1 = players[currentPlayer].getDice1();
            int d2 = players[currentPlayer].getDice2();

            {
                std::string logMsg = players[currentPlayer].getNickname() + " rzuca kostka, wypadlo " + std::to_string(d1 + d2);
                std::thread([this, logMsg]() {
                    std::lock_guard<std::mutex> lock(logsMutex);
                    logs.add(logMsg);
                }).detach();
            }

            std::string resLog = "Otrzymane zasoby:";
            bool anyResource = false;
            for (const auto& [pid, resMap] : received) {
                if (resMap.empty()) continue;
                anyResource = true;
                resLog += " " + players[pid].getNickname() + ":";
                bool first = true;
                for (const auto& [rtype, count] : resMap) {
                    if (!first) resLog += ",";
                    resLog += " " + std::to_string(count) + " " + resourceName(rtype);
                    first = false;
                }
            }
            if (!anyResource) {
                resLog += " Nic";
            }
            {
                std::string logMsg = resLog;
                std::thread([this, logMsg]() {
                    std::lock_guard<std::mutex> lock(logsMutex);
                    logs.add(logMsg);
                }).detach();
            }

            if (players[currentPlayer].getDice1() + players[currentPlayer].getDice2() == 12) {//<----------złodziej przy wyrzuceniu 12 zmiana na debug
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
            return; 
        }

        if (setupPhase) {
            auto& players = turnManager.getPlayers();
            int currentPlayer = setupPlayerIndex;

            if (setupStep == 0) {
                std::vector<sf::Vector2f> hexCenters;
                for (const auto& tile : board.getTiles())
                    hexCenters.push_back(tile.getPosition());
                auto settlementSpots = getUniqueHexVertices(hexCenters, hexSize);

                if (buildButtons.empty()) {
                    buildButtons.clear();
                    for (const auto& pos : settlementSpots) {
                        buildButtons.push_back(std::make_unique<SettlementSpotButton>(pos, [this, pos](const sf::Vector2f&) {
                            bool freeBuildSettlementTemp = true;
                            auto& players = turnManager.getPlayers();
                            int currentPlayer = setupPlayerIndex;
                            if (tryBuildSettlement(buildables, players, currentPlayer, pos, hexSize * std::sqrt(3.f) - 5, freeBuildSettlementTemp, true, &logs)) {
                                lastSettlementPos[currentPlayer] = pos;
                                setupStep = 1;
                                buildButtons.clear();
                            }
                        }));
                    }
                }

                for (auto& btn : buildButtons) {
                    if (btn->isClicked(mousePos)) {
                        btn->onClick();
                        break;
                    }
                }
                return;
            }
            else if (setupStep == 1) {
                std::vector<sf::Vector2f> hexCenters;
                for (const auto& tile : board.getTiles())
                    hexCenters.push_back(tile.getPosition());
                auto roadSpots = getUniqueHexEdges(hexCenters, hexSize);
                for (const auto& edge : roadSpots) {
                    sf::Vector2f mid = (edge.first + edge.second) / 2.f;
                    if ((std::hypot(lastSettlementPos[currentPlayer].x - edge.first.x, lastSettlementPos[currentPlayer].y - edge.first.y) < 1.f ||
                        std::hypot(lastSettlementPos[currentPlayer].x - edge.second.x, lastSettlementPos[currentPlayer].y - edge.second.y) < 1.f) &&
                        std::hypot(mousePos.x - mid.x, mousePos.y - mid.y) < 15.f) {
                        bool freeBuildRoadTemp = true;
                        if (tryBuildRoad(buildables, players, currentPlayer, edge.first, edge.second, freeBuildRoadTemp, true, lastSettlementPos[currentPlayer], &logs)) {
                            setupPlayerIndex++;
                            if (setupPlayerIndex >= static_cast<int>(players.size())) {
                                setupPlayerIndex = 0;
                                setupTurn++;
                                if (setupTurn >= 2) {
                                    setupPhase = false;
                                }
                            }
                            setupStep = 0;
                            buildButtons.clear();
                        }
                        break;
                    }
                }
            }
            return;
        }

       
        if (!players[currentPlayer].hasRolled()) {//rzut kostką przed innymi akcjami
            std::string logMsg = players[currentPlayer].getNickname() + " najpierw rzuc kostka!";
            std::thread([this, logMsg]() {
                std::lock_guard<std::mutex> lock(logsMutex);
                logs.add(logMsg);
            }).detach();
            return;
        }

        if (trade.exchangeMode) {//kliknięcia
            trade.handleClick(mousePos, players, currentPlayer);
            return;
        }
        if (knightMoveMode) {
            knight.handleMoveClick(mousePos, knightMoveButtons, knightMoveMode);
            return;
        }
        for (auto& btn : playerButtons) {
            if (btn->isClicked(mousePos)) btn->onClick();
        }
        if (cardManager.buyCardButton->isClicked(mousePos)) cardManager.buyCardButton->onClick();
        if (cardManager.showCardsButton->isClicked(mousePos)) cardManager.showCardsButton->onClick();

		//*****************************************************************************************budowa akademików
        if (buildMode == BuildMode::Settlement) {
            std::vector<sf::Vector2f> hexCenters;
            for (const auto& tile : board.getTiles())
                hexCenters.push_back(tile.getPosition());
            auto settlementSpots = getUniqueHexVertices(hexCenters, hexSize);
            for (const auto& pos : settlementSpots) {
                if (std::hypot(mousePos.x - pos.x, mousePos.y - pos.y) < 15.f) {
                    if (tryBuildSettlement(buildables, players, currentPlayer, pos, hexSize * std::sqrt(3.f) - 5, freeBuildSettlement, setupPhase, &logs)) {
                        std::string logMsg = players[currentPlayer].getNickname() + " buduje akademik";
                        std::thread([this, logMsg]() {
                            std::lock_guard<std::mutex> lock(logsMutex);
                            logs.add(logMsg);
                        }).detach();
                        buildMode = BuildMode::None;
                    }
                    break;
                }
            }
        }
        //*****************************************************************************************budowa dróg
        else if (buildMode == BuildMode::Road) {
            std::vector<sf::Vector2f> hexCenters;
            for (const auto& tile : board.getTiles())
                hexCenters.push_back(tile.getPosition());
            auto roadSpots = getUniqueHexEdges(hexCenters, hexSize);
            for (const auto& edge : roadSpots) {
                sf::Vector2f mid = (edge.first + edge.second) / 2.f;
                if (std::hypot(mousePos.x - mid.x, mousePos.y - mid.y) < 15.f) {
                    bool valid = tryBuildRoad(buildables, players, currentPlayer, edge.first, edge.second, freeBuildRoad, setupPhase, lastSettlementPos[currentPlayer], &logs);
                    if (valid || (freeBuildRoad && isRoadConnected(buildables, edge.first, edge.second, players[currentPlayer].getId()))) {
                        std::string logMsg = players[currentPlayer].getNickname() + " buduje korytarz";
                        std::thread([this, logMsg]() {
                            std::lock_guard<std::mutex> lock(logsMutex);
                            logs.add(logMsg);
                        }).detach();
                        buildMode = BuildMode::None;

                        std::vector<int> victoryPoints;
                        for (const auto& player : players) {
                            victoryPoints.push_back(cardManager.getVictoryPointCardCount(player.getId()));
                        }
                        score.updateScores(players, buildables, victoryPoints);
                    }
                    break;
                }
            }
        }
        //*****************************************************************************************budowa kampusów
        else if (buildMode == BuildMode::City) {
            if (tryBuildCity(buildables, players, currentPlayer, mousePos)) {
                std::string logMsg = players[currentPlayer].getNickname() + " buduje kampus";
                std::thread([this, logMsg]() {
                    std::lock_guard<std::mutex> lock(logsMutex);
                    logs.add(logMsg);
                }).detach();
                buildMode = BuildMode::None;
            }
        }
        //*****************************************************************************************pokazywanie kart i korzystanie
        if (cardManager.cardsVisible && !turnManager.getPlayers().empty()) {
            int currentPlayerId = turnManager.getCurrentPlayer().getId();
            auto& playerCardsMap = cardManager.getPlayerCards();
            auto it = playerCardsMap.find(currentPlayerId);
            if (it != playerCardsMap.end()) {
                auto& cards = it->second;
                float cardPanelX = static_cast<float>(window.getSize().x) - 420.f;
                float cardPanelY = 400.f;
                float cardWidth = 400.f;
                float cardHeight = 40.f;
                int visibleIdx = 0;
                int clickedIdx = -1;
                for (const auto& card : cards | std::views::filter([](const auto& c) { return c->type != CardType::VictoryPoint; })) {
                    sf::FloatRect cardRect(cardPanelX, cardPanelY + visibleIdx * (cardHeight + 10.f), cardWidth, cardHeight);
                    if (cardRect.contains(mousePos)) {
                        clickedIdx = visibleIdx;
                        break;
                    }
                    ++visibleIdx;
                }
                if (clickedIdx != -1) {
                  
                    int realIdx = -1;
                    int count = 0;
                    for (size_t i = 0; i < cards.size(); ++i) {
                        if (cards[i]->type == CardType::VictoryPoint) continue;
                        if (count == clickedIdx) {
                            realIdx = static_cast<int>(i);
                            break;
                        }
                        ++count;
                    }
                    if (realIdx != -1) {
                    
                        if (turnManager.getCurrentPlayer().hasUsedCardThisTurn()) {
                            std::string logMsg = players[currentPlayer].getNickname() + " nie mozesz uzyc wiecej niż 1 karty w tej turze!";
                            std::thread([this, logMsg]() {
                                std::lock_guard<std::mutex> lock(logsMutex);
                                logs.add(logMsg);
                            }).detach();
                            return;
                        }
                        cardManager.useCard(
                            static_cast<size_t>(realIdx),
                            turnManager.getCurrentPlayer(),
                            buildables,
                            board,
                            knight,
                            players,
                            buildMode,
                            buildButtons,
                            window,
                            hexSize,
                            currentPlayer,
                            freeBuildRoad,
                            freeBuildSettlement,
                            knightMoveMode,
                            knightMoveButtons
                        );
                        return;
                    }
                }
            }
        }
    }
}

void Game::update() {
    if (menu.isFullscreenToggleRequested()) {
        menu.handleFullscreenToggle(window, currentStyle);
        menu.resetFullscreenToggleRequest();
    }

    std::vector<int> victoryPoints;
    const auto& players = turnManager.getPlayers();
    for (const auto& player : players) {
        victoryPoints.push_back(cardManager.getVictoryPointCardCount(player.getId()));
    }
    score.updateScores(players, buildables, victoryPoints);

    
    int winnerId = -1;
    for (const auto& player : players) {
        if (score.getScore(player.getId()) >= 3) {//WARUNEK ZWYCIĘZTWA(standard -15) <-------------------------------------------
            winnerId = player.getId();  
            break;
        }
    }
    //GAME OVER
    static bool gameOver = false;
    if (winnerId != -1 && !gameOver) {
        gameOver = true;
      
        sf::RenderWindow endWindow(sf::VideoMode(1000, 250), "Koniec gry", sf::Style::Titlebar | sf::Style::Close);
        sf::Font endFont;
        endFont.loadFromFile("Fonts/pixel.ttf");
        sf::Text endText;
        endText.setFont(endFont);
        endText.setString("Koniec gry! " + players[winnerId].getNickname() + " wygrywa.\nKliknij, aby wrócić do menu.");//trzeba na nick zmienić jakbyco
        endText.setCharacterSize(32);
        endText.setFillColor(sf::Color::Red);
        endText.setStyle(sf::Text::Bold);
        endText.setPosition(30, 60);

        while (endWindow.isOpen()) {
            sf::Event event;
            while (endWindow.pollEvent(event)) {
                if (event.type == sf::Event::Closed || event.type == sf::Event::MouseButtonPressed) {
                    endWindow.close();
                }
            }
            endWindow.clear(sf::Color::Black);
            endWindow.draw(endText);
            endWindow.display();
        }

        //powrót do menu
        inMenu = true;
        buildables.clear();
        turnManager.initialize(menu.getSelectedPlayerCount());
        setupPhase = true;
        setupTurn = 0;
        setupStep = 0;
        setupPlayerIndex = 0;
        buildButtons.clear();
        playerButtons.clear();
        lastSettlementPos.assign(4, sf::Vector2f(-1000, -1000));
        score = Score();
        cardManager = CardManager();
        setupPlayerButtons();
        gameOver = false;
        return;
    }

    std::vector<Player> scoringPlayers;
    for (const auto& p : turnManager.getPlayers()) {
        if (p.getId() >= 0) scoringPlayers.push_back(p);
    }
    //*****************************************************************************************pierwsza tura
    if (setupPhase) {
        if (setupStep == 0 && buildButtons.empty()) {
            buildButtons.clear();
            std::vector<sf::Vector2f> hexCenters;
            for (const auto& tile : board.getTiles())
                hexCenters.push_back(tile.getPosition());
            auto settlementSpots = getUniqueHexVertices(hexCenters, hexSize);
            for (const auto& pos : settlementSpots) {
                buildButtons.push_back(std::make_unique<SettlementSpotButton>(pos, [this, pos](const sf::Vector2f&) {
                    bool freeBuildSettlementTemp = true;
                    auto& players = turnManager.getPlayers();
                    int currentPlayer = setupPlayerIndex;
                    if (tryBuildSettlement(buildables, players, currentPlayer, pos, hexSize * std::sqrt(3.f) - 5, freeBuildSettlementTemp, true, &logs)) {
                        lastSettlementPos[currentPlayer] = pos;
                        setupStep = 1;
                        buildButtons.clear();
                    }
                }));
            }
        }
        if (setupStep == 1 && buildButtons.empty()) {
            buildButtons.clear();
            std::vector<sf::Vector2f> hexCenters;
            for (const auto& tile : board.getTiles())
                hexCenters.push_back(tile.getPosition());
            auto roadSpots = getUniqueHexEdges(hexCenters, hexSize);
            for (const auto& edge : roadSpots) {
                sf::Vector2f mid = (edge.first + edge.second) / 2.f;
                if (std::hypot(lastSettlementPos[setupPlayerIndex].x - edge.first.x, lastSettlementPos[setupPlayerIndex].y - edge.first.y) < 1.f ||
                    std::hypot(lastSettlementPos[setupPlayerIndex].x - edge.second.x, lastSettlementPos[setupPlayerIndex].y - edge.second.y) < 1.f) {
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
                        buildButtons.push_back(std::make_unique<RoadSpotButton>(edge.first, edge.second, [this, edge](const sf::Vector2f&, const sf::Vector2f&) {
                            bool freeBuildRoadTemp = true;
                            auto& players = turnManager.getPlayers();
                            int currentPlayer = setupPlayerIndex;
                            if (tryBuildRoad(buildables, players, currentPlayer, edge.first, edge.second, freeBuildRoadTemp, true, lastSettlementPos[currentPlayer], &logs)) {
                                setupPlayerIndex++;
                                if (setupPlayerIndex >= static_cast<int>(players.size())) {
                                    setupPlayerIndex = 0;
                                    setupTurn++;
                                    if (setupTurn >= 2) {
                                        setupPhase = false;
                                    }
                                }
                                setupStep = 0;
                                buildButtons.clear();
                            }
                        }));
                    }
                }
            }
        }
    }
}
//******************************************************************************************renderowanie
void Game::render() {
    window.clear();
    if (inMenu) {
        sf::Vector2i mousePixel = sf::Mouse::getPosition(window);
        sf::Vector2f mousePos = window.mapPixelToCoords(mousePixel);
        menu.update(mousePos);
        menu.draw(window);
    }
    else {
        board.draw(window);

       
        HexTile::drawPorts(window);

        {
            std::lock_guard<std::mutex> lock(buildMutex);
            for (const auto& b : buildables)
                b->draw(window);
        }

        float logsPanelWidth = 500.f;
        float logsPanelX = 30.f;
        float logsPanelY = 30.f;
        logs.draw(window, logsPanelX, logsPanelY, logsPanelWidth);

        playerUI.drawAll(turnManager.getPlayers(), window);

        for (auto& btn : playerButtons) btn->draw(window);

        if (buildMode == BuildMode::Road || buildMode == BuildMode::Settlement) {
            for (const auto& btn : buildButtons)
                btn->draw(window);
        }

        sf::RectangleShape dice1(sf::Vector2f(60, 60));
        dice1.setPosition(static_cast<float>(window.getSize().x) - 180.f, 200.f);
        dice1.setFillColor(sf::Color(220, 220, 220));
        sf::RectangleShape dice2(sf::Vector2f(60, 60));
        dice2.setPosition(static_cast<float>(window.getSize().x) - 110.f, 200.f);
        dice2.setFillColor(sf::Color(220, 220, 220));
        window.draw(dice1);
        window.draw(dice2);

        sf::Text dice1Text;
        dice1Text.setFont(font);
        dice1Text.setCharacterSize(36);
        dice1Text.setFillColor(sf::Color::Black);
        dice1Text.setString(turnManager.getPlayers().empty() ? "" : std::to_string(turnManager.getCurrentPlayer().getDice1()));
        dice1Text.setPosition(dice1.getPosition().x + 18.f, dice1.getPosition().y + 10.f);
        window.draw(dice1Text);

        sf::Text dice2Text;
        dice2Text.setFont(font);
        dice2Text.setCharacterSize(36);
        dice2Text.setFillColor(sf::Color::Black);
        dice2Text.setString(turnManager.getPlayers().empty() ? "" : std::to_string(turnManager.getCurrentPlayer().getDice2()));
        dice2Text.setPosition(dice2.getPosition().x + 18.f, dice2.getPosition().y + 10.f);
        window.draw(dice2Text);

        sf::RectangleShape diceButton(sf::Vector2f(120, 50));
        diceButton.setPosition(static_cast<float>(window.getSize().x) - 180.f, 300.f);
        diceButton.setFillColor(sf::Color(60, 160, 60));
        diceButton.setOutlineColor(sf::Color::White);
        diceButton.setOutlineThickness(4.f);
        sf::Text diceButtonText("Rzut", font, 24);
        diceButtonText.setFillColor(sf::Color::White);
        diceButtonText.setStyle(sf::Text::Bold);
        diceButtonText.setPosition(diceButton.getPosition().x + 8.f, diceButton.getPosition().y + 10.f);
        window.draw(diceButton);
        window.draw(diceButtonText);

        cardManager.buyCardButton->draw(window);
        cardManager.showCardsButton->draw(window);
        sf::Vector2i mousePixel = sf::Mouse::getPosition(window);
        sf::Vector2f mousePos = window.mapPixelToCoords(mousePixel);

        knight.draw(window, board.getTiles(), hexSize);

        if (knightMoveMode) {
            for (const auto& btn : knightMoveButtons)
                btn->draw(window);
        }

        trade.draw(window);
		//*****************************************************************************************budowanie w setupie
        if (setupPhase) {
            players.push_back(bank);//BANK MUSI TU BYĆ DODANY PROSZE NIE KUSIĆ LOSU, wywala wektor bez niego
            sf::Text setupText;
            setupText.setFont(font);
            std::string msg;
            auto& playersNoBank = turnManager.getPlayers(); 
            msg = PLAYER_NICK(playersNoBank, setupPlayerIndex);
            if (setupStep == 0)
                msg += " stawia akademik";
            else
                msg += " stawia korytarz";
            setupText.setString(msg);
            setupText.setCharacterSize(40);
            setupText.setFillColor(sf::Color::Yellow);
            setupText.setStyle(sf::Text::Bold);
            setupText.setPosition(window.getSize().x / 2.f - setupText.getLocalBounds().width / 2.f, 10.f);
            window.draw(setupText);

         
            if (setupStep == 0 || setupStep == 1) {
                for (const auto& btn : buildButtons)
                    btn->draw(window);
            }
        }

        std::string buildMsg;
        if (playerButtons.size() > 0 && playerButtons[0]->isClicked(mousePos)) {
            buildMsg = "Budowanie korytarza: 1x Kawa, 1x Kabel";
        }
        else if (playerButtons.size() > 1 && playerButtons[1]->isClicked(mousePos)) {
            buildMsg = "Budowanie akademika: 1x Pizza, 1x Piwo, 1x Notatki";
        }
        else if (playerButtons.size() > 2 && playerButtons[2]->isClicked(mousePos)) {
            buildMsg = "Budowanie kampusu: 2x Kawa, 2x Piwo, 2x Pizza";
        }
        else if (cardManager.buyCardButton && cardManager.buyCardButton->isClicked(mousePos)) {
            buildMsg = "Zakup karty: 1x Kabel, 1x Piwo, 1x Notatki";
        }

        if (!buildMsg.empty()) {
            sf::Text buildInfo;
            buildInfo.setFont(font);
            buildInfo.setCharacterSize(28);
            buildInfo.setFillColor(sf::Color::Cyan);
            buildInfo.setStyle(sf::Text::Bold);
            buildInfo.setString(buildMsg);
            buildInfo.setPosition(30.f, 0.f);
            window.draw(buildInfo);
        }

		// wyświetlanie kart
        if (cardManager.cardsVisible && !turnManager.getPlayers().empty()) {
            int currentPlayerId = turnManager.getCurrentPlayer().getId();
            const auto& playerCardsMap = cardManager.getPlayerCards();
            auto it = playerCardsMap.find(currentPlayerId);
            if (it != playerCardsMap.end()) {
                const auto& cards = it->second;
                float cardPanelX = static_cast<float>(window.getSize().x) - 420.f;
                float cardPanelY = 400.f;
                float cardWidth = 400.f;
                float cardHeight = 40.f;
                int visibleIdx = 0; 
                for (const auto& card : cards | std::views::filter([](const auto& c) { return c->type != CardType::VictoryPoint; })) {
                    sf::RectangleShape cardRect(sf::Vector2f(cardWidth, cardHeight));
                    cardRect.setPosition(cardPanelX, cardPanelY + visibleIdx * (cardHeight + 10.f));
                    cardRect.setFillColor(sf::Color(60, 60, 120, 200));
                    cardRect.setOutlineColor(sf::Color::White);
                    cardRect.setOutlineThickness(2.f);
                    window.draw(cardRect);

                    sf::Text cardText;
                    cardText.setFont(font);
                    cardText.setString(card->getName());
                    cardText.setCharacterSize(24);
                    cardText.setFillColor(sf::Color::White);
                    cardText.setPosition(cardPanelX + 10.f, cardPanelY + visibleIdx * (cardHeight + 10.f) + 5.f);
                    window.draw(cardText);

                    ++visibleIdx;
                }
            }
        }
    }
    window.display();
}
//*****************************************************************************************przyciskoza
void Game::setupPlayerButtons() {
    playerButtons.clear();
    playerButtons.push_back(std::make_unique<SimpleButton>(font, "Buduj korytarz", sf::Vector2f(30, 350), [&]() {
        buildMode = BuildMode::Road;
        buildButtons.clear();
        auto& players = turnManager.getPlayers();
        int currentPlayer = turnManager.getCurrentPlayerIndex();
        initializeBuildButtons(buildButtons, buildables, board, hexSize, buildMode, players, currentPlayer, window);
    }));
    playerButtons.push_back(std::make_unique<SimpleButton>(font, "Buduj akademik", sf::Vector2f(30, 410), [&]() {
        buildMode = BuildMode::Settlement;
        buildButtons.clear();
        auto& players = turnManager.getPlayers();
        int currentPlayer = turnManager.getCurrentPlayerIndex();
        initializeBuildButtons(buildButtons, buildables, board, hexSize, buildMode, players, currentPlayer, window);
    }));
    playerButtons.push_back(std::make_unique<SimpleButton>(font, "Buduj kampus", sf::Vector2f(30, 470), [&]() {
        buildMode = BuildMode::City;
    }));
    playerButtons.push_back(std::make_unique<SimpleButton>(font, "Wymiana", sf::Vector2f(30, 530), [&]() {
        auto& players = turnManager.getPlayers();
        // Dodaj bank do wektora graczy na czas wymiany
        players.push_back(bank);
        int currentPlayer = turnManager.getCurrentPlayerIndex();
        trade.startTrade(font, players, currentPlayer, &logs);
    }));
    playerButtons.push_back(std::make_unique<SimpleButton>(font, "Kolejna tura", sf::Vector2f(30, 590), [&]() {
        if (!turnManager.getPlayers().empty() && !turnManager.getCurrentPlayer().hasRolled()) {
            std::string logMsg = players[turnManager.getCurrentPlayerIndex()].getNickname() + " najpierw rzuc kostka!";
            std::thread([this, logMsg]() {
                std::lock_guard<std::mutex> lock(logsMutex);
                logs.add(logMsg);
            }).detach();
            return;
        }
        turnManager.nextTurn();
        std::string logMsg = "Tura gracza: " + players[turnManager.getCurrentPlayerIndex()].getNickname();
        std::thread([this, logMsg]() {
            std::lock_guard<std::mutex> lock(logsMutex);
            logs.add(logMsg);
        }).detach();
        for (auto& player : turnManager.getPlayers()) {
            player.setUsedCardThisTurn(false);
        }

        buildMode = BuildMode::None;
        buildButtons.clear();
    }));

    cardManager.buyCardButton = std::make_unique<SimpleButton>(font, "Zakup karty", sf::Vector2f(30, 650), [&]() {
        if (!turnManager.getPlayers().empty()) {
            auto& player = turnManager.getCurrentPlayer();
            std::string cardName = cardManager.buyCardWithMessage(player);
            if (!cardName.empty()) {
                std::string logMsg = players[turnManager.getCurrentPlayerIndex()].getNickname() + " zakupil karte: " + cardName;
                std::thread([this, logMsg]() {
                    std::lock_guard<std::mutex> lock(logsMutex);
                    logs.add(logMsg);
                }).detach();
            }
        }
    });
    cardManager.showCardsButton = std::make_unique<SimpleButton>(font, "Zobacz karty", sf::Vector2f(30, 710), [this]() {
        cardManager.cardsVisible = !cardManager.cardsVisible;
    });
}