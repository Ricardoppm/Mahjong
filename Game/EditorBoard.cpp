//
//  EditorBoard.cpp
//  Mahjong
//
//  Created by Ricardo Martins on 05/05/2017.
//  Copyright © 2017 Ricardo Martins. All rights reserved.
//

#include "EditorBoard.hpp"


#include <Bengine/ResourceManager.hpp>
#include <Bengine/errors.hpp>

#include <fstream>
#include <sstream>
#include <iostream>
#include <map>

EditorBoard::EditorBoard()
{
    // Empty
}

EditorBoard::~EditorBoard()
{
}

void EditorBoard::init(const glm::vec2 &tileDims)
{
    tileDimensions_ = tileDims;
    drawDimensions_ = tileDims - PADDING;
    
    numTilesWidth_ = TILE_MAX_WIDTH;
    numTilesHeight_ = TILE_MAX_HEIGHT;
    
    // Load texture
    texture_.texture = Bengine::ResourceManager::getTexture("Textures/128/tile.png");
}

void EditorBoard::destroy()
{
}

void EditorBoard::draw(Bengine::SpriteBatch &spriteBatch)
{
    for(auto& tile: tiles_)
        tile->draw(spriteBatch);
}

void EditorBoard::drawDebug(Bengine::DebugRenderer& debugRenderer)
{
    // Draw horizontal Lines
    for(int i = 0; i <= (numTilesHeight_)*2 ; i++){
        debugRenderer.drawLine(glm::vec2( 0.0f, -(drawDimensions_.y * (float)i)/2.f ),
                               glm::vec2( numTilesWidth_ * drawDimensions_.x *2, -(drawDimensions_.y * (float)i))/2.f,
                               Bengine::ColorRGBA8(255, 0, 0, 200));
    }
    
    // Draw Vertical Lines
    for(int i = 0; i <= (numTilesWidth_)*2; i++){
        debugRenderer.drawLine(glm::vec2( (drawDimensions_.x * (float)i)/2.f , 0.f ),
                               glm::vec2( (drawDimensions_.x * (float)i)/2.f , -numTilesHeight_ * drawDimensions_.y),
                               Bengine::ColorRGBA8(0, 0, 255, 200));
    }
    
}

void EditorBoard::update(Bengine::InputManager &inputManager, Bengine::Camera2D& camera)
{
    bool hasClickedTile = false;
    if( inputManager.isKeyPressed(SDL_BUTTON_LEFT)){
        glm::vec2 mouseCoords = camera.convertScreenToWorld( inputManager.getMouseCoords());
        
        if( mouseCoords.x > 0.f && mouseCoords.x < TILE_DIM_WIDTH*TILE_MAX_WIDTH*2 &&
           mouseCoords.y < 0.f && mouseCoords.y < TILE_DIM_HEIGHT*TILE_MAX_HEIGHT*2){
            int x = (int)mouseCoords.x / (TILE_DIM_WIDTH/2);
            int y = -(int)mouseCoords.y / (TILE_DIM_HEIGHT/2);
            
            glm::vec3 tileDimensions;
            tileDimensions.x = tileDimensions_.x;
            tileDimensions.y = tileDimensions_.y;
            tileDimensions.z = PADDING;
            
            Tile* newTile = new Tile();
            newTile->init(glm::vec2(x*(TILE_DIM_WIDTH/2), -(y+2)*(TILE_DIM_HEIGHT/2)),
                         tileDimensions,
                         glm::ivec3(x,y,0),
                         texture_,
                         Bengine::ColorRGBA8(255,255,255,255),
                         0.f);
            
            tiles_.push_back(newTile);
        }
        
    }
}

void EditorBoard::restart()
{
    // Clear tile vectors
    activeTiles_.clear();
    tiles_.clear();
}


// Private Methods

void EditorBoard::removeTile(const glm::ivec3& coords)
{
    int tileIndex = coords.y * (numTilesWidth_*2) + coords.x;
    
    //Update boardState
    
    for (int i = 0; i < tiles_.size();) {
        glm::ivec3 tileCoord = tiles_[i]->getCoordinates();
        if( tileCoord.x == coords.x && tileCoord.y == coords.y && tileCoord.z == coords.z){
            // Found tile to be removed
            delete tiles_[i];
            tiles_[i] = tiles_.back();
            tileCoord = tiles_[i]->getCoordinates();
            tiles_.pop_back();
        }
        else{
            tileIndex = tileCoord.y * (numTilesWidth_*2) + tileCoord.x;
            Uint8 height = tileCoord.z + 1;
            if( !tiles_[i]->isActive() ){
                // MISSING CONDITIONS!!!
                
                
                // Tile is unblocked
                // Add it to the active pile
                activeTiles_.push_back(tiles_[i]);
                tiles_[i]->setActive(true);
            }
            i++;
        }
    }
}
