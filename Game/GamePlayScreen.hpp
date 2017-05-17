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

enum class GameState{SELECTING, PLAYING, FINISHED};

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
    
    void loadBackground(const std::string& filePath,int alpha, bool loadHUD = true);
    void drawHUD();
    void initUI();
    void initPuzzleMenu();
    void initScoreBoardUI();
    void setupScoreBoard(bool isWin);
    
    // CEGUI input handlers
    bool onShuffleClick(const CEGUI::EventArgs& e);
    bool onExitClick(const CEGUI::EventArgs& e);
    bool onRestartClick(const CEGUI::EventArgs& e);
    bool onEasyToggleClick(const CEGUI::EventArgs& e);

    bool onScoreCancelClick(const CEGUI::EventArgs& e);
    bool onScoreRestartClick(const CEGUI::EventArgs& e);
    bool onScoreExitClick(const CEGUI::EventArgs& e);
    // Puzzle Selectors
    bool onSpiderClick(const CEGUI::EventArgs& e);
    bool onCatClick(const CEGUI::EventArgs& e);
    bool onDragonClick(const CEGUI::EventArgs& e);
    bool onTowerClick(const CEGUI::EventArgs& e);
    bool onTurtleClick(const CEGUI::EventArgs& e);
    bool onCustomClick(const CEGUI::EventArgs& e);
    bool onCustomCancelClick(const CEGUI::EventArgs& e);
    bool onCustomLoadClick(const CEGUI::EventArgs& e);

    // Private member
    Bengine::Window* window_;
    
    Bengine::GLSLProgram textureProgram_;
    
    Bengine::SpriteBatch tileSpriteBatch_;
    Bengine::SpriteBatch hudSpriteBatch_;
    Bengine::SpriteBatch bgSpriteBatch_;
    
    Bengine::SpriteFont* spriteFont_;
    
    Bengine::Camera2D camera_;
    
    Bengine::GUI gui_;
    Bengine::GUI scoreBoardGUI_;
    Bengine::GUI puzzleGUI_;
    
    Bengine::ColorRGBA8 hudColor_;
    
    Uint32 startTimer;
    
    GameBoard board_;
    
    int shuffleRemaining_ = NUM_SHUFFLES;
    bool easyMode_ = false;
    
    GameState gameState_ = GameState::SELECTING;
    
    // CEGUI elements
    CEGUI::PushButton* shuffleButton_ = nullptr;
    CEGUI::PushButton* exitButton_ = nullptr;
    CEGUI::PushButton* restartButton_ = nullptr;
    CEGUI::ToggleButton* easyToggle_ = nullptr;
    CEGUI::PushButton* exitSelectionButton_ = nullptr;


    // Score board
    CEGUI::FrameWindow* scoreBoardWindow_ = nullptr;
    CEGUI::PushButton* scoreWindowRestartButton_ = nullptr;
    CEGUI::PushButton* scoreWindowExitButton_ = nullptr;
    CEGUI::DefaultWindow* winLossText_ = nullptr;
    CEGUI::DefaultWindow* scoreText_ = nullptr;
    // Load functionality
    CEGUI::FrameWindow* customWindow_ = nullptr;
    CEGUI::PushButton* customWindowLoadButton_ = nullptr;
    CEGUI::Combobox* customWindowCombobox_ = nullptr;
    std::vector<CEGUI::ListboxTextItem*> customListBoxItems_;
    
    
};
#endif /* GamePlayScreen_hpp */
