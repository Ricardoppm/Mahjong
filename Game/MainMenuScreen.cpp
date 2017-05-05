//
//  MainMenu.cpp
//  Mahjong
//
//  Created by Ricardo Martins on 03/05/2017.
//  Copyright © 2017 Ricardo Martins. All rights reserved.
//

#include "MainMenuScreen.hpp"

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
    glClearColor(0.f, 0.f, 0.3f, 1.f);
    
    gui_.draw();
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

    /*
    gui_.setMouseCursor("TaharezLook/MouseArrow");
    gui_.showMouseCursor();
    SDL_ShowCursor(0);*/
    
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
