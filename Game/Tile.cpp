//
//  Tile.cpp
//  Mahjong
//
//  Created by Ricardo Martins on 29/04/2017.
//  Copyright © 2017 Ricardo Martins. All rights reserved.
//

#include "Tile.hpp"

Tile::Tile()
{
}

Tile::~Tile()
{
}

void Tile::init(const glm::vec2 &pos,
                const glm::vec2 &dims,
                Bengine::GLTexture texture,
                Bengine::ColorRGBA8 color,
                float depth,
                Tile* below)
{
    position_ = pos;
    dimensions_ = dims;
    color_ = color;
    texture_ = texture;
    depth_ = depth;
    tileBelow_ = below;
}

void Tile::draw(Bengine::SpriteBatch &spriteBatch)
{
    glm::vec4 destRect(position_.x,
                       position_.y,
                       dimensions_.x,
                       dimensions_.y);
    spriteBatch.draw(destRect, glm::vec4(0.f,0.f,1.f,1.f), texture_.id, 0.f, color_);
    
}
