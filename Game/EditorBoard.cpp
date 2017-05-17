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

#include "LevelReaderWriter.hpp"

EditorBoard::EditorBoard()
{
    // Empty
}

EditorBoard::~EditorBoard()
{
    destroy();
}

void EditorBoard::init(const glm::vec2 &tileDims)
{
    tileDimensions_ = tileDims;
    drawDimensions_ = tileDims - PADDING;
    
    numTilesWidth_ = TILE_MAX_WIDTH;
    numTilesHeight_ = TILE_MAX_HEIGHT;
    
    // Load texture
    texture_.texture = Bengine::ResourceManager::getTexture("Textures/128/tile.png");
    
    // Create empty board
    board_.resize(TILE_MAX_WIDTH*TILE_MAX_HEIGHT*4, -1);
}

void EditorBoard::destroy()
{
    for (size_t i = 0; i < tiles_.size(); i++) {
        delete tiles_[i];
    }
    tiles_.clear();
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
                               Bengine::ColorRGBA8(255, 255, 255, 255));//Bengine::ColorRGBA8(255, 0, 0, 200));
    }
    
    // Draw Vertical Lines
    for(int i = 0; i <= (numTilesWidth_)*2; i++){
        debugRenderer.drawLine(glm::vec2( (drawDimensions_.x * (float)i)/2.f , 0.f ),
                               glm::vec2( (drawDimensions_.x * (float)i)/2.f , -numTilesHeight_ * drawDimensions_.y),
                               Bengine::ColorRGBA8(255, 255, 255, 255));//Bengine::ColorRGBA8(0, 0, 255, 200));
    }
    
}

void EditorBoard::update(Bengine::InputManager &inputManager, Bengine::Camera2D& camera)
{
    // Improve this code, too repetitive
    
    bool hasClickedTile = false;
    if( inputManager.isKeyPressed(SDL_BUTTON_LEFT) && tiles_.size()!=TOTAL_TILES){
        glm::vec2 mouseCoords = camera.convertScreenToWorld( inputManager.getMouseCoords());
        
        Tile* clickedTile = nullptr;
        for (auto tile : tiles_) {
            if(tile->isClicked(mouseCoords)){
                // check if clicked tile is higher than the current one
                if( clickedTile == nullptr || tile->getCoordinates().z > clickedTile->getCoordinates().z){
                    clickedTile = tile;
                }
            }
        }
        
        if(clickedTile != nullptr){
            // User clicked on an existing Tile
            // Verify which quadrant was clicked
            glm::vec2 tilePos = clickedTile->getPosition();
            glm::ivec3 tileCoords = clickedTile->getCoordinates();
            glm::ivec3 mouseIndex;
            if( mouseCoords.x < tilePos.x + PADDING + drawDimensions_.x/2.f){
                // Left Side
                if(mouseCoords.y > tilePos.y + PADDING + drawDimensions_.y/2.f){
                    // Top Left
                    mouseIndex.x = tileCoords.x;
                    mouseIndex.y = tileCoords.y;
                }
                else{
                    // Bottom Left
                    mouseIndex.x = tileCoords.x;
                    mouseIndex.y = tileCoords.y+1;
                }
            }
            else {
                // Right Side
                if(mouseCoords.y > tilePos.y + PADDING + drawDimensions_.y/2.f){
                    // Top Right
                    mouseIndex.x = tileCoords.x+1;
                    mouseIndex.y = tileCoords.y;
                }
                else{
                    // Bottom Right
                    mouseIndex.x = tileCoords.x+1;
                    mouseIndex.y = tileCoords.y+1;
                }
            }
            mouseIndex.z = tileCoords.z;
            
            // Check is tile is being created on a plane surface
            int tileIndex = (mouseIndex.y * numTilesWidth_*2) + mouseIndex.x;
            if( board_[tileIndex] == mouseIndex.z &&
                board_[tileIndex + 1] == mouseIndex.z &&
                board_[tileIndex + (numTilesWidth_*2)] == mouseIndex.z &&
                board_[tileIndex + (numTilesWidth_*2) + 1] == mouseIndex.z){
                // Create Tile
                glm::vec3 tileDimensions;
                tileDimensions.x = tileDimensions_.x;
                tileDimensions.y = tileDimensions_.y;
                tileDimensions.z = PADDING;
                
                // place tile one higher
                mouseIndex.z++;

                Tile* newTile = new Tile();
                float depth = (float)(-(mouseIndex.z) / 5.f) + (0.01f * (float)(mouseIndex.x/2)) - (0.01f * (float)(mouseIndex.y/2)) - (0.0001f * (float)mouseIndex.y) + (0.0001f * (float)mouseIndex.x);
                glm::vec2 pos;
                pos.x = (float)(mouseIndex.x) * drawDimensions_.x/2.f + PADDING*mouseIndex.z;
                pos.y = -(float)(mouseIndex.y+2) *  drawDimensions_.y/2.f + PADDING*mouseIndex.z;
                newTile->init(pos,
                              tileDimensions,
                              glm::ivec3(mouseIndex.x,mouseIndex.y,mouseIndex.z),
                              texture_,
                              Bengine::ColorRGBA8(255,255,255,255),
                              depth);
                tiles_.push_back(newTile);
                
                // Update board
                board_[tileIndex]++;
                board_[tileIndex + 1]++;
                board_[tileIndex + (numTilesWidth_*2)]++;
                board_[tileIndex + (numTilesWidth_*2) + 1]++;
            }
            
            
        }
        else{
            // User didn't click on tile. See if he clicked on gri
            if( mouseCoords.x > 0.f && mouseCoords.x < drawDimensions_.x*TILE_MAX_WIDTH &&
               mouseCoords.y < 0.f && mouseCoords.y < drawDimensions_.y*TILE_MAX_HEIGHT){
                // Insert tile on grid position clicked
                int x = (int)mouseCoords.x / (drawDimensions_.x /2);
                int y = -(int)mouseCoords.y / (drawDimensions_.y/2);
                
                // Check is tile is being created on a plane surface
                int tileIndex = (y * numTilesWidth_*2) + x;
                uint8_t floor = -1;
                if( board_[tileIndex] == floor &&
                   board_[tileIndex + 1] == floor &&
                   board_[tileIndex + (numTilesWidth_*2)] == floor &&
                   board_[tileIndex + (numTilesWidth_*2) + 1] == floor){
                
                    glm::vec3 tileDimensions;
                    tileDimensions.x = tileDimensions_.x;
                    tileDimensions.y = tileDimensions_.y;
                    tileDimensions.z = PADDING;
                    
                    float depth = (0.01f * (float)(x/2)) - (0.01f * (float)(y/2)) - (0.0001f * (float)y) + (0.0001f * (float)x);
                    Tile* newTile = new Tile();
                    newTile->init(glm::vec2(x*(drawDimensions_.x/2), -(y+2)*(drawDimensions_.y/2)),
                                 tileDimensions,
                                 glm::ivec3(x,y,0),
                                 texture_,
                                 Bengine::ColorRGBA8(255,255,255,255),
                                 depth);
                    
                    tiles_.push_back(newTile);
                    
                    // Update board
                    board_[tileIndex]++;
                    board_[tileIndex + 1]++;
                    board_[tileIndex + (numTilesWidth_*2)]++;
                    board_[tileIndex + (numTilesWidth_*2) + 1]++;
                }
            }
        }
    }
    else if( inputManager.isKeyPressed(SDL_BUTTON_RIGHT) && tiles_.size()){
        glm::vec2 mouseCoords = camera.convertScreenToWorld( inputManager.getMouseCoords());
        
        int clickedTileIndex = -1;
        for(size_t i = 0; i < tiles_.size(); i++){
            if(tiles_[i]->isClicked(mouseCoords)){
                // check if clicked tile is higher than the current one
                if( clickedTileIndex == -1 || tiles_[i]->getCoordinates().z > tiles_[clickedTileIndex]->getCoordinates().z){
                    clickedTileIndex = (int)i;
                }
            }
        }
        if (clickedTileIndex != -1) {
            // User clicked a Tile
            glm::ivec3 tileCoords = tiles_[clickedTileIndex]->getCoordinates();
            int tileIndex = (tileCoords.y * numTilesWidth_*2) + tileCoords.x;

            if( board_[tileIndex] == tileCoords.z &&
               board_[tileIndex + 1] == tileCoords.z &&
               board_[tileIndex + (numTilesWidth_*2)] == tileCoords.z &&
               board_[tileIndex + (numTilesWidth_*2) + 1] == tileCoords.z){
                // Clicked tile has nothing above it -> remove
                delete tiles_[clickedTileIndex];
                tiles_[clickedTileIndex] = tiles_.back();
                tiles_.pop_back();
                
                // Update board
                board_[tileIndex]--;
                board_[tileIndex + 1]--;
                board_[tileIndex + (numTilesWidth_*2)]--;
                board_[tileIndex + (numTilesWidth_*2) + 1]--;
            }
        }
        
    }
}

void EditorBoard::restart()
{
    std::fill(board_.begin(), board_.end(), (uint8_t)-1);
    
    // Clear tile
    destroy();
}

bool EditorBoard::saveBoard(const std::string &filePath)
{
    int left ,right,top,bottom = -1;
    
    for (auto tile : tiles_) {
        glm::ivec3 tileCoords = tile->getCoordinates();
        if(bottom==-1){
            bottom = tileCoords.y;
            top = bottom;
            left = tileCoords.x;
            right = left;
        }
        else{
            if( tileCoords.x < left)
                left = tileCoords.x;
            if( tileCoords.x > right)
                right = tileCoords.x;
            if( tileCoords.y < bottom)
                bottom = tileCoords.y;
            if( tileCoords.y > top)
                top = tileCoords.y;
        }
    }

    return  LevelReaderWriter::saveAsText(filePath, board_, glm::ivec4(left,right,bottom,top), TILE_MAX_WIDTH);

}

bool EditorBoard::loadFromFile(const std::string &filePath)
{
    std::ifstream file;
    file.open(filePath);
    if( file.fail()){
        Bengine::fatalError("Failed to open level: " + filePath);
    }
    
    std::string temp;
    
    int width, heigth;
    
    // get rows and cols
    file >> width >> heigth;
    std::getline(file, temp); // Throw away the rest of the line
    
    // Check file dimensions
    if(width > TILE_MAX_WIDTH || heigth > TILE_MAX_HEIGHT){
        // Size too big
        return false;
    }
    
    int xoffset =(2*(TILE_MAX_WIDTH-width))/2;
    int yoffset =(2*(TILE_MAX_HEIGHT-heigth))/2;

    restart();
    
    // Read Board setup
    int line = 0;
    while(std::getline(file, temp)){
        if(line == (TILE_MAX_HEIGHT*2) || temp.size() > (TILE_MAX_WIDTH*2)){
            // Line too long or too many lines
            return false;
        }
        for(size_t i = 0; i < temp.size(); i++){
            board_[( (line+yoffset)*numTilesWidth_*2) + i + xoffset] = (uint8_t)(temp[i] - '1');
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
    
    createTiles();
    return true;
}


// Private Methods

bool EditorBoard::createTiles()
{
    std::vector<uint8_t> level = board_;
    
    // Resize to hold all tiles
    tiles_.reserve(144);
    
    glm::vec3 tileDimensions;
    tileDimensions.x = tileDimensions_.x;
    tileDimensions.y = tileDimensions_.y;
    tileDimensions.z = PADDING;
    
    
    for(int height = 0; height < 5; height++){
        for (int y = 0; y < (numTilesHeight_*2); y++) {
            
            for (int x = 0; x < (numTilesWidth_*2)-1; x++) {
                int index = (y*(numTilesWidth_*2) + x);
                if( level[index] >= 0 && level[index]<=TILE_MAX_LENGTH && board_[index] == (level[index]+height)){
                    glm::vec2 pos;
                    pos.x = (float)(x) * drawDimensions_.x/2.f + PADDING*height;
                    pos.y = -(float)(y+2) *  drawDimensions_.y/2.f + PADDING*height;
                    
                    // Create Tile
                    Tile* newTile = new Tile();
                   
                    float depth = (float)(-(height) / 5.f) + (0.01f * (float)(x/2)) - (0.01f * (float)(y/2)) - (0.0001f * (float)y) + (0.0001f * (float)x);
                    newTile->init(pos,
                                  tileDimensions,
                                  glm::ivec3(x,y,height) ,
                                  texture_,
                                  Bengine::ColorRGBA8(255,255,255,255),
                                  depth);
                    tiles_.push_back(newTile);
                    
                    //Update level
                    level[index]--;
                    level[index + 1]--;
                    level[index + 1 + (numTilesWidth_*2)]--;
                    level[index + (numTilesWidth_*2)]--;
                }
            }
        }
    }
    
    
    return true;
}


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
    }
}
