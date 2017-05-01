//
//  Tile.hpp
//  Mahjong
//
//  Created by Ricardo Martins on 29/04/2017.
//  Copyright © 2017 Ricardo Martins. All rights reserved.
//

#ifndef Tile_hpp
#define Tile_hpp

#include <glm/glm.hpp>

#include <Bengine/Vertex.h>
#include <Bengine/GLTexture.h>
#include <Bengine/SpriteBatch.hpp>

class Tile
{
public:
    Tile();
    ~Tile();
    
    void init(const glm::vec2& pos,
              const glm::vec2& dims,
              Bengine::GLTexture texture,
              Bengine::ColorRGBA8 color,
              Tile* below = nullptr);
    
    void draw(Bengine::SpriteBatch& spriteBatch);

    // Getters
    Tile* getBelow() const { return tileBelow_; }
    
private:
    glm::vec2 position_;
    glm::vec2 dimensions_;
    
    Bengine::ColorRGBA8 color_;
    Bengine::GLTexture texture_;
    
    Tile* tileBelow_ = nullptr;

};
#endif /* Tile_hpp */
