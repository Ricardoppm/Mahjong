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
    blockColor_ = Bengine::ColorRGBA8(166,166,166,255);
}

Tile::~Tile()
{
}

void Tile::init(const glm::vec2& pos,
                const glm::vec3& dims,
                const glm::ivec3& coordinates,
                TileTexture texture,
                Bengine::ColorRGBA8 color,
                float depth)
{
    position_ = pos;
    dimensions_ = dims;
    coordinates_ = coordinates;
    color_ = color;
    texture_ = texture;
    depth_ = depth;
}

void Tile::draw(Bengine::SpriteBatch &spriteBatch, bool easyMode)
{
    glm::vec4 destRect(position_.x,
                       position_.y,
                       dimensions_.x,
                       dimensions_.y);
    Bengine::ColorRGBA8 color = (easyMode && !active_ && !selected_)? blockColor_ : color_;
    spriteBatch.draw(destRect, glm::vec4(0.f,0.f,1.f,1.f), texture_.texture.id, depth_, color);
}

bool Tile::isClicked(const glm::vec2& mouseClick)
{
    if ( mouseClick.x > (position_.x + dimensions_.z) && mouseClick.x < (position_.x + dimensions_.x) ){
        // X position is correct
        if ( mouseClick.y > (position_.y + dimensions_.z) && mouseClick.y < (position_.y + dimensions_.y) ){
            return true;
        }
    }
    return false;
}

bool Tile::isSameTileType(const Tile* tile)
{
    if ( (tile->texture_.type == texture_.type) && (texture_.type == TileType::FLOWER || texture_.type == TileType::SEASON) ) {
        return true;
    }
    return (tile->texture_.texture.id == texture_.texture.id);
}
