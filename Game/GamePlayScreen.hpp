//
//  GamePlayScreen.hpp
//  Mahjong
//
//  Created by Ricardo Martins on 29/04/2017.
//  Copyright © 2017 Ricardo Martins. All rights reserved.
//

#ifndef GamePlayScreen_hpp
#define GamePlayScreen_hpp

#include <Bengine/IGameScreen.hpp>
#include <Bengine/Window.hpp>
#include <Bengine/GLSLProgram.hpp>
#include <Bengine/SpriteBatch.hpp>
#include <Bengine/Camera2D.hpp>
#include <Bengine/GLTexture.h>
#include <Bengine/InputManager.hpp>
#include <Bengine/DebugRenderer.hpp>

#include "GameBoard.hpp"

const float TILE_DIM_WIDTH = 10.f;
const float TILE_DIM_HEIGHT = 13.1f;

class GamePlayScreen : public Bengine::IGameScreen
{
public:
    GamePlayScreen(Bengine::Window* window);
    ~GamePlayScreen();
    
    // Returns the index of the previous/next screen
    virtual int getNextScreenIndex() const override;
    virtual int getPreviousScreenIndex() const override;
    
    // Called at beginning and end of the application
    virtual void build() override;
    virtual void destroy() override;
    
    // Called when screen enters/exits focus
    virtual void onEntry() override;
    virtual void onExit() override;
    
    // Called in the main game loop
    virtual void update() override;
    virtual void draw() override;
    
private:
    void checkInput();
    
    Bengine::Window* window_;
    
    Bengine::GLSLProgram textureProgram_;
    Bengine::SpriteBatch spriteBatch_;
    Bengine::Camera2D camera_;
    Bengine::InputManager inputManager_;
    
    Bengine::DebugRenderer debugRenderer_;


    GameBoard board_;
};
#endif /* GamePlayScreen_hpp */
