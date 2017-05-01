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
#include <map>

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
    boardState_ = board_;

    createTiles();
}

void GameBoard::destroy()
{
}

void GameBoard::draw(Bengine::SpriteBatch &spriteBatch)
{
    for(auto& tile: tiles_)
        tile->draw(spriteBatch);
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

void GameBoard::update(Bengine::InputManager &inputManager, Bengine::Camera2D& camera)
{
    bool hasClickedTile = false;
    if( inputManager.isKeyPressed(SDL_BUTTON_LEFT)){
        glm::vec2 mouseCoords = camera.convertScreenToWorld( inputManager.getMouseCoords());
        for (auto tile : activeTiles_) {
            if( tile != selectedTile_ && tile->isClicked(mouseCoords)){
                hasClickedTile = true;
                //User clicked a valid Tile
                if( selectedTile_ == nullptr){
                    selectedTile_ = tile;
                    selectedTile_->setColor(Bengine::ColorRGBA8(225,255,225,255));
                }
                else{
                    // Compare to previously selected tile
                    if( selectedTile_->isSameTileType(tile)){
                        // Same Tile Type
                        glm::ivec3 coords = selectedTile_->getCoordinates();
                        removeTile(coords);
                        coords = tile->getCoordinates();
                        removeTile(coords);
                        selectedTile_ = nullptr;
                        calculatePairsAvailable();
                        break;
                    }
                    else{
                        selectedTile_->setColor(Bengine::ColorRGBA8(255,255,255,255));
                        selectedTile_ = nullptr;
                    }
                }
                break;
            }
        }
        if( !hasClickedTile && selectedTile_){
            selectedTile_->setColor(Bengine::ColorRGBA8(255,255,255,255));
            selectedTile_ = nullptr;
        }
        
    }
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
    loadTextureType(counter, "a", 9, 4, TileType::NUMERAL);
    // bamboos
    loadTextureType(counter, "b", 9, 4, TileType::BAMBOO);
    // circles
    loadTextureType(counter, "c", 9, 4, TileType::CIRCLE);
    // winds
    loadTextureType(counter, "w", 4, 4, TileType::WIND);
    // dragons
    loadTextureType(counter, "d", 3, 4, TileType::DRAGON);
    // flowers
    loadTextureType(counter, "f", 4, 1, TileType::FLOWER);
    // seasons
    loadTextureType(counter, "s", 4, 1, TileType::SEASON);
}

void GameBoard::loadTextureType(std::vector<TileTexture>& counter, const std::string &type, int max, int numTiles, TileType tileType)
{
    static const std::string dirPath = "Textures/128/fulltiles/";
    
    for( int i = 1; i <= max; i++){
        std::string file = dirPath + type + std::to_string(i) + std::string(".png");
        TileTexture newTileTexture;
        newTileTexture.counter = numTiles;
        newTileTexture.texture = Bengine::ResourceManager::getTexture(file);
        newTileTexture.type = tileType;
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
    tiles_.reserve(144);
    activeTiles_.reserve(144);
    
    glm::vec3 tileDimensions;
    tileDimensions.x = tileDimensions_.x;
    tileDimensions.y = tileDimensions_.y;
    tileDimensions.z = PADDING;


    for(int height = 0; height < 5; height++){
        for (int y = 0; y < (numTilesHeight_*2); y++) {

        for (int x = (numTilesWidth_*2)-1; x >=0; x--) {
                int index = (y*(numTilesWidth_*2) + x);
                if( level[index] > 0 && board_[index] == (level[index]+height)){
                    glm::vec2 pos;
                    pos.x = (float)(x-1) * drawDimensions_.x/2.f + PADDING*height;
                    pos.y = -(float)(y+2) *  drawDimensions_.y/2.f + PADDING*height;
                    // Create Tile
                    Tile* newTile = new Tile();
                    // Select Texture
                    int textureIndex = rand() % counter.size();
                    
                    newTile->init(pos,
                                 tileDimensions,
                                 glm::ivec3(x,y,height) ,
                                 counter[textureIndex],
                                 Bengine::ColorRGBA8(255,255,255,255),
                                 (float)(-height / 5.f) + (0.01f * (float)x) + (float)(-0.0001f * (y/2)) );
                    tiles_.push_back(newTile);

                    // Update Texture Counter
                    counter[textureIndex].counter--;
                    if(counter[textureIndex].counter == 0){
                        counter[textureIndex] = counter.back();
                        counter.pop_back();
                    }
                    
                    //Update level
                    level[index]--;
                    level[index - 1]--;
                    level[index - 1 + (numTilesWidth_*2)]--;
                    level[index + (numTilesWidth_*2)]--;

                    if (!level[index] &&
                        !level[index - 1] &&
                        !level[index - 1 + (numTilesWidth_*2)] &&
                        !level[index + (numTilesWidth_*2)]) {
                        // There is no other tile above
                        activeTiles_.push_back(newTile);
                    }
                }
            }
        }
    }
    
    for( int i = 0; i< activeTiles_.size();){
        if(!isTileActive(activeTiles_[i]->getCoordinates())){
            // Tile has neighbours on both sides
            // Remove from active pile
            activeTiles_[i] = activeTiles_.back();
            activeTiles_.pop_back();
        }
        else{
            activeTiles_[i]->setActive(true);
            i++;
        }
    }
    
    calculatePairsAvailable();

    
    return true;
}

void GameBoard::removeTile(const glm::ivec3& coords)
{
    int tileIndex = coords.y * (numTilesWidth_*2) + coords.x;
    
    //Update boardState
    boardState_[tileIndex]--;
    boardState_[tileIndex - 1]--;
    boardState_[tileIndex - 1 + (numTilesWidth_*2)]--;
    boardState_[tileIndex + (numTilesWidth_*2)]--;
    
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
            if( !tiles_[i]->isActive() &&
                boardState_[tileIndex] == height &&
                boardState_[tileIndex - 1] == height &&
                boardState_[tileIndex - 1 + (numTilesWidth_*2)] == height &&
                boardState_[tileIndex + (numTilesWidth_*2)] == height &&
                isTileActive(tileCoord)){
                // Tile is unblocked
                // Add it to the active pile
                activeTiles_.push_back(tiles_[i]);
                tiles_[i]->setActive(true);
            }
            i++;
        }
    }
}


bool GameBoard::isTileActive(const glm::ivec3 &coords)
{
    static const int LEFT_BORDER_INDEX = 1;
    static const int RIGHT_BORDER_INDEX = (numTilesWidth_*2)-1;

    int tileIndex = coords.y * (numTilesWidth_*2) + coords.x;
    Uint8 height = coords.z + 1;
    if( (coords.x == LEFT_BORDER_INDEX || coords.x == RIGHT_BORDER_INDEX) && boardState_[tileIndex] == height ){
        // TOP Border Tiles are always active
        return true;
    }
    
    // Check if it has a left neighbour with lower height
    if( boardState_[tileIndex - 2] < height &&
        boardState_[tileIndex - 2 + (numTilesWidth_*2)] < height){
        return true;
    }
    
    // Check if it has a left neighbour with lower height
    if( boardState_[tileIndex + 1] < height &&
        boardState_[tileIndex + 1 + (numTilesWidth_*2)] < height){
        return true;
    }
    return false;
}

void GameBoard::calculatePairsAvailable()
{
    std::map<int, int> map;
    
    // Make map
    for (auto tile: activeTiles_) {
        int textureID =tile->getTextureId();
        auto it = map.find(textureID);
        if( it == map.end()){
            map[textureID] = 1;
        }
        else{
            map[textureID]++;
        }
    }
    
    numPairsAvailable = 0;
    // Sum available pairs
    for(auto it = map.begin(); it != map.end(); it++){
        numPairsAvailable += it->second/2;
    }
}
