//
//  MainMenu.hpp
//  Mahjong
//
//  Created by Ricardo Martins on 03/05/2017.
//  Copyright © 2017 Ricardo Martins. All rights reserved.
//

#ifndef MainMenuScreen_hpp
#define MainMenuScreen_hpp

#include <Bengine/Window.hpp>
#include <Bengine/GUI.hpp>
#include <Bengine/Camera2D.hpp>
#include <Bengine/IGameScreen.hpp>
#include <Bengine/SpriteBatch.hpp>
#include <Bengine/GLSLProgram.hpp>

#include "ScreenIndices.h"

class MainMenuScreen : public Bengine::IGameScreen
{
public:
    MainMenuScreen(Bengine::Window* window);
    ~MainMenuScreen();
    
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
    void initUI();
    void checkInput();
    void loadBackground(const std::string& filePath);
    
    // Button Handling
    bool onNewGameClicked(const CEGUI::EventArgs& e);
    bool onEditorClicked(const CEGUI::EventArgs& e);
    bool onExitClicked(const CEGUI::EventArgs& e);
    
    int nextScreenIndex_;
    
    Bengine::Window* window_;
    Bengine::Camera2D camera_;
    Bengine::GUI gui_;
    Bengine::GLSLProgram textureProgram_;
    Bengine::SpriteBatch spriteBatch_;
    
    
};
#endif /* MainMenu_hpp */
