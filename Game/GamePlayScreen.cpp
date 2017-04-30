//
//  GamePlayScreen.cpp
//  Mahjong
//
//  Created by Ricardo Martins on 29/04/2017.
//  Copyright © 2017 Ricardo Martins. All rights reserved.
//

#include "GamePlayScreen.hpp"

#include <iostream>

#include <Bengine/IMainGame.hpp>

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
    spriteBatch_.init();
    
    // Initialize Debug Renderer
    debugRenderer_.init();
    
    // Initialize Board
    board_.init(glm::vec2(10.f,13.1f));
    board_.loadFromFile("Levels/puzzle.txt");
    
    // Initialize Camera
    camera_.init(window_->getScreenWidth(), window_->getScreenHeight());
    camera_.setScale(4.f);
    
    glm::vec2 boardCenter;
    boardCenter.x = board_.getNumTilesWidth() * TILE_DIM_WIDTH / 2.f;
    boardCenter.y = board_.getNumTilesHeight() * TILE_DIM_HEIGHT / 2.f;
    camera_.setPosition(boardCenter);
    
    //Init Shaders
    textureProgram_.compileShaders("Shaders/textureShading.vert", "Shaders/textureShading.frag");
    textureProgram_.addAttribute("vertexPosition");
    textureProgram_.addAttribute("vertexColor");
    textureProgram_.addAttribute("vertexUV");
    textureProgram_.linkShaders();
    
   
}


void GamePlayScreen::onExit()
{

}

void GamePlayScreen::update()
{
    checkInput();
    camera_.update();
}

void GamePlayScreen::draw()
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glClearColor(0.11f, 0.68, 0.24, 1.f);
    
    textureProgram_.use();
    
    // Upload texture uniform
    GLuint textureUniform = textureProgram_.getUniformLocation("mySampler");
    glUniform1i(textureUniform, 0);
    glActiveTexture(GL_TEXTURE0);
    
    // Camera matrix
    glm::mat4 projectionMatrix = camera_.getCameraMatrix();
    GLuint cameraUniform = textureProgram_.getUniformLocation("transformationMatrix");
    glUniformMatrix4fv(cameraUniform, 1, GL_FALSE, &projectionMatrix[0][0]);
    
    spriteBatch_.begin();
    
    board_.draw(spriteBatch_);
    
    spriteBatch_.end();
    spriteBatch_.renderBatch();
    
    // Debug
    board_.drawDebug(debugRenderer_);
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
