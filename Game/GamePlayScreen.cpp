//
//  GamePlayScreen.cpp
//  Mahjong
//
//  Created by Ricardo Martins on 29/04/2017.
//  Copyright © 2017 Ricardo Martins. All rights reserved.
//

#include "GamePlayScreen.hpp"

#include <iostream>
#include <string>
#include <SDL2/SDL.h>

#include <Bengine/IMainGame.hpp>
#include <Bengine/ResourceManager.hpp>
#include <Bengine/IOManager.hpp>
#include "ScreenIndices.h"


GamePlayScreen::GamePlayScreen(Bengine::Window* window):
window_(window)
{
    screenIndex_ = SCREEN_INDEX_GAMEPLAY;
}

GamePlayScreen::~GamePlayScreen(){}

int GamePlayScreen::getNextScreenIndex() const
{
    return SCREEN_INDEX_NO_SCREEN;
}
int GamePlayScreen::getPreviousScreenIndex() const
{
    return SCREEN_INDEX_MAINMENU;
}

void GamePlayScreen::build()
{
}


void GamePlayScreen::destroy()
{
    std::cout << "Destroy\n";
}

void GamePlayScreen::onEntry()
{
    //Initialize SpriteBatch
    tileSpriteBatch_.init();
    bgSpriteBatch_.init();
    hudSpriteBatch_.init();
    
    // Initialize SpriteFont
    spriteFont_= new Bengine::SpriteFont("Fonts/raleway/Raleway-Regular.ttf", 20);
    
    // Initialize Camera
    camera_.init(window_->getScreenWidth(), window_->getScreenHeight());
    camera_.setScale(5.f);
    
    //Init Shaders
    textureProgram_.compileShaders("Shaders/textureShading.vert", "Shaders/textureShading.frag");
    textureProgram_.addAttribute("vertexPosition");
    textureProgram_.addAttribute("vertexColor");
    textureProgram_.addAttribute("vertexUV");
    textureProgram_.linkShaders();
    
    // Place BackGround
    loadBackground("Textures/Backgrounds/mainmenu.png", 255, false);
    
    // Start timer
    startTimer = SDL_GetTicks();
    
    // Init GUI elements
    initUI();
    initPuzzleMenu();
    initScoreBoardUI();
    
    // Initialize Game state
    gameState_ = GameState::SELECTING;
    shuffleRemaining_ = NUM_SHUFFLES;
}


void GamePlayScreen::onExit()
{
    // Clear glsl program
    textureProgram_.dispose();
    
    // Clear spritebatches
    hudSpriteBatch_.dispose();
    bgSpriteBatch_.dispose();
    tileSpriteBatch_.dispose();
    
    // Clear spritefonts
    spriteFont_->dispose();
    
    // Clear guis
    gui_.destroy();
    scoreBoardGUI_.destroy();
    puzzleGUI_.destroy();
    
    // Clear board
    board_.destroy();
}

void GamePlayScreen::update()
{
    checkInput();
    camera_.update();
    board_.update(game_->getInputManager(), camera_);
}

void GamePlayScreen::draw()
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    //glClearColor(0.11f, 0.68, 0.24, 1.f);
    glClearColor(1, 1, 1, 1);
    
    textureProgram_.use();
    
    // Upload texture uniform
    GLuint textureUniform = textureProgram_.getUniformLocation("mySampler");
    glUniform1i(textureUniform, 0);
    glActiveTexture(GL_TEXTURE0);
    
    // Camera matrix
    glm::mat4 projectionMatrix = camera_.getCameraMatrix();
    GLuint cameraUniform = textureProgram_.getUniformLocation("transformationMatrix");
    glUniformMatrix4fv(cameraUniform, 1, GL_FALSE, &projectionMatrix[0][0]);
    
    bgSpriteBatch_.renderBatch();
    
    switch (gameState_) {
        case GameState::SELECTING:
            puzzleGUI_.draw();
            break;
            
        case GameState::PLAYING:
            
            tileSpriteBatch_.begin(Bengine::GlyphSortType::BACK_TO_FRONT);
            
            board_.draw(tileSpriteBatch_, easyMode_ );
            
            tileSpriteBatch_.end();
            tileSpriteBatch_.renderBatch();
            
            drawHUD();
            break;
            
        case GameState::FINISHED:
            scoreBoardGUI_.draw();
            break;
    }
    textureProgram_.unuse();
}


// Private Functions
void GamePlayScreen::checkInput()
{
    SDL_Event e;
    while ( SDL_PollEvent(&e)  ) {
        game_->onSDLEvent(e);
        switch (e.type) {
            case SDL_QUIT:
                currentState_ = Bengine::ScreenState::EXIT_APPLICATION;
                break;
        }
        
        switch (gameState_) {
            case GameState::SELECTING:
                puzzleGUI_.onSDLEvent(e);
                break;
                
            case GameState::PLAYING :
                if( board_.getTilesRemaining()==0){
                    gameState_ = GameState::FINISHED;
                    // Update ScoreBoard Message
                    setupScoreBoard(true);
                } else if( board_.getNumPairsAvailable() == 0 && shuffleRemaining_==0){
                    gameState_ = GameState::FINISHED;
                    // Update ScoreBoard Message
                    setupScoreBoard(false);
                }
                
                gui_.onSDLEvent(e);
                break;
                
            case GameState::FINISHED:
                scoreBoardGUI_.onSDLEvent(e);
                break;
        }
    }
}

void GamePlayScreen::loadBackground(const std::string& filePath, int alpha, bool loadHUD)
{
    Bengine::GLTexture texture_ = Bengine::ResourceManager::getTexture(filePath);
    
    bgSpriteBatch_.begin(Bengine::GlyphSortType::BACK_TO_FRONT);
    
    glm::vec2 cameraPos = camera_.getPosition();
    glm::vec4 destRect;
    destRect.x = cameraPos.x - window_->getScreenWidth() / (camera_.getScale() * 2.f);
    destRect.y = cameraPos.y - window_->getScreenHeight() / (camera_.getScale() *2.f);
    destRect.z = window_->getScreenWidth() / camera_.getScale();
    destRect.w = window_->getScreenHeight() / camera_.getScale();
    
    
    bgSpriteBatch_.draw(destRect, glm::vec4(0.f,0.f,1.f,1.f), texture_.id, 0.2f, Bengine::ColorRGBA8(255,255,255,alpha));
    
    
    if( loadHUD){
        bgSpriteBatch_.draw(glm::vec4(destRect.x,destRect.y, window_->getScreenWidth()/camera_.getScale(), 7.5f),
                            glm::vec4(0.f,0.f,1.f,1.f),
                            Bengine::ResourceManager::getTexture("Textures/hud.png").id,
                            0.f,
                            hudColor_);
        
    }
    
    
    bgSpriteBatch_.end();
}

void GamePlayScreen::drawHUD()
{
    std::string tileRemain("Tiles Remaining: ");
    tileRemain.append(std::to_string(board_.getTilesRemaining()));
    
    std::string timeText("Time: ");
    timeText.append(std::to_string( (SDL_GetTicks()-startTimer)/1000));
    timeText.append("s");
    
    std::string pairText("Available Pairs: ");
    pairText.append(std::to_string( board_.getNumPairsAvailable()));
    
    hudSpriteBatch_.begin();
    
    glm::vec2 cameraPos = camera_.getPosition();
    
    float height = cameraPos.y - window_->getScreenHeight()/(camera_.getScale()*2.f);
    float width = cameraPos.x - window_->getScreenWidth()/(camera_.getScale()*2.f);
    
    spriteFont_->draw(hudSpriteBatch_,
                      tileRemain.c_str(),
                      glm::vec2( width + 10.f, height + 1.5f),
                      glm::vec2(0.18f),
                      0.0f,
                      Bengine::ColorRGBA8(0,0,0,255));
    
    spriteFont_->draw(hudSpriteBatch_,
                      timeText.c_str(),
                      glm::vec2(camera_.getPosition().x  , height + 1.5f),
                      glm::vec2(0.18f),
                      0.0f,
                      Bengine::ColorRGBA8(0,0,0,255),
                      Bengine::Justification::MIDDLE);
    
    spriteFont_->draw(hudSpriteBatch_,
                      pairText.c_str(),
                      glm::vec2(camera_.getPosition().x + 75.f  , height + 1.5f),
                      glm::vec2(0.18f),
                      0.0f,
                      Bengine::ColorRGBA8(0,0,0,255),
                      Bengine::Justification::MIDDLE);
    
    
    hudSpriteBatch_.end();
    
    hudSpriteBatch_.renderBatch();
    
    gui_.draw();
    
    glEnable(GL_BLEND);
}

void GamePlayScreen::initUI()
{
    // Initialize GUI
    gui_.init("GUI");
    gui_.loadScheme("TaharezLook.scheme");
    gui_.setFont("DejaVuSans-10");
    
    {// Shuffle, Restart and exit Buttons
        const float X_POS = 0.87f;
        const float Y_POS = 0.05f;
        const float X_DIM = 0.1f;
        const float Y_DIM = 0.05f;
        const float PADDING = 0.07f;
        const float DIMS_PIXELS = 20.0f;
        
        shuffleButton_ = static_cast<CEGUI::PushButton*>(gui_.createWidget("TaharezLook/Button", glm::vec4(X_POS, Y_POS, X_DIM, Y_DIM), glm::vec4(0.0f), "ShuffleButton"));
        shuffleButton_->setText("Shuffle (3)");
        shuffleButton_->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&GamePlayScreen::onShuffleClick, this));
        
        exitButton_ = static_cast<CEGUI::PushButton*>(gui_.createWidget("TaharezLook/Button", glm::vec4(X_POS, Y_POS + PADDING, X_DIM, Y_DIM), glm::vec4(0.0f), "ExitButton"));
        exitButton_->setText("Main Menu");
        exitButton_->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&GamePlayScreen::onExitClick, this));
        
        restartButton_ = static_cast<CEGUI::PushButton*>(gui_.createWidget("TaharezLook/Button", glm::vec4(X_POS, Y_POS + 2*PADDING, X_DIM, Y_DIM), glm::vec4(0.0f), "RestartButton"));
        restartButton_->setText("Restart");
        restartButton_->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&GamePlayScreen::onRestartClick, this));
        
        easyToggle_ = static_cast<CEGUI::ToggleButton*>(gui_.createWidget("TaharezLook/Checkbox", glm::vec4(X_POS, Y_POS + PADDING*3, 0.0f, 0.0f), glm::vec4(0.0f, 0.0f, DIMS_PIXELS, DIMS_PIXELS), "DebugToggle"));
        easyToggle_->setSelected(false);
        easyToggle_->subscribeEvent(CEGUI::ToggleButton::EventMouseClick, CEGUI::Event::Subscriber(&GamePlayScreen::onEasyToggleClick, this));
        //widgetLabels_.emplace_back(debugToggle_, "Debug", TEXT_SCALE);
        easyMode_ = false;
    }
}

void GamePlayScreen::initPuzzleMenu()
{
    // Initialize GUI
    puzzleGUI_.init("GUI");
    puzzleGUI_.loadScheme("TaharezLook.scheme");
    puzzleGUI_.setFont("DejaVuSans-10");
    
    {// 5 puzzles and Custom buttons
        const float X_POS = 0.125f;
        const float Y_POS = 0.2f;
        const float X_DIM = 0.2f;
        const float Y_DIM = 0.25f;
        const float X_PADDING = 0.075f;
        const float Y_PADDING = 0.05f;
        
        // Spider
        CEGUI::PushButton* puzzleButton_ = static_cast<CEGUI::PushButton*>(puzzleGUI_.createWidget("TaharezLook/PuzzleButton", glm::vec4(X_POS, Y_POS, X_DIM, Y_DIM), glm::vec4(0.0f), "PuzzleSpider"));
        puzzleButton_->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&GamePlayScreen::onSpiderClick, this));
        puzzleButton_->setProperty("NormalImage", "TaharezLook/puzzlespider");
        puzzleButton_->setProperty("HoverImage", "TaharezLook/puzzlespider");
        puzzleButton_->setProperty("PushedImage", "TaharezLook/puzzlespider");
        puzzleButton_->setProperty("DisabledImage", "TaharezLook/puzzlespider");
        
        // Cat
        puzzleButton_ = static_cast<CEGUI::PushButton*>(puzzleGUI_.createWidget("TaharezLook/PuzzleButton", glm::vec4(X_POS + X_PADDING + X_DIM, Y_POS, X_DIM, Y_DIM), glm::vec4(0.0f), "PuzzleCat"));
        puzzleButton_->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&GamePlayScreen::onCatClick, this));
        puzzleButton_->setProperty("NormalImage", "TaharezLook/puzzlecat");
        puzzleButton_->setProperty("HoverImage", "TaharezLook/puzzlecat");
        puzzleButton_->setProperty("PushedImage", "TaharezLook/puzzlecat");
        puzzleButton_->setProperty("DisabledImage", "TaharezLook/puzzlecat");
        
        // Dragon
        puzzleButton_ = static_cast<CEGUI::PushButton*>(puzzleGUI_.createWidget("TaharezLook/PuzzleButton", glm::vec4(X_POS + 2*(X_PADDING+X_DIM), Y_POS, X_DIM, Y_DIM), glm::vec4(0.0f), "PuzzleDragon"));
        puzzleButton_->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&GamePlayScreen::onDragonClick, this));
        puzzleButton_->setProperty("NormalImage", "TaharezLook/puzzledragon");
        puzzleButton_->setProperty("HoverImage", "TaharezLook/puzzledragon");
        puzzleButton_->setProperty("PushedImage", "TaharezLook/puzzledragon");
        puzzleButton_->setProperty("DisabledImage", "TaharezLook/puzzledragon");
        
        // Tower
        puzzleButton_ = static_cast<CEGUI::PushButton*>(puzzleGUI_.createWidget("TaharezLook/PuzzleButton", glm::vec4(X_POS, Y_POS + Y_PADDING + Y_DIM, X_DIM, Y_DIM), glm::vec4(0.0f), "PuzzleTower"));
        puzzleButton_->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&GamePlayScreen::onTowerClick, this));
        puzzleButton_->setProperty("NormalImage", "TaharezLook/puzzletower");
        puzzleButton_->setProperty("HoverImage", "TaharezLook/puzzletower");
        puzzleButton_->setProperty("PushedImage", "TaharezLook/puzzletower");
        puzzleButton_->setProperty("DisabledImage", "TaharezLook/puzzletower");
        
        // Turtle
        puzzleButton_ = static_cast<CEGUI::PushButton*>(puzzleGUI_.createWidget("TaharezLook/PuzzleButton", glm::vec4(X_POS + X_PADDING + X_DIM, Y_POS + Y_PADDING + Y_DIM, X_DIM, Y_DIM), glm::vec4(0.0f), "PuzzleTurtle"));
        puzzleButton_->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&GamePlayScreen::onTurtleClick, this));
        puzzleButton_->setProperty("NormalImage", "TaharezLook/puzzleturtle");
        puzzleButton_->setProperty("HoverImage", "TaharezLook/puzzleturtle");
        puzzleButton_->setProperty("PushedImage", "TaharezLook/puzzleturtle");
        puzzleButton_->setProperty("DisabledImage", "TaharezLook/puzzleturtle");
        
        // Custom
        puzzleButton_ = static_cast<CEGUI::PushButton*>(puzzleGUI_.createWidget("TaharezLook/PuzzleButton", glm::vec4(X_POS + 2*(X_PADDING + X_DIM), Y_POS + Y_PADDING + Y_DIM, X_DIM, Y_DIM), glm::vec4(0.0f), "PuzzleCustom"));
        puzzleButton_->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&GamePlayScreen::onCustomClick, this));
        puzzleButton_->setProperty("NormalImage", "TaharezLook/puzzlecustom");
        puzzleButton_->setProperty("HoverImage", "TaharezLook/puzzlecustom");
        puzzleButton_->setProperty("PushedImage", "TaharezLook/puzzlecustom");
        puzzleButton_->setProperty("DisabledImage", "TaharezLook/puzzlecustom");
        
        // Exit button
        exitSelectionButton_ = static_cast<CEGUI::PushButton*>(puzzleGUI_.createWidget("TaharezLook/Button", glm::vec4(0.45f, 0.85, 0.1f, 0.05f), glm::vec4(0.0f), "ExitSelectionButton"));
        exitSelectionButton_->setText("Exit");
        exitSelectionButton_->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&GamePlayScreen::onExitClick, this));

    }
    
    { // Add load window widgets
        customWindow_ = static_cast<CEGUI::FrameWindow*>(puzzleGUI_.createWidget("TaharezLook/FrameWindow", glm::vec4(0.3f, 0.3f, 0.4f, 0.4f), glm::vec4(0.0f), "CustomWindow"));
        customWindow_->subscribeEvent(CEGUI::FrameWindow::EventCloseClicked, CEGUI::Event::Subscriber(&GamePlayScreen::onCustomCancelClick, this));
        customWindow_->setText("Load Custom Level");
        // Don't let user drag window around
        customWindow_->setDragMovingEnabled(false);
        
        // Children of loadWindow
        customWindowCombobox_ = static_cast<CEGUI::Combobox*>(puzzleGUI_.createWidget(customWindow_, "TaharezLook/Combobox", glm::vec4(0.1f, 0.1f, 0.8f, 0.4f), glm::vec4(0.0f), "customCombobox"));
        customWindowLoadButton_ = static_cast<CEGUI::PushButton*>(puzzleGUI_.createWidget(customWindow_, "TaharezLook/Button", glm::vec4(0.35f, 0.8f, 0.3f, 0.1f), glm::vec4(0.0f), "customCancelButton"));
        customWindowLoadButton_->setText("Load");
        customWindowLoadButton_->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&GamePlayScreen::onCustomLoadClick, this));
        
        // Start disabled
        customWindow_->setAlpha(0.0f);
        customWindow_->disable();
    }
    
    
}
void GamePlayScreen::initScoreBoardUI()
{
    // Initialize GUI
    scoreBoardGUI_.init("GUI");
    scoreBoardGUI_.loadScheme("TaharezLook.scheme");
    scoreBoardGUI_.setFont("DejaVuSans-10");
    
    
    
    { // Add Scoreboard window widgets
        scoreBoardWindow_ = static_cast<CEGUI::FrameWindow*>(scoreBoardGUI_.createWidget("TaharezLook/FrameWindow", glm::vec4(0.3f, 0.35f, 0.4f, 0.3f), glm::vec4(0.0f), "ScoreBoard"));
        scoreBoardWindow_->subscribeEvent(CEGUI::FrameWindow::EventCloseClicked, CEGUI::Event::Subscriber(&GamePlayScreen::onScoreCancelClick, this));
        scoreBoardWindow_->setText("ScoreBoard!");
        // Don't let user drag window around
        scoreBoardWindow_->setDragMovingEnabled(false);
        
        winLossText_ = static_cast<CEGUI::DefaultWindow*>(scoreBoardGUI_.createWidget(scoreBoardWindow_, "TaharezLook/Label", glm::vec4(0.f, 0.2f, 1.f, 0.2f), glm::vec4(0.0f), "ScoreWinLabel"));
        winLossText_->setText("YOU WIN!");
        
        scoreText_ = static_cast<CEGUI::DefaultWindow*>(scoreBoardGUI_.createWidget(scoreBoardWindow_, "TaharezLook/Label", glm::vec4(0.f, 0.5f, 1.f, 0.1f), glm::vec4(0.0f), "ScoreResultLabel"));
        std::string text("Time: 53s  -  ");
        text.append("using ");
        text.append(std::to_string(3-shuffleRemaining_));
        text.append(" shuffles");
        scoreText_->setText(text);
        
        scoreWindowRestartButton_ = static_cast<CEGUI::PushButton*>(scoreBoardGUI_.createWidget(scoreBoardWindow_, "TaharezLook/Button", glm::vec4(0.2f, 0.8f, 0.25f, 0.1f), glm::vec4(0.0f), "ScoreRestartButton"));
        scoreWindowExitButton_ = static_cast<CEGUI::PushButton*>(scoreBoardGUI_.createWidget(scoreBoardWindow_, "TaharezLook/Button", glm::vec4(0.55f, 0.8f, 0.25f, 0.1f), glm::vec4(0.0f), "ScoreExitButton"));
        
        scoreWindowRestartButton_->setText("New Puzzle");
        scoreWindowRestartButton_->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&GamePlayScreen::onScoreRestartClick, this));
        
        scoreWindowExitButton_->setText("Exit");
        scoreWindowExitButton_->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&GamePlayScreen::onScoreExitClick, this));
        
        
        // Start enabled
        scoreBoardWindow_->setAlpha(1.0f);
        scoreBoardWindow_->enable();
    }
    
    
}

void GamePlayScreen::setupScoreBoard(bool isWin)
{
    winLossText_->setText( isWin? "You Win!" : "You Lose!");
    std::string text("Time: ");
    text.append( std::to_string( (SDL_GetTicks()-startTimer)/1000) );
    text.append("s - using ");
    text.append(std::to_string(3-shuffleRemaining_));
    text.append(" shuffles");
    scoreText_->setText(text);
}


// Button Handlers
bool GamePlayScreen::onShuffleClick(const CEGUI::EventArgs& e) {
    if (shuffleRemaining_ == 0) {
        return true;
    }
    std::string text("Shuffle (" + std::to_string(--shuffleRemaining_) + ")");
    shuffleButton_->setText(text);
    board_.shuffle();
    return true;
}

bool GamePlayScreen::onExitClick(const CEGUI::EventArgs& e) {
    //currentState_ = Bengine::ScreenState::CHANGE_PREVIOUS;
    gameState_ = GameState::FINISHED;
    return true;
}

bool GamePlayScreen::onRestartClick(const CEGUI::EventArgs& e) {
    board_.restart();
    startTimer = SDL_GetTicks();
    return true;
}

bool GamePlayScreen::onEasyToggleClick(const CEGUI::EventArgs& e) {
    easyMode_ = easyToggle_->isSelected();
    return true;
}

bool GamePlayScreen::onScoreCancelClick(const CEGUI::EventArgs& e){
    currentState_ = Bengine::ScreenState::CHANGE_PREVIOUS;
    return true;
}
bool GamePlayScreen::onScoreRestartClick(const CEGUI::EventArgs& e){
    gameState_ = GameState::SELECTING;
    loadBackground("Textures/Backgrounds/mainmenu.png", 255, false);
    board_.destroy();
    return true;
}

bool GamePlayScreen::onScoreExitClick (const CEGUI::EventArgs& e){
    currentState_ = Bengine::ScreenState::CHANGE_PREVIOUS;
    return true;
}

bool GamePlayScreen::onSpiderClick (const CEGUI::EventArgs& e){
    // Initialize Board
    board_.init(glm::vec2(10.f,13.1f), "Levels/Base/Spider");
    
    glm::vec2 boardCenter;
    boardCenter.x = board_.getNumTilesWidth() * TILE_DIM_WIDTH / 2.f;
    boardCenter.y = -board_.getNumTilesHeight() * TILE_DIM_HEIGHT / 2.f;
    camera_.setPosition(boardCenter);
    
    // Place BackGround
    hudColor_ = Bengine::ColorRGBA8(255, 204, 128,255);
    loadBackground("Textures/Backgrounds/spider.png", 100);
    
    gameState_ = GameState::PLAYING;
    startTimer = SDL_GetTicks();

    return true;
}

bool GamePlayScreen::onCatClick (const CEGUI::EventArgs& e){
    // Initialize Board
    board_.init(glm::vec2(10.f,13.1f), "Levels/Base/Cat");
    
    glm::vec2 boardCenter;
    boardCenter.x = board_.getNumTilesWidth() * TILE_DIM_WIDTH / 2.f;
    boardCenter.y = -board_.getNumTilesHeight() * TILE_DIM_HEIGHT / 2.f;
    camera_.setPosition(boardCenter);
    
    // Place BackGround
    hudColor_ = Bengine::ColorRGBA8(153, 255, 153,255);
    loadBackground("Textures/Backgrounds/cat.png", 100);
    
    gameState_ = GameState::PLAYING;
    startTimer = SDL_GetTicks();

    return true;
}

bool GamePlayScreen::onDragonClick (const CEGUI::EventArgs& e){
    // Initialize Board
    board_.init(glm::vec2(10.f,13.1f), "Levels/Base/Dragon");
    
    glm::vec2 boardCenter;
    boardCenter.x = board_.getNumTilesWidth() * TILE_DIM_WIDTH / 2.f;
    boardCenter.y = -board_.getNumTilesHeight() * TILE_DIM_HEIGHT / 2.f;
    camera_.setPosition(boardCenter);
    
    // Place BackGround
    hudColor_ = Bengine::ColorRGBA8(255, 128, 128,255);
    loadBackground("Textures/Backgrounds/dragon.png", 100);
    
    gameState_ = GameState::PLAYING;
    startTimer = SDL_GetTicks();

    return true;
}

bool GamePlayScreen::onTowerClick (const CEGUI::EventArgs& e){
    // Initialize Board
    board_.init(glm::vec2(10.f,13.1f), "Levels/Base/Tower");
    
    glm::vec2 boardCenter;
    boardCenter.x = board_.getNumTilesWidth() * TILE_DIM_WIDTH / 2.f;
    boardCenter.y = -board_.getNumTilesHeight() * TILE_DIM_HEIGHT / 2.f;
    camera_.setPosition(boardCenter);
    
    // Place BackGround
    hudColor_ = Bengine::ColorRGBA8(217, 179, 140,255);
    loadBackground("Textures/Backgrounds/tower.png", 100);
    
    gameState_ = GameState::PLAYING;
    startTimer = SDL_GetTicks();

    return true;
}

bool GamePlayScreen::onTurtleClick (const CEGUI::EventArgs& e){
    // Initialize Board
    board_.init(glm::vec2(10.f,13.1f), "Levels/Base/Turtle");
    
    glm::vec2 boardCenter;
    boardCenter.x = board_.getNumTilesWidth() * TILE_DIM_WIDTH / 2.f;
    boardCenter.y = -board_.getNumTilesHeight() * TILE_DIM_HEIGHT / 2.f;
    camera_.setPosition(boardCenter);
    
    // Place BackGround
    hudColor_ = Bengine::ColorRGBA8(179, 218, 255,255);
    loadBackground("Textures/Backgrounds/turtle.png", 100);
    
    gameState_ = GameState::PLAYING;
    startTimer = SDL_GetTicks();

    return true;
}

bool GamePlayScreen::onCustomClick (const CEGUI::EventArgs& e){
    customWindowCombobox_->clearAllSelections();
    
    // Remove all items
    for (auto& item : customListBoxItems_) {
        // We don't have to call delete since removeItem does it for us
        customWindowCombobox_->removeItem(item);
    }
    customListBoxItems_.clear();
    
    // Get all directory entries
    std::vector<Bengine::DirEntry> entries;
    Bengine::IOManager::getDirectoryEntries("Levels/Custom", entries);
    
    
    // Add all files to list box
    for (auto& e : entries) {
        // Don't add directories
        if (!e.isDirectory) {
            // Remove "Levels/" substring
            e.path.erase(0, std::string("Levels/Custom/").size());
            customListBoxItems_.push_back(new CEGUI::ListboxTextItem(e.path));
            customWindowCombobox_->addItem(customListBoxItems_.back());
        }
    }
    
    customWindow_->enable();
    customWindow_->setAlpha(1.0f);
    customWindow_->moveToFront();
    
    return true;
}

bool GamePlayScreen::onCustomCancelClick (const CEGUI::EventArgs& e)
{
    customWindow_->disable();
    customWindow_->setAlpha(0.0f);
    return true;
}

bool GamePlayScreen::onCustomLoadClick (const CEGUI::EventArgs& e)
{
    puts("Loading game...");
    std::string path = "Levels/Custom/" + std::string(customWindowCombobox_->getText().c_str());
    
    // Load from file
    board_.init(glm::vec2(10.f,13.1f), path);
    
    glm::vec2 boardCenter;
    boardCenter.x = board_.getNumTilesWidth() * TILE_DIM_WIDTH / 2.f;
    boardCenter.y = -board_.getNumTilesHeight() * TILE_DIM_HEIGHT / 2.f;
    camera_.setPosition(boardCenter);
    
    // Place BackGround
    hudColor_ = Bengine::ColorRGBA8(255, 179, 255,255);
    loadBackground("Textures/Backgrounds/custom.png", 100);
    
    gameState_ = GameState::PLAYING;
    startTimer = SDL_GetTicks();
    
    customWindow_->disable();
    customWindow_->setAlpha(0.0f);
    
    return true;
}


