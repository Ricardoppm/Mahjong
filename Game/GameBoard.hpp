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
#include <Bengine/GLTexture.h>

#include <SDL2/SDL_events.h>

#include "Tile.hpp"

const int TILE_MAX_WIDTH = 18;
const int TILE_MAX_HEIGHT = 12;

const float PADDING = 1.2f;

struct TileTexture {
    Bengine::GLTexture texture;
    int counter = 0;
};


class GameBoard
{
public:
    GameBoard();
    ~GameBoard();
    
    void init(const glm::vec2& tileDims, const std::string& filePath);
    void destroy();
    
    void draw(Bengine::SpriteBatch& spriteBatch);
    void drawDebug(Bengine::DebugRenderer& debugRenderer);
    
    void update(Bengine::InputManager& inputManager);
    
    
    // Getters
    const int       getNumTilesWidth() const        { return numTilesWidth_;  }
    const int       getNumTilesHeight() const       { return numTilesHeight_; }

private:
    bool loadFromFile(const std::string& filePath);
    void loadTileTextures(std::vector<TileTexture>& counter);
    void loadTextureType(std::vector<TileTexture>& counter, const std::string& type, int max, int numTiles);
    bool createTiles();
    
    glm::vec2 drawDimensions_;
    glm::vec2 tileDimensions_;
    
    int numTilesWidth_;
    int numTilesHeight_;
    
    Tile tile_;
    
    // Holds the level setup
    std::vector<Uint8> board_;
    
    // Holds all tiles
    std::vector<Tile> tiles_;
};
#endif /* GameBoard_hpp */
