//
//  MainMenu.cpp
//  Mahjong
//
//  Created by Ricardo Martins on 03/05/2017.
//  Copyright © 2017 Ricardo Martins. All rights reserved.
//

#include "MainMenuScreen.hpp"

#include <Bengine/ResourceManager.hpp>

MainMenuScreen::MainMenuScreen(Bengine::Window* window):
window_(window)
{
    screenIndex_ = SCREEN_INDEX_MAINMENU;
}

MainMenuScreen::~MainMenuScreen(){}

int MainMenuScreen::getNextScreenIndex() const
{
    return nextScreenIndex_;
}
int MainMenuScreen::getPreviousScreenIndex() const
{
    return SCREEN_INDEX_NO_SCREEN;
}

void MainMenuScreen::build()
{
    
}

void MainMenuScreen::destroy()
{
}

void MainMenuScreen::onEntry()
{
    // Initialize camera
    camera_.init(window_->getScreenWidth(), window_->getScreenHeight());
    camera_.setScale(1.f);
    
    // Initialize UI
    initUI();
    
    //Init Shaders
    textureProgram_.compileShaders("Shaders/textureShading.vert", "Shaders/textureShading.frag");
    textureProgram_.addAttribute("vertexPosition");
    textureProgram_.addAttribute("vertexColor");
    textureProgram_.addAttribute("vertexUV");
    textureProgram_.linkShaders();
    
    // Place BackGround
    spriteBatch_.init();
    loadBackground("Textures/Backgrounds/mainmenu.png");
}

void MainMenuScreen::onExit()
{
    gui_.destroy();
}

void MainMenuScreen::update()
{
    checkInput();
    camera_.update();
}

void MainMenuScreen::draw()
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(1.f, 1.f, 1.f, 1.f);
    
    textureProgram_.use();
    
    // Upload texture uniform
    GLuint textureUniform = textureProgram_.getUniformLocation("mySampler");
    glUniform1i(textureUniform, 0);
    glActiveTexture(GL_TEXTURE0);
    
    // Camera matrix
    glm::mat4 projectionMatrix = camera_.getCameraMatrix();
    GLuint cameraUniform = textureProgram_.getUniformLocation("transformationMatrix");
    glUniformMatrix4fv(cameraUniform, 1, GL_FALSE, &projectionMatrix[0][0]);
    
    spriteBatch_.renderBatch();

    gui_.draw();
    
    textureProgram_.unuse();
}

void MainMenuScreen::initUI()
{
    gui_.init("GUI");
    gui_.loadScheme("TaharezLook.scheme");
    gui_.setFont("DejaVuSans-10");

    
    // Make Play button
    CEGUI::PushButton* playGameButton = static_cast<CEGUI::PushButton*>(gui_.createWidget("TaharezLook/Button", glm::vec4(0.45f, 0.40f, 0.1f, 0.05f), glm::vec4(0.f), "Play Game Button"));
    playGameButton->setText("New Game!");
    // Set event for button
    playGameButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MainMenuScreen::onNewGameClicked, this));
    
    // Make Play button
    CEGUI::PushButton* EditorButton = static_cast<CEGUI::PushButton*>(gui_.createWidget("TaharezLook/Button", glm::vec4(0.45f, 0.47f, 0.1f, 0.05f), glm::vec4(0.f), "Editor Button"));
    EditorButton->setText("Level Editor!");
    // Set event for button
    EditorButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MainMenuScreen::onEditorClicked, this));
    
    // Make exit button
    CEGUI::PushButton* exitButton = static_cast<CEGUI::PushButton*>(gui_.createWidget("TaharezLook/Button", glm::vec4(0.45f, 0.54f, 0.1f, 0.05f), glm::vec4(0.f), "Exit Button"));
    exitButton->setText("Exit Game!");
    // Set event for button
    exitButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MainMenuScreen::onExitClicked, this));
}

void MainMenuScreen::checkInput()
{
    SDL_Event e;
    while ( SDL_PollEvent(&e)  ) {
        gui_.onSDLEvent(e);
        switch (e.type) {
            case SDL_QUIT:
                onExitClicked(CEGUI::EventArgs());
                break;
        }
    }
}

void MainMenuScreen::loadBackground(const std::string& filePath)
{
    Bengine::GLTexture texture_ = Bengine::ResourceManager::getTexture(filePath);
    
    spriteBatch_.begin();
    
    glm::vec2 cameraPos = camera_.getPosition();
    glm::vec4 destRect;
    destRect.x = cameraPos.x - window_->getScreenWidth() / (camera_.getScale() * 2.f);
    destRect.y = cameraPos.y - window_->getScreenHeight() / (camera_.getScale() *2.f);
    destRect.z = window_->getScreenWidth() / camera_.getScale();
    destRect.w = window_->getScreenHeight() / camera_.getScale();
    
    
    spriteBatch_.draw(destRect, glm::vec4(0.f,0.f,1.f,1.f), texture_.id, 0.f, Bengine::ColorRGBA8(255,255,255,255));
    
    spriteBatch_.end();
}

bool MainMenuScreen::onNewGameClicked(const CEGUI::EventArgs& e)
{
    nextScreenIndex_ = SCREEN_INDEX_GAMEPLAY;
    currentState_ = Bengine::ScreenState::CHANGE_NEXT;
    return true;
}

bool MainMenuScreen::onEditorClicked(const CEGUI::EventArgs& e)
{
    nextScreenIndex_ = SCREEN_INDEX_EDITOR;
    currentState_ = Bengine::ScreenState::CHANGE_NEXT;
    return true;
}

bool MainMenuScreen::onExitClicked(const CEGUI::EventArgs& e)
{
    currentState_ = Bengine::ScreenState::EXIT_APPLICATION;
    return true;
}
