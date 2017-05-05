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
#include <Bengine/SpriteFont.hpp>
#include <Bengine/GUI.hpp>

#include "GameBoard.hpp"

const int NUM_SHUFFLES = 3;

enum class GameState{ PLAYING, FINISHED};

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
    
    void loadBackground(const std::string& filePath);
    void drawHUD();
    void initUI();
    
    // CEGUI input handlers
    bool onShuffleClick(const CEGUI::EventArgs& e);
    bool onExitClick(const CEGUI::EventArgs& e);
    bool onRestartClick(const CEGUI::EventArgs& e);

    // Private member    
    Bengine::Window* window_;
    
    Bengine::GLSLProgram textureProgram_;
    
    Bengine::SpriteBatch tileSpriteBatch_;
    Bengine::SpriteBatch hudSpriteBatch_;
    Bengine::SpriteBatch bgSpriteBatch_;
    
    Bengine::SpriteFont* spriteFont_;
    
    Bengine::Camera2D camera_;
    
    Bengine::DebugRenderer debugRenderer_;
    
    Bengine::GUI gui_;

    Uint32 startTimer;
    
    GameBoard board_;
    
    int shuffleRemaining_ = NUM_SHUFFLES;
    
    GameState gameState_ = GameState::PLAYING;
    
    // CEGUI elements
    CEGUI::PushButton* shuffleButton_;
    CEGUI::PushButton* exitButton_;
    CEGUI::PushButton* restartButton_;

};
#endif /* GamePlayScreen_hpp */
