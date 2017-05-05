//
//  EditorScreen.cpp
//  Mahjong
//
//  Created by Ricardo Martins on 03/05/2017.
//  Copyright © 2017 Ricardo Martins. All rights reserved.
//

#include "EditorScreen.hpp"

#include <Bengine/IMainGame.hpp>


EditorScreen::EditorScreen(Bengine::Window* window):
window_(window)
{
    screenIndex_ = SCREEN_INDEX_EDITOR;
}

EditorScreen::~EditorScreen(){}

int EditorScreen::getNextScreenIndex() const
{
    return SCREEN_INDEX_NO_SCREEN;
}
int EditorScreen::getPreviousScreenIndex() const
{
    return SCREEN_INDEX_MAINMENU;
}

void EditorScreen::build()
{
    
}

void EditorScreen::destroy()
{
}

void EditorScreen::onEntry()
{
    // Initialize UI
    initUI();
    
    // Initialize Debug Renderer
    debugRenderer_.init();
    
    //Initialize SpriteBatch
    tileSpriteBatch_.init();
    
    board_.init(glm::vec2(10.f,13.1f));

    // Initialize camera
    camera_.init(window_->getScreenWidth(), window_->getScreenHeight());
    camera_.setScale(3.f);
    
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
    
    
}

void EditorScreen::onExit()
{
    textureProgram_.dispose();
    
    tileSpriteBatch_.dispose();
    
    gui_.destroy();
}

void EditorScreen::update()
{
    checkInput();
    camera_.update();
    board_.update(game_->getInputManager(), camera_);
}

void EditorScreen::draw()
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.f, 0.f, 0.3f, 1.f);
    
    textureProgram_.use();
    
    // Upload texture uniform
    GLuint textureUniform = textureProgram_.getUniformLocation("mySampler");
    glUniform1i(textureUniform, 0);
    glActiveTexture(GL_TEXTURE0);
    
    // Camera matrix
    glm::mat4 projectionMatrix = camera_.getCameraMatrix();
    GLuint cameraUniform = textureProgram_.getUniformLocation("transformationMatrix");
    glUniformMatrix4fv(cameraUniform, 1, GL_FALSE, &projectionMatrix[0][0]);
    

    
    // Draw tiles
    tileSpriteBatch_.begin(Bengine::GlyphSortType::BACK_TO_FRONT);
    
    board_.draw(tileSpriteBatch_);
    
    tileSpriteBatch_.end();
    tileSpriteBatch_.renderBatch();
    
    // Draw grid lines
    board_.drawDebug(debugRenderer_);
    debugRenderer_.end();
    debugRenderer_.render(camera_.getCameraMatrix(), 2.f);

    
    textureProgram_.unuse();
}

void EditorScreen::initUI()
{
    gui_.init("GUI");
    gui_.loadScheme("TaharezLook.scheme");
    gui_.setFont("DejaVuSans-10");
    
   
    
    
    /*
     gui_.setMouseCursor("TaharezLook/MouseArrow");
     gui_.showMouseCursor();
     SDL_ShowCursor(0);*/
    
}

void EditorScreen::checkInput()
{
    SDL_Event e;
    while ( SDL_PollEvent(&e)  ) {
        game_->onSDLEvent(e);
        gui_.onSDLEvent(e);
        switch (e.type) {
            case SDL_QUIT:
                onExitClicked(CEGUI::EventArgs());
                break;
        }
    }
}

bool EditorScreen::onExitClicked(const CEGUI::EventArgs& e)
{
    currentState_ = Bengine::ScreenState::EXIT_APPLICATION;
    return true;
}
