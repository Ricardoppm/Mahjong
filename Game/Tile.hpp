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

enum class TileType{
    NUMERAL,
    BAMBOO,
    CIRCLE,
    DRAGON,
    WIND,
    FLOWER,
    SEASON
};

struct TileTexture {
    Bengine::GLTexture texture;
    int counter = 0;
    TileType type;
};

class Tile
{
public:
    Tile();
    ~Tile();
    
    void init(const glm::vec2& pos,
              const glm::vec3& dims,
              const glm::ivec3& coordinates,
              TileTexture texture,
              Bengine::ColorRGBA8 color,
              float depth);
    
    void draw(Bengine::SpriteBatch& spriteBatch, bool easyMode = false);

    bool isClicked(const glm::vec2& mouseClick);
    
    bool isSameTileType( const Tile* tile);
    
    // Getters
    const glm::ivec3 getCoordinates() const { return coordinates_; }
    const glm::vec2 getPosition() const { return position_; }
    const GLubyte getTextureId() const { return texture_.texture.id; }
    const TileTexture getTileTexture() const { return texture_; }
    bool isActive() const { return active_;}
    
    // Setters
    void setColor(const Bengine::ColorRGBA8& color) { color_ = color;}
    void setActive(bool active) { active_ = active;}
    void setSelected(bool selected) { selected_ = selected; }
    void setTileTexture(TileTexture& texture) { texture_ = texture; }
    
private:
    glm::vec2 position_;
    glm::vec3 dimensions_;
    glm::ivec3 coordinates_;

    float depth_;
    
    bool active_ = false;
    bool selected_ = false;

    Bengine::ColorRGBA8 color_;
    Bengine::ColorRGBA8 blockColor_;
    TileTexture texture_;
    
};
#endif /* Tile_hpp */
