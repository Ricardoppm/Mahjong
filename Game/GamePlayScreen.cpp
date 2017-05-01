//
//  GamePlayScreen.cpp
//  Mahjong
//
//  Created by Ricardo Martins on 29/04/2017.
//  Copyright © 2017 Ricardo Martins. All rights reserved.
//

#include "GamePlayScreen.hpp"

#include <iostream>
#include <SDL2/SDL.h>

#include <Bengine/IMainGame.hpp>
#include <Bengine/ResourceManager.hpp>

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
    return SCREEN_INDEX_NO_SCREEN;
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
    
    // Initialize GUI
    gui_.init("GUI/");
    
    // Initialize Debug Renderer
    debugRenderer_.init();
    
    // Initialize Board
    board_.init(glm::vec2(10.f,13.1f), "Levels/puzzle2.txt");
    
    // Initialize Camera
    camera_.init(window_->getScreenWidth(), window_->getScreenHeight());
    camera_.setScale(5.f);
    
    glm::vec2 boardCenter;
    boardCenter.x = board_.getNumTilesWidth() * TILE_DIM_WIDTH / 2.f;
    boardCenter.y = -board_.getNumTilesHeight() * TILE_DIM_HEIGHT / 2.f;
    camera_.setPosition(boardCenter);

    //Init Shaders
    textureProgram_.compileShaders("Shaders/textureShading.vert", "Shaders/textureShading.frag");
    textureProgram_.addAttribute("vertexPosition");
    textureProgram_.addAttribute("vertexColor");
    textureProgram_.addAttribute("vertexUV");
    textureProgram_.linkShaders();
    
    // Place BackGround
    loadBackground("Textures/Backgrounds/bg1.png");
    
    // Start timer
    startTimer = SDL_GetTicks();
}


void GamePlayScreen::onExit()
{

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
    
    tileSpriteBatch_.begin(Bengine::GlyphSortType::BACK_TO_FRONT);
    
    board_.draw(tileSpriteBatch_);
    
    tileSpriteBatch_.end();
    tileSpriteBatch_.renderBatch();
    
    drawHUD();
    
    // Debug
    //board_.drawDebug(debugRenderer_);
    debugRenderer_.end();
    debugRenderer_.render(camera_.getCameraMatrix(), 2.f);
    
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
    }
}

void GamePlayScreen::loadBackground(const std::string& filePath)
{
    Bengine::GLTexture texture_ = Bengine::ResourceManager::getTexture(filePath);
    
    bgSpriteBatch_.begin();
    
    glm::vec2 cameraPos = camera_.getPosition();
    glm::vec4 destRect;
    destRect.x = cameraPos.x - window_->getScreenWidth() / (camera_.getScale() * 2.f);
    destRect.y = cameraPos.y - window_->getScreenHeight() / (camera_.getScale() *2.f);
    destRect.z = window_->getScreenWidth() / camera_.getScale();
    destRect.w = window_->getScreenHeight() / camera_.getScale();

    
    bgSpriteBatch_.draw(destRect, glm::vec4(0.f,0.f,1.f,1.f), texture_.id, 0.f, Bengine::ColorRGBA8(255,255,255,100));
    
    bgSpriteBatch_.draw(glm::vec4(destRect.x,destRect.y, window_->getScreenWidth()/camera_.getScale(), 7.5f),
                         glm::vec4(0.f,0.f,1.f,1.f),
                         Bengine::ResourceManager::getTexture("Textures/hud.png").id,
                         0.f,
                         Bengine::ColorRGBA8(255,255,255,255));

    
    bgSpriteBatch_.end();
}

void GamePlayScreen::drawHUD()
{
    std::string tileRemain("Tiles Remaining: ");
    tileRemain.append(std::to_string(board_.getTilesRemaining()));
    
    std::string timeText("Time: ");
    timeText.append(std::to_string( (SDL_GetTicks()-startTimer)/1000));
    
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
}

void GamePlayScreen::initUI()
{

}
