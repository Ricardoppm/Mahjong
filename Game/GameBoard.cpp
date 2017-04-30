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

void GameBoard::init(const glm::vec2 &tileDims)
{
    tileDimensions_ = tileDims;
    
    // Init texture
    Bengine::GLTexture texture_ = Bengine::ResourceManager::getTexture("Textures/618/fulltiles/fall.png");
    tile_.init(glm::vec2(0.f,0.f), tileDimensions_, texture_, Bengine::ColorRGBA8(255,255,255,255));
}

void GameBoard::destroy()
{
}

void GameBoard::draw(Bengine::SpriteBatch &spriteBatch)
{
    tile_.draw(spriteBatch);
}

void GameBoard::drawDebug(Bengine::DebugRenderer& debugRenderer)
{
    // Draw horizontal Lines
    for(int i = 0; i <= (numTilesHeight_)*2 ; i++){
        debugRenderer.drawLine(glm::vec2( 0.0f, (tileDimensions_.y * (float)i)/2.f ),
                               glm::vec2( numTilesWidth_ * tileDimensions_.x *2, (tileDimensions_.y * (float)i))/2.f,
                               Bengine::ColorRGBA8(255, 0, 0, 200));
    }
  
    // Draw Vertical Lines
    for(int i = 0; i <= (numTilesWidth_)*2; i++){
        debugRenderer.drawLine(glm::vec2( (tileDimensions_.x * (float)i)/2.f , 0.f ),
                               glm::vec2( (tileDimensions_.x * (float)i)/2.f , numTilesHeight_ * tileDimensions_.y),
                               Bengine::ColorRGBA8(0, 0, 255, 200));
    }
    
}

void GameBoard::update(Bengine::InputManager &inputManager)
{

}

void GameBoard::loadFromFile(const std::string &filePath)
{
    std::ifstream file;
    file.open(filePath);
    if( file.fail()){
        Bengine::fatalError("Failed to open level: " + filePath);
    }
    
    std::string temp;
    
    // get number of humans
    file >> numTilesWidth_ >> numTilesHeight_;
    std::getline(file, temp); // Throw away the rest of the line
    
    std::stringstream ss;
    int xPos, yPos, height;
    // Read Tiles
    while(std::getline(file, temp)){
        // Each line has position and "tower" size
        ss.clear();
        ss.str(temp);
        ss >> xPos >> yPos >> height;
        
        std::cout << "Tile on (" << xPos << "," << yPos << ") with " << height << " tiles\n";
    }
    

}
