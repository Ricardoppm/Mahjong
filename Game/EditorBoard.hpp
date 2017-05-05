//
//  EditorBoard.hpp
//  Mahjong
//
//  Created by Ricardo Martins on 05/05/2017.
//  Copyright © 2017 Ricardo Martins. All rights reserved.
//

#ifndef EditorBoard_hpp
#define EditorBoard_hpp


#include <glm/glm.hpp>

#include <Bengine/SpriteBatch.hpp>
#include <Bengine/InputManager.hpp>
#include <Bengine/DebugRenderer.hpp>
#include <Bengine/GLTexture.h>
#include <Bengine/Camera2D.hpp>

#include <SDL2/SDL_events.h>

#include "Tile.hpp"
#include "GameDimensions.h"



class EditorBoard
{    
public:
    EditorBoard();
    ~EditorBoard();
    
    void init(const glm::vec2& tileDims);
    void destroy();
    
    void draw(Bengine::SpriteBatch& spriteBatch);
    void drawDebug(Bengine::DebugRenderer& debugRenderer);
    
    void update(Bengine::InputManager& inputManager, Bengine::Camera2D& camera);
    
    void restart();
    
    // Getters
    const int       getNumTilesWidth() const        { return numTilesWidth_;  }
    const int       getNumTilesHeight() const       { return numTilesHeight_; }
    const size_t    getTilesRemaining() const       { return tiles_.size();   }
    const int       getNumPairsAvailable() const    { return numPairsAvailable; }
    
private:
    bool createTiles();
    
    void removeTile(const glm::ivec3& coords);
    
    glm::vec2 drawDimensions_;
    glm::vec2 tileDimensions_;
    
    int numTilesWidth_;
    int numTilesHeight_;
    
    // Holds the level setup
    std::vector<Uint8> board_;
    
    // Holds all tiles
    std::vector<Tile*> tiles_;
    std::vector<Tile*> activeTiles_;
    
    int numPairsAvailable;
    
    TileTexture texture_;
    
    
    
};

#endif /* EditorBoard_hpp */
