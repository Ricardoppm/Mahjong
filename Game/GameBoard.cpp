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
    drawDimensions_ = tileDims - PADDING;
    
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

void GameBoard::loadTileTextures(std::vector<TileTexture> &counter)
{
    counter.reserve(42);
    
    // numerals
    loadTextureType(counter, "a", 9, 4);
    // bamboos
    loadTextureType(counter, "b", 9, 4);
    // circles
    loadTextureType(counter, "c", 9, 4);
    // winds
    loadTextureType(counter, "w", 4, 4);
    // dragons
    loadTextureType(counter, "d", 3, 4);
    // flowers
    loadTextureType(counter, "f", 4, 1);
    // seasons
    loadTextureType(counter, "s", 4, 1);
}

void GameBoard::loadTextureType(std::vector<TileTexture>& counter, const std::string &type, int max, int numTiles)
{
    static const std::string dirPath = "Textures/128/fulltiles/";
    
    for( int i = 1; i <= max; i++){
        std::string file = dirPath + type + std::to_string(i) + std::string(".png");
        TileTexture newTileTexture;
        newTileTexture.counter = numTiles;
        newTileTexture.texture = Bengine::ResourceManager::getTexture(file);
        counter.push_back(newTileTexture);
    }
}

bool GameBoard::createTiles()
{
    // initialize random seed
    srand (time(NULL));
    
    std::vector<TileTexture> counter;
    loadTileTextures(counter);
    
    std::vector<Uint8> level = board_;

    // Resize to hold all tiles
    tiles_.resize(144);
    
    Bengine::GLTexture texture1_ = Bengine::ResourceManager::getTexture("Textures/128/fulltiles/a1.png");
    Bengine::GLTexture texture2_ = Bengine::ResourceManager::getTexture("Textures/128/fulltiles/b1.png");

    
    int numTiles = 0;
    
    for(int height = 0; height < 5; height++){
        for (int x = (numTilesWidth_*2)-1; x >=0; x--) {
            for (int y = 0; y < (numTilesHeight_*2); y++) {
                int index = (y*(numTilesWidth_*2) + x);
                if( level[index] > 0 && board_[index] == (level[index]+height)){
                    glm::vec2 pos;
                    pos.x = (float)(x-1) * drawDimensions_.x/2.f + PADDING*height;
                    pos.y = -(float)(y+2) *  drawDimensions_.y/2.f + PADDING*height;
                    
                    std::cout << "Creating tile on ("<<  y << "," << x << ")\n";
                    // Create Tile
                    Tile newTile;
                    // Select Texture
                    int textureIndex = rand() % counter.size();
                
                    if(rand() % 2 == 1)
                        newTile.init(pos, tileDimensions_, texture1_,Bengine::ColorRGBA8(255,255,255,255),1.f/(float)x);
                    else
                        newTile.init(pos, tileDimensions_, texture2_,Bengine::ColorRGBA8(255,255,255,255),1.f/(float)x);

                    
                    
                    tiles_.push_back(newTile);
                    
                    //Update level
                    level[index]--;
                    level[index - 1]--;
                    level[index - 1 + (numTilesWidth_*2)]--;
                    level[index + (numTilesWidth_*2)]--;
                
                    numTiles++;
                }
            }
        }
    }
    
    
    return true;
}

