//
//  GameBoard.hpp
//  Mahjong
//
//  Created by Ricardo Martins on 29/04/2017.
//  Copyright © 2017 Ricardo Martins. All rights reserved.
//

#ifndef GameBoard_hpp
#define GameBoard_hpp

#include <glm/glm.hpp>

#include <Bengine/SpriteBatch.hpp>
#include <Bengine/InputManager.hpp>
#include <Bengine/DebugRenderer.hpp>

#include <SDL2/SDL_events.h>

#include "Tile.hpp"

const int TILE_MAX_WIDTH = 18;
const int TILE_MAX_HEIGHT = 12;

class GameBoard
{
public:
    GameBoard();
    ~GameBoard();
    
    void init(const glm::vec2& tileDims);
    void destroy();
    
    void draw(Bengine::SpriteBatch& spriteBatch);
    void drawDebug(Bengine::DebugRenderer& debugRenderer);
    
    void update(Bengine::InputManager& inputManager);
    
    void loadFromFile(const std::string& filePath);
    
    // Getters
    const int       getNumTilesWidth() const        { return numTilesWidth_;  }
    const int       getNumTilesHeight() const       { return numTilesHeight_; }

private:
    glm::vec2 tileDimensions_;
    int numTilesWidth_;
    int numTilesHeight_;
    
    Tile tile_;
};
#endif /* GameBoard_hpp */
