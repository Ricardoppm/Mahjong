//
//  GameBoard.cpp
//  Mahjong
//
//  Created by Ricardo Martins on 29/04/2017.
//  Copyright © 2017 Ricardo Martins. All rights reserved.
//

#include "GameBoard.hpp"

#include <Bengine/ResourceManager.hpp>
#include <Bengine/errors.hpp>

#include <fstream>
#include <sstream>
#include <iostream>

GameBoard::GameBoard()
{
}

GameBoard::~GameBoard()
{
}

void GameBoard::init(const glm::vec2 &tileDims, const std::string& filePath)
{
    tileDimensions_ = tileDims;
    
    // Load from file
    if( !loadFromFile(filePath)){
        Bengine::fatalError("Puzzle Setup FILE issue!\n");
    }
    
    createTiles();
}

void GameBoard::destroy()
{
}

void GameBoard::draw(Bengine::SpriteBatch &spriteBatch)
{
    for(auto& tile: tiles_)
        tile.draw(spriteBatch);
}

void GameBoard::drawDebug(Bengine::DebugRenderer& debugRenderer)
{
    // Draw horizontal Lines
    for(int i = 0; i <= (numTilesHeight_)*2 ; i++){
        debugRenderer.drawLine(glm::vec2( 0.0f, -(tileDimensions_.y * (float)i)/2.f ),
                               glm::vec2( numTilesWidth_ * tileDimensions_.x *2, -(tileDimensions_.y * (float)i))/2.f,
                               Bengine::ColorRGBA8(255, 0, 0, 200));
    }
  
    // Draw Vertical Lines
    for(int i = 0; i <= (numTilesWidth_)*2; i++){
        debugRenderer.drawLine(glm::vec2( (tileDimensions_.x * (float)i)/2.f , 0.f ),
                               glm::vec2( (tileDimensions_.x * (float)i)/2.f , -numTilesHeight_ * tileDimensions_.y),
                               Bengine::ColorRGBA8(0, 0, 255, 200));
    }
    
}

void GameBoard::update(Bengine::InputManager &inputManager)
{

}


// Private Methods

bool GameBoard::loadFromFile(const std::string &filePath)
{
    std::ifstream file;
    file.open(filePath);
    if( file.fail()){
        Bengine::fatalError("Failed to open level: " + filePath);
    }
    
    std::string temp;
    
    // get rows and cols
    file >> numTilesWidth_ >> numTilesHeight_;
    std::getline(file, temp); // Throw away the rest of the line
    
    if(numTilesWidth_ > TILE_MAX_WIDTH || numTilesHeight_ > TILE_MAX_HEIGHT){
        // Size too big
        return false;
    }
    
    board_.clear();
    board_.resize((numTilesWidth_*2)*(numTilesHeight_*2));
    
    // Read Board setup
    int line = 0;
    while(std::getline(file, temp)){
        if(line == (numTilesHeight_*2) || temp.size() > (numTilesWidth_*2)){
            // Line too long or too many lines
            return false;
        }
        for(size_t i = 0; i < temp.size(); i++){
            board_[(line*numTilesWidth_*2) + i] = temp[i] - '0';
        }
        line++;
    }
    
    // Print File
    for(int i = 0; i< board_.size() ; i++ ){
        if( i != 0 && i % (numTilesWidth_*2) == 0)
            std::cout << std::endl;
        std::cout << board_[i];
    }
    std::cout << std::endl;

    return true;
}

bool GameBoard::createTiles()
{
    std::vector<Uint8> level = board_;

    // Init texture
    Bengine::GLTexture texture_ = Bengine::ResourceManager::getTexture("Textures/618/fulltiles/fall.png");

    int tiles=0;
    
    for(size_t i = 0; i<= board_.size(); i++){
        glm::vec2 basePos;
        basePos.x = (float)(i % ( numTilesWidth_*2)) * tileDimensions_.x/2.f;
        basePos.y = -(float)((i / ( numTilesWidth_*2))+2) *  tileDimensions_.y/2.f;

        Uint8 towerSize = level[i];
        for(Uint8 numTiles = 0; numTiles < towerSize; numTiles++){
            std::cout << "Creating tile "<< numTiles << " of "<< board_[i] << std::endl;
            std::cout << (numTiles == board_[i]);
            std::cout << (numTiles < board_[i]);
            std::cout << (numTiles > board_[i]);

            // Create Tile
            Tile newTile;
            glm::vec2 pos;
            pos.x = basePos.x + PADDING*numTiles;
            pos.y = basePos.y + PADDING*numTiles;
            newTile.init(pos, tileDimensions_, texture_, Bengine::ColorRGBA8(255,255,255,255));
            
            tiles_.push_back(newTile);
            
            //Update level
            level[i]--;
            level[i + 1]--;
            level[i + 1 + (numTilesWidth_*2)]--;
            level[i + (numTilesWidth_*2)]--;
            
            tiles++;
        }
    }
    
    
    std::cout << tiles << std::endl;
    
    
    return true;
}
